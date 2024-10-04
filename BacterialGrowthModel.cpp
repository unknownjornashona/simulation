#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp> // JSON 库

using json = nlohmann::json;

const int DEFAULT_GRID_SIZE = 10;
const int DEFAULT_INITIAL_POPULATION = 10;
const double DEFAULT_GROWTH_RATE = 0.3;
const double DEFAULT_DEATH_RATE = 0.1;
const double DEFAULT_RESOURCE_LIMIT = 20;

class Cell {
public:
    int population;
    double resources;

    Cell() : population(0), resources(DEFAULT_RESOURCE_LIMIT) {}
};

class EnvironmentalFactors {
public:
    double temperature;
    double pH;
    double nutrientConcentration;

    EnvironmentalFactors(double temp = 25.0, double pHVal = 7.0, double nutrient = 1.0)
        : temperature(temp), pH(pHVal), nutrientConcentration(nutrient) {}
};

class BacterialGrowthModel {
public:
    BacterialGrowthModel(int gridSize, int initialPopulation, double growthRate, double deathRate, const EnvironmentalFactors& envFactors)
        : gridSize(gridSize), initialPopulation(initialPopulation), growthRate(growthRate), deathRate(deathRate), envFactors(envFactors) {
        grid.resize(gridSize, std::vector<Cell>(gridSize));
        initializePopulation();
        generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }

    void simulate(int timeSteps) {
        std::ofstream logFile("simulation_log.txt");
        std::ofstream csvFile("simulation_data.csv");
        csvFile << "Time,Avg Population\n";
        
        for (int t = 0; t < timeSteps; ++t) {
            updatePopulation();
            double avgPopulation = outputAveragePopulation();
            csvFile << t << "," << avgPopulation << "\n";
            logFile << "Time: " << t << ", Avg Population: " << avgPopulation << "\n";
            distributeResources();
        }

        logFile.close();
        csvFile.close();
    }

private:
    std::vector<std::vector<Cell>> grid;
    int gridSize;
    int initialPopulation;
    double growthRate;
    double deathRate;
    EnvironmentalFactors envFactors;
    std::default_random_engine generator;

    void initializePopulation() {
        for (int i = 0; i < initialPopulation; ++i) {
            int x = rand() % gridSize;
            int y = rand() % gridSize;
            grid[x][y].population++;
        }
    }

    void updatePopulation() {
        for (int x = 0; x < gridSize; ++x) {
            for (int y = 0; y < gridSize; ++y) {
                double effectiveGrowthRate = adjustGrowthRate(envFactors.temperature, envFactors.pH, envFactors.nutrientConcentration);
                if (grid[x][y].resources > 0) {
                    if (std::bernoulli_distribution(effectiveGrowthRate)(generator)) {
                        grid[x][y].population++;
                    }
                    if (std::bernoulli_distribution(deathRate)(generator) && grid[x][y].population > 0) {
                        grid[x][y].population--;
                    }
                    if (grid[x][y].population > 0) {
                        grid[x][y].resources -= 1;
                    }
                }
            }
        }
    }

    double adjustGrowthRate(double temperature, double pH, double nutrient) {
        // 这里可以添加基于实际生物学数据调整的逻辑
        return growthRate * (1 + 0.1 * (temperature - 25)) * (1 - fabs(pH - 7) / 14) * (nutrient / 10);
    }

    double outputAveragePopulation() {
        double totalPopulation = 0;
        for (int x = 0; x < gridSize; ++x) {
            for (int y = 0; y < gridSize; ++y) {
                totalPopulation += grid[x][y].population;
            }
        }
        return totalPopulation / (gridSize * gridSize);
    }

    void distributeResources() {
        for (int x = 0; x < gridSize; ++x) {
            for (int y = 0; y < gridSize; ++y) {
                if (grid[x][y].resources < DEFAULT_RESOURCE_LIMIT) {
                    grid[x][y].resources += 1;
                }
            }
        }
    }
};

void loadConfig(const std::string& configFile, int& gridSize, int& initialPopulation, double& growthRate, double& deathRate, EnvironmentalFactors& envFactors) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开配置文件");
    }

    json j;
    file >> j;

    gridSize = j["grid_size"].get<int>();
    initialPopulation = j["initial_population"].get<int>();
    growthRate = j["growth_rate"].get<double>();
    deathRate = j["death_rate"].get<double>();
    envFactors.temperature = j.contains("temperature") ? j["temperature"].get<double>() : 25.0;
    envFactors.pH = j.contains("pH") ? j["pH"].get<double>() : 7.0;
    envFactors.nutrientConcentration = j.contains("nutrient_concentration") ? j["nutrient_concentration"].get<double>() : 1.0;
}

int main(int argc, char* argv[]) {
    int gridSize = DEFAULT_GRID_SIZE;
    int initialPopulation = DEFAULT_INITIAL_POPULATION;
    double growthRate = DEFAULT_GROWTH_RATE;
    double deathRate = DEFAULT_DEATH_RATE;
    EnvironmentalFactors envFactors;
    int timeSteps = 50;

    try {
        if (argc > 1) {
            loadConfig(argv[1], gridSize, initialPopulation, growthRate, deathRate, envFactors);
        }

        BacterialGrowthModel model(gridSize, initialPopulation, growthRate, deathRate, envFactors);
        model.simulate(timeSteps);
    } catch (const std::exception& e) {
        std::cerr << "发生错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
