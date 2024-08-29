#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

class WhiteDwarf {
public:
    WhiteDwarf(float mass) : mass(mass) {}
    float getMass() const { return mass; }
    
private:
    float mass; // 白矮星质量
};

class IaSupernova {
public:
    IaSupernova(WhiteDwarf* star1, WhiteDwarf* star2) {
        totalMass = star1->getMass() + star2->getMass();
        if (totalMass > 1.4f) { // 钱德拉塞卡极限
            explode();
        } else {
            std::cout << "No supernova event. Mass below Chandrasekhar limit." << std::endl;
        }
    }

    void explode() {
        std::cout << "Ia Supernova explosion initiated!" << std::endl;
        luminosityCurve();
    }

    void luminosityCurve() {
        std::cout << "Simulating luminosity curve:" << std::endl;
        for (int t = 0; t <= 100; t += 10) {
            float luminosity = 5 * exp(-0.03 * t); // 简化的光度曲线
            std::cout << "Time: " << t << ", Luminosity: " << luminosity << std::endl;
        }
    }

private:
    float totalMass; // 总质量
};

int main() {
    // 初始化GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ia Supernova Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化GLEW
    glewInit();

    // 创建白矮星实例
    WhiteDwarf* dwarf1 = new WhiteDwarf(0.7f);
    WhiteDwarf* dwarf2 = new WhiteDwarf(0.8f);

    // 创建Ia超新星实例
    IaSupernova* supernova = new IaSupernova(dwarf1, dwarf2);

    // 主循环
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // OpenGL绘制代码（例如绘制光度曲线的图形化表示可在此添加）

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理资源
    delete dwarf1;
    delete dwarf2;
    delete supernova;
    glfwTerminate();
    return 0;
}
