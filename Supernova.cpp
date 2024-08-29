#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glui.h>
#include <vector>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// 粒子结构
struct Particle {
    float position[3];
    float lifespan;
};

// 粒子系统
class Supernova {
public:
    Supernova(int particleCount);
    void update(float deltaTime);
    void render();
    void setParticleCount(int count);

private:
    std::vector<Particle> particles;
    int maxParticles;
};

Supernova::Supernova(int particleCount) : maxParticles(particleCount) {
    particles.resize(maxParticles);
    for (int i = 0; i < maxParticles; ++i) {
        particles[i].position[0] = 0.0f;
        particles[i].position[1] = 0.0f;
        particles[i].position[2] = 0.0f;
        particles[i].lifespan = static_cast<float>(rand() % 100) / 100.0f;
    }
}

void Supernova::update(float deltaTime) {
    for (auto& particle : particles) {
        particle.lifespan -= deltaTime;
        if (particle.lifespan > 0) {
            particle.position[0] += ((rand() % 200) - 100) / 100.0f;
            particle.position[1] += ((rand() % 200) - 100) / 100.0f;
            particle.position[2] += ((rand() % 200) - 100) / 100.0f;
        }
    }
}

void Supernova::render() {
    glBegin(GL_POINTS);
    for (const auto& particle : particles) {
        if (particle.lifespan > 0) {
            glVertex3f(particle.position[0], particle.position[1], particle.position[2]);
        }
    }
    glEnd();
}

void Supernova::setParticleCount(int count) {
    maxParticles = count;
    particles.resize(maxParticles);
}

Supernova* supernova;

// 回调函数用于更新粒子数量
void updateParticleCount(int newCount) {
    supernova->setParticleCount(newCount);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Supernova Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    supernova = new Supernova(500); // 创建超新星实例

    // 创建 GLUI 窗口
    GLUI *glui = GLUI_Master.create_glui("Control");
    int particleCount = 500;
    GLUI_Spinner* particleSpinner = glui->add_spinner("Particle Count", GLUI_SPINNER_INT, &particleCount);
    particleSpinner->set_int_limits(1, 2000);
    particleSpinner->set_callback((GLUI_Update_cb)updateParticleCount);

    // OpenGL 视口设置
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)WIDTH/(float)HEIGHT, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    float lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // 清除窗口
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glPointSize(2.0f);

        // 更新超新星状态
        supernova->update(deltaTime);
        supernova->render();

        // 更新 GLUI
        glui->sync_live();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete supernova; // 清理动态分配的内存
    glfwTerminate();
    return 0;
}
