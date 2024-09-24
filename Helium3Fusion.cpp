#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>

class Helium3Fusion {
public:
    Helium3Fusion(int reactants) : reactantCount(reactants) {
        if (reactantCount < 2) {
            throw std::invalid_argument("需要至少两个氦-3反应物。");
        }
    }

    void performFusionIterations(int iterations) {
        for (int i = 0; i < iterations; ++i) {
            double energyReleased = performFusion();
            logToCSV(reactantCount, energyReleased);
        }
    }

private:
    int reactantCount;

    double performFusion() {
        double energyReleased = 12.860; // 能量释放 (MeV)
        logFusion(reactantCount, energyReleased);
        return energyReleased;
    }

    void logFusion(int reactants, double energy) {
        std::cout << "聚变反应： " << reactants << " * He-3 -> D + He-4, "
                  << "释放能量: " << energy << " MeV" << std::endl;
    }

    void logToCSV(int reactants, double energy) {
        std::ofstream outFile("fusion_data.csv", std::ios::app);
        if (!outFile) {
            throw std::runtime_error("无法打开CSV文件。");
        }
        outFile << reactants << "," << energy << "\n";
        outFile.close();
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "请提供氦-3反应物数量和迭代次数。" << std::endl;
        return 1;
    }

    try {
        int reactantCount = std::stoi(argv[1]); // 从命令行获取氦-3反应物数量
        int iterations = std::stoi(argv[2]);     // 从命令行获取迭代次数

        Helium3Fusion fusion(reactantCount);
        fusion.performFusionIterations(iterations); // 执行指定次数的聚变反应
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl; // 捕捉并显示错误信息
        return 1;
    }

    return 0;
}
