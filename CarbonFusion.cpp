#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

class CarbonFusion {
public:
    CarbonFusion(float temperature, float density)
        : temperature(temperature), density(density) {}

    bool isFusionPossible() const {
        return (temperature >= 6e8) && (density >= 2e8);
    }

    void simulate() const {
        if (isFusionPossible()) {
            std::cout << "Carbon fusion is occurring at T: " << temperature 
                      << " K, D: " << density << " kg/m^3." << std::endl;
        } else {
            std::cout << "Conditions are not suitable for carbon fusion at T: " 
                      << temperature << " K, D: " << density << " kg/m^3." << std::endl;
        }
    }

private:
    float temperature; // 温度 (K)
    float density;     // 密度 (kg/m^3)
};

std::vector<CarbonFusion> loadFusionData(const std::string& filename) {
    std::vector<CarbonFusion> fusionData;
    std::ifstream file(filename);
    std::string line;

    // Skip the header
    if (std::getline(file, line)) {
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string tempStr, densStr;
            if (std::getline(ss, tempStr, ',') && std::getline(ss, densStr, ',')) {
                float temperature = std::stof(tempStr);
                float density = std::stof(densStr);
                fusionData.emplace_back(temperature, density);
            }
        }
    }
    return fusionData;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Carbon Fusion Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewInit();

    // Load carbon fusion data from CSV
    std::vector<CarbonFusion> fusionData = loadFusionData("fusion_data.csv");
    
    // Iterate over data for 100 iterations
    for (int iteration = 0; iteration < 100; ++iteration) {
        std::cout << "Iteration: " << iteration + 1 << std::endl;
        for (const auto& fusion : fusionData) {
            fusion.simulate();
        }

        // Optional: Sleep to simulate time passage or processing
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 0.5 seconds delay
    }

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // OpenGL rendering code (can add for graphical representation)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
