#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>

const int GRID_SIZE = 20;           // 网格大小
const float TIME_STEP = 0.01;       // 时间步长
const int TIME_STEPS = 100;          // 模拟时间步骤

class FluidCell {
public:
    float temperature;
    float density;
    float velocityX;
    float velocityY;

    FluidCell(float temp = 0.0f, float dens = 0.0f) 
        : temperature(temp), density(dens), velocityX(0.0f), velocityY(0.0f) {}
};

class CFDSimulation {
public:
    CFDSimulation(std::vector<FluidCell>& initialGrid) 
        : grid(initialGrid) {}

    void update() {
        // 使用简化的Navier-Stokes方程进行更新
        for (int i = 1; i < GRID_SIZE - 1; ++i) {
            for (int j = 1; j < GRID_SIZE - 1; ++j) {
                // 简化的流体动力学计算
                float pressure = (grid[i * GRID_SIZE + j].density * grid[i * GRID_SIZE + j].temperature) / 1000.0f;

                // 更新速度（动量守恒）
                velocityX = grid[i * GRID_SIZE + j].velocityX - (pressure / grid[i * GRID_SIZE + j].density) * TIME_STEP;
                velocityY = grid[i * GRID_SIZE + j].velocityY - (pressure / grid[i * GRID_SIZE + j].density) * TIME_STEP;

                grid[i * GRID_SIZE + j].velocityX = velocityX;
                grid[i * GRID_SIZE + j].velocityY = velocityY;

                // 更新温度
                grid[i * GRID_SIZE + j].temperature += (0.1f * velocityX + 0.1f * velocityY); // 热源

                // 更新密度
                grid[i * GRID_SIZE + j].density *= 1.001f;  // 简化反应过程
            }
        }
    }

    void simulate(int steps) {
        for (int step = 0; step < steps; ++step) {
            update();
            render();
        }
    }

    void render() const {
        glClear(GL_COLOR_BUFFER_BIT);
        
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                float temp = grid[i * GRID_SIZE + j].temperature;
                float dens = grid[i * GRID_SIZE + j].density;

                // 颜色映射
                glColor3f(temp / 100.0f, 0.0f, dens / 1000.0f);
                
                // 绘制矩形
                glBegin(GL_QUADS);
                glVertex2f(i * 0.1f, j * 0.1f); 
                glVertex2f(i * 0.1f, (j + 1) * 0.1f); 
                glVertex2f((i + 1) * 0.1f, (j + 1) * 0.1f); 
                glVertex2f((i + 1) * 0.1f, j * 0.1f); 
                glEnd();
            }
        }

        glFlush();
    }

private:
    std::vector<FluidCell> grid;
};

std::vector<FluidCell> loadInitialGrid(const std::string& filename) {
    std::vector<FluidCell> initialGrid(GRID_SIZE * GRID_SIZE);
    std::ifstream file(filename);
    std::string line;

    if (std::getline(file, line)) {
        for (int i = 0; i < GRID_SIZE; ++i) {
            if (std::getline(file, line)) {
                std::istringstream ss(line);
                std::string tempStr, densStr;
                float temperature = 0.0, density = 0.0;
                if (std::getline(ss, tempStr, ',') && std::getline(ss, densStr, ',')) {
                    temperature = std::stof(tempStr);
                    density = std::stof(densStr);
                }
                initialGrid[i * GRID_SIZE] = FluidCell(temperature, density);
                for (int j = 1; j < GRID_SIZE; ++j) {
                    initialGrid[i * GRID_SIZE + j] = FluidCell(temperature, density);
                }
            }
        }
    }
    return initialGrid;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 800, "Carbon Fusion CFD Visualization", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    
    // 初始化GLEW
    glewInit();
    
    std::vector<FluidCell> initialGrid = loadInitialGrid("fusion_data.csv");
    
    CFDSimulation simulation(initialGrid);

    while (!glfwWindowShouldClose(window)) {
        simulation.simulate(TIME_STEPS);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
