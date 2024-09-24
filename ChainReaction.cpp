#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cmath> // 引入cmath头文件以使用指数和其他数学函数

class ChainReactionLogger {
private:
    std::ofstream logFile;

public:
    ChainReactionLogger(const std::string& filename) : logFile(filename, std::ios::app) {
        if (!logFile) {
            throw std::runtime_error("无法打开日志文件");
        }
    }

    ~ChainReactionLogger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void log(const std::string& message) {
        logFile << message << std::endl;
    }
};

class ChainReaction {
private:
    double nu;          // 每次裂变生成的中子数量
    double lambda;      // 裂变截面
    double beta;        // 中子的漏失率
    double density;     // 核材料密度（单位：g/cm³）
    double mass;        // 质量（单位：g）
    double diffusion;   // 中子的扩散系数
    double absorption;  // 吸收率
    double temperature; // 温度（单位：K）
    
    ChainReactionLogger logger;

    // 温度对裂变率和漏失率的影响函数
    double calculateFissionRate(double base_rate) {
        return base_rate * (1.0 + 0.01 * (temperature - 300.0)); // 假设在300K时为基准
    }

    double calculateLeakRate(double base_rate) {
        return base_rate * (1.0 + 0.005 * (temperature - 300.0)); // 假设在300K时为基准
    }

public:
    ChainReaction(double nu, double lambda, double beta, double density, double mass, 
                  double diffusion, double absorption, double temperature, 
                  const std::string& logFilename) 
        : nu(nu), lambda(lambda), beta(beta), density(density), mass(mass), 
          diffusion(diffusion), absorption(absorption), temperature(temperature), 
          logger(logFilename) {
        if (lambda <= 0 || beta < 0 || density <= 0 || mass <= 0 || 
            diffusion < 0 || absorption < 0 || temperature <= 0) {
            throw std::invalid_argument("参数必须为非负数，且裂变截面、密度和质量必须为正数");
        }
    }

    void simulate(double endTime, double deltaTime) {
        double N = mass * density;    // 初始原子数
        double I = 1.0;                // 初始中子数量

        std::vector<std::pair<double, double>> results;

        for (double t = 0; t < endTime; t += deltaTime) {
            double currentLambda = calculateFissionRate(lambda);
            double currentBeta = calculateLeakRate(beta);
            double fissionRate = currentLambda * N * I; // 裂变率

            // 限制裂变不会超过当前原子数
            if (fissionRate > N) fissionRate = N;
            N -= fissionRate;

            // 更新中子数量，考虑吸收和泄漏
            I += nu * fissionRate - currentBeta * I - absorption * I; 
            if (I < 0) I = 0; // 防止中子数量为负数

            // 中子的扩散影响
            I += diffusion * (N / (N + I)); // 中子的扩散增加
            if (I < 0) I = 0; // 防止中子数量为负数

            // 限制原子数量不为负数
            if (N < 0) N = 0;

            // 存储数据
            results.emplace_back(t, N);
            logger.log("时间: " + std::to_string(t) + ", 原子核数量: " + std::to_string(N) + ", 中子数量: " + std::to_string(I));
        }

        // 生成CSV数据文件
        std::ofstream csvFile("reaction_data.csv");
        if (!csvFile) {
            throw std::runtime_error("无法打开CSV文件");
        }
        csvFile << "时间(s),原子核数量,中子数量\n";
        for (const auto& result : results) {
            csvFile << result.first << "," << result.second << "\n";
        }
    }
};

int main() {
    try {
        ChainReaction reaction(2.5, 0.007, 0.1, 19.1, 10.0, 0.01, 0.005, 350.0, "reaction_log.txt");
        reaction.simulate(10.0, 0.1);
        std::cout << "模拟完成，数据已输出到 reaction_log.txt 和 reaction_data.csv" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
    return 0;
}
