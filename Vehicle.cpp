#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <ctime>
#include <cstdlib>

// 参数类用于管理超参数设置
class Parameters {
public:
    double initial_epsilon = 1.0;  // 初始探索率
    double min_epsilon = 0.05;      // 最小探索率
    double decay_factor = 0.99;     // 探索率衰减因子
    double initial_alpha = 0.1;      // 初始学习率
    double min_alpha = 0.01;         // 最小学习率
    double decay_alpha = 0.99;       // 学习率衰减因子
    double initial_gamma = 0.9;      // 初始折扣因子
};

// 模糊控制器
class FuzzyController {
public:
    double control(double error) const {
        if (error > 0.1) return 1.0;  // 过快
        else if (error < -0.1) return -1.0; // 过慢
        return 0.0; // 目标达成
    }
};

// Q-learning 代理
class QLearningAgent {
public:
    QLearningAgent(const Parameters& params) 
        : epsilon_(params.initial_epsilon), 
          alpha_(params.initial_alpha), 
          gamma_(params.initial_gamma) {
        srand(static_cast<unsigned int>(time(0)));
    }

    void chooseAction(double state) {
        if (rand() / static_cast<double>(RAND_MAX) < epsilon_) {
            action_ = (rand() % 2) ? 1.0 : -1.0; // 随机选择
        } else {
            action_ = getMaxAction(state); // 选择最佳动作
        }
    }

    void update(double state, double reward) {
        double best_next_action = getMaxAction(state);
        q_table_[state][action_] += alpha_ * (reward + gamma_ * best_next_action - q_table_[state][action_]);

        // 动态调整学习率与探索率
        updateAlpha();
        updateEpsilon();
    }

    double getAction() const {
        return action_;
    }

private:
    double action_;
    double epsilon_, alpha_, gamma_;
    std::unordered_map<double, std::unordered_map<double, double>> q_table_; // Q 表

    double getMaxAction(double state) {
        double max_value = -std::numeric_limits<double>::infinity();
        double best_action = 0.0;

        for (const auto& action_pair : q_table_[state]) {
            if (action_pair.second > max_value) {
                max_value = action_pair.second;
                best_action = action_pair.first;
            }
        }
        return best_action;
    }

    void updateEpsilon() {
        if (epsilon_ > 0.05) { 
            epsilon_ *= decay_factor; // 衰减
        }
    }

    void updateAlpha() {
        if (alpha_ > 0.01) { 
            alpha_ *= decay_alpha; // 衰减
        }
    }
};

// 车辆类
class Vehicle {
public:
    Vehicle(double x, double y, double theta, const Parameters& params)
        : x_(x), y_(y), theta_(theta), v_(0.0), omega_(0.0),
          target_velocity_(0.0), target_angle_(0.0),
          pid_kp_(1.0), pid_ki_(0.1), pid_kd_(0.01),
          integral_(0.0), previous_error_(0.0), 
          rlAgent_(params) {}

    void setControls(double target_velocity, double target_angle) {
        target_velocity_ = target_velocity;
        target_angle_ = target_angle;
    }

    void update(double dt) {
        double velocity_error = target_velocity_ - v_;
        v_ += pidControl(velocity_error, dt);
        
        double angle_error = target_angle_ - theta_;
        omega_ = pidControl(angle_error, dt);

        // 使用模糊控制器调整角速度
        omega_ += fuzzy_.control(angle_error);

        // 使用 Q-learning 代理选择行为
        rlAgent_.chooseAction(angle_error);
        v_ += rlAgent_.getAction(); // 更新速度基于强化学习的动作

        // 更新车辆状态
        x_ += v_ * cos(theta_) * dt;
        y_ += v_ * sin(theta_) * dt;
        theta_ += omega_ * dt;

        // 更新 Q-learning
        double reward = calculateReward();
        rlAgent_.update(angle_error, reward);
    }

    void saveToCSV(const std::string& filename) const {
        std::ofstream csvFile(filename);
        if (!csvFile) {
            throw std::runtime_error("Unable to open CSV file.");
        }
        csvFile << "x,y,theta\n";  // CSV Header
        csvFile << std::fixed << std::setprecision(2) << x_ << "," << y_ << "," << theta_ << "\n";
        csvFile.close();
    }

private:
    double x_, y_, theta_; // 车辆位置和朝向
    double v_, omega_; // 控制输入
    double target_velocity_, target_angle_; // 期望速度和角度

    // PID 控制器参数
    double pid_kp_, pid_ki_, pid_kd_;
    double integral_, previous_error_;
    
    QLearningAgent rlAgent_;
    FuzzyController fuzzy_; // 模糊控制器实例

    double pidControl(double error, double dt) {
        integral_ += error * dt;
        double derivative = (error - previous_error_) / dt;
        previous_error_ = error;

        // PID 输出
        return pid_kp_ * error + pid_ki_ * integral_ + pid_kd_ * derivative;
    }

    double calculateReward() const {
        // 简单的奖励函数设计
        return (std::abs(target_angle_ - theta_) < 0.1) ? 10.0 : -1.0; // 接近目标与远离目标
    }
};

int main() {
    try {
        Parameters params; // 创建参数实例
        Vehicle vehicle(0.0, 0.0, 0.0, params);
        
        // 设置目标控制
        vehicle.setControls(1.0, 0.0);

        // 更新车辆状态
        const double dt = 0.1; // 时间步长
        for (int i = 0; i < 100; ++i) {
            vehicle.update(dt);
        }

        // 保存数据到 CSV 文件
        vehicle.saveToCSV("vehicle_data.csv");

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }

    return 0;
}
