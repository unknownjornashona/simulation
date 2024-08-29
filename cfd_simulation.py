import numpy as np
import pandas as pd
import logging
import os
import tensorflow as tf
from OpenGL.GL import *
from OpenGL.GLU import *
import pygame

# 设置日志配置
logging.basicConfig(filename='ammonia_production.log', level=logging.INFO,
                    format='%(asctime)s - %(levelname)s - %(message)s')


class CFDModel:
    def __init__(self, grid_size):
        self.grid_size = grid_size
        self.velocity_field = np.zeros((grid_size, grid_size, 2))
        self.density_field = np.ones((grid_size, grid_size))

    def initialize_velocity(self):
        """初始化速度场"""
        self.velocity_field[:, :, 0] = 0.1  # 设定初始流动速度

    def apply_boundary_conditions(self):
        """应用边界条件"""
        self.velocity_field[0, :, :] = 0  # 左边界
        self.velocity_field[-1, :, :] = 0  # 右边界
        self.velocity_field[:, 0, :] = 0  # 下边界
        self.velocity_field[:, -1, :] = 0  # 上边界
        self.density_field[0, :] = 1  # 固定边界密度
        self.density_field[-1, :] = 1  # 固定边界密度

    def update_velocity(self):
        """动态更新速度场"""
        advection = np.roll(self.velocity_field, 1, axis=0) + np.roll(self.velocity_field, -1, axis=1) - 0.1 * self.velocity_field
        self.velocity_field = advection
        self.apply_boundary_conditions()

    def compute_speed(self):
        """计算每个网格的速度大小"""
        return np.linalg.norm(self.velocity_field, axis=2)

    def predict_velocity(self, model):
        """使用TensorFlow模型预测下一时刻的速度场"""
        pred_velocities = model.predict(self.velocity_field.reshape((1, self.grid_size, self.grid_size, 2)))
        self.velocity_field = pred_velocities[0]  # 更新速度场为预测值


class NeuralNetworkModel:
    def __init__(self):
        self.model = self.build_model()

    def build_model(self):
        """构建神经网络模型"""
        model = tf.keras.Sequential([
            tf.keras.layers.Input(shape=(20, 20, 2)),
            tf.keras.layers.Conv2D(32, kernel_size=(3, 3), activation='relu'),
            tf.keras.layers.MaxPooling2D(pool_size=(2, 2)),
            tf.keras.layers.Conv2D(64, kernel_size=(3, 3), activation='relu'),
            tf.keras.layers.MaxPooling2D(pool_size=(2, 2)),
            tf.keras.layers.Flatten(),
            tf.keras.layers.Dense(128, activation='relu'),
            tf.keras.layers.Dense(20 * 20 * 2, activation='linear'),
            tf.keras.layers.Reshape((20, 20, 2))  # 重新调整为速度场
        ])
        model.compile(optimizer='adam', loss='mean_squared_error')
        return model

    def train(self, x_train, y_train, epochs=100, batch_size=32):
        """训练神经网络"""
        self.model.fit(x_train, y_train, epochs=epochs, batch_size=batch_size)

    def predict(self, x):
        """进行预测"""
        return self.model.predict(x)


class AmmoniaProduction:
    def __init__(self, temperature_range):
        self.temperatures = temperature_range
        self.production_data = None
        self.cfd_model = CFDModel(grid_size=20)
        self.neural_model = NeuralNetworkModel()

    def calculate_production(self):
        """计算氨的生产量"""
        try:
            self.production_data = np.exp(-((self.temperatures - 450) ** 2) / (2 * 50 ** 2))
            logging.info("氨生产量计算成功")
        except Exception as e:
            logging.error(f"计算氨生产量时出现错误: {e}")
            raise

    def read_csv_data(self, directory):
        """读取指定目录的CSV文件"""
        try:
            csv_files = [f for f in os.listdir(directory) if f.endswith('.csv')]
            data_frames = []
            for file in csv_files:
                df = pd.read_csv(os.path.join(directory, file))
                data_frames.append(df)
            logging.info(f"成功读取 {len(csv_files)} 个 CSV 文件")
            return data_frames
        except Exception as e:
            logging.error(f"读取CSV文件时出现错误: {e}")
            raise

    def display_openGL(self):
        """使用 OpenGL 进行 3D 渲染"""
        pygame.init()
        display = (800, 600)
        pygame.display.set_mode(display, pygame.DOUBLEBUF | pygame.OPENGL)
        gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)
        glTranslatef(0.0, 0.0, -5)

        self.cfd_model.initialize_velocity()

        # 主循环
        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    return

            self.cfd_model.update_velocity()
            
            # 使用神经网络进行下一时刻的速度场预测
            self.cfd_model.predict_velocity(self.neural_model)

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

            self.draw_velocity_field()
            self.draw_heatmap()
            pygame.display.flip()
            pygame.time.wait(100)

    def draw_velocity_field(self):
        """绘制流动速度场"""
        for i in range(self.cfd_model.grid_size):
            for j in range(self.cfd_model.grid_size):
                velocity = self.cfd_model.velocity_field[i, j]
                x = (i - self.cfd_model.grid_size / 2) * 0.5
                y = (j - self.cfd_model.grid_size / 2) * 0.5
                speed = np.linalg.norm(velocity)
                glColor3f(0.0, speed, 0.5)  # 颜色基于速度大小
                glBegin(GL_LINES)
                glVertex3f(x, y, 0)
                glVertex3f(x + velocity[0] * 0.1, y + velocity[1] * 0.1, 0)
                glEnd()

    def draw_heatmap(self):
        """绘制速度云图"""
        speed_field = self.cfd_model.compute_speed()
        max_speed = np.max(speed_field) if np.max(speed_field) > 0 else 1  # 防止除以零

        for i in range(self.cfd_model.grid_size):
            for j in range(self.cfd_model.grid_size):
                speed = speed_field[i, j]
                normalized_speed = speed / max_speed  # 归一化速度
                color = self.get_color_from_speed(normalized_speed)

                # 绘制带颜色的矩形
                x = (i - self.cfd_model.grid_size / 2) * 0.5
                y = (j - self.cfd_model.grid_size / 2) * 0.5
                glColor3f(*color)
                glBegin(GL_QUADS)
                glVertex3f(x - 0.1, y - 0.1, 0)
                glVertex3f(x + 0.1, y - 0.1, 0)
                glVertex3f(x + 0.1, y + 0.1, 0)
                glVertex3f(x - 0.1, y + 0.1, 0)
                glEnd()

    def get_color_from_speed(self, normalized_speed):
        """根据归一化速度生成颜色"""
        r = min(1.0, normalized_speed * 2)
        g = min(1.0, 1 - normalized_speed * 2)
        b = 0.5
        return r, g, b


# 使用示例
if __name__ == "__main__":
    try:
        temperatures = np.linspace(200, 700, 100)  # 温度范围
        ammonia_production = AmmoniaProduction(temperatures)

        # 计算氨生产量
        ammonia_production.calculate_production()

        # 读取CSV数据
        directory = './data'  # 指定CSV文件目录
        csv_data = ammonia_production.read_csv_data(directory)

        # 启动 OpenGL 模拟
        ammonia_production.display_openGL()
    except Exception as e:
        print(f"程序出现异常: {e}")
