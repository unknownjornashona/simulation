#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <filesystem>

class ElNinoModel {
public:
    ElNinoModel(double beta, double gamma, int initial_conditions)
        : beta(beta), gamma(gamma), S(initial_conditions), I(0.0) {
        if (initial_conditions < 0) {
            throw std::invalid_argument("Initial conditions cannot be negative.");
        }
    }

    void simulate(int days) {
        for (int day = 0; day < days; ++day) {
            // 分数調波解の計算ロジックをここに実装
            double new_infected = beta * S * I; // シンプルな感染モデル
            double new_recovered = gamma * I; // 回復者数

            // 状態の更新
            S -= new_infected; 
            I += new_infected - new_recovered;

            // 各人口が負にならないようにクリッピング
            clip_values();
            
            // ログの記録
            logs.push_back(format_log(day, S, I));
        }
    }

    void save_to_csv(const std::string &filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file to save data.");
        }

        file << "Day,Susceptible,Infectious\n";
        for (const auto &log : logs) {
            file << log << "\n";
        }
        file.close();
    }

    void save_log(const std::string &log_filename) {
        std::ofstream log_file(log_filename);
        if (!log_file.is_open()) {
            throw std::runtime_error("Could not open log file.");
        }

        log_file << "Day,Susceptible,Infectious\n";
        for (const auto &log : logs) {
            log_file << log << "\n";
        }
        log_file.close();
    }

private:
    double beta; // 感染率
    double gamma; // 回復率
    double S; // 感受性人口
    double I; // 感染者人口
    std::vector<std::string> logs;

    void clip_values() {
        if (S < 0) S = 0;
        if (I < 0) I = 0;
    }

    std::string format_log(int day, double S, double I) {
        std::ostringstream oss;
        oss << day << "," << std::fixed << std::setprecision(2) << S << "," << I;
        return oss.str();
    }
};

int main() {
    try {
        double beta = 0.2; // 感染率
        double gamma = 0.1; // 回復率
        int initial_conditions = 1000; // 初期条件

        ElNinoModel model(beta, gamma, initial_conditions);
        
        int days = 100; // シミュレーション日数
        model.simulate(days);
        
        // CSVとログファイルの保存
        std::string csv_filename = "elnino_simulation.csv";
        model.save_to_csv(csv_filename);
        
        std::string log_filename = "elnino_log.txt";
        model.save_log(log_filename);
        
        std::cout << "Simulation completed successfully. Data saved to " << csv_filename << " and logs saved to " << log_filename << "." << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
