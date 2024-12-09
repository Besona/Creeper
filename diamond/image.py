import numpy as np
import random

from PIL import Image
from scipy.stats import multivariate_normal


def generate_image(w, h, p, lb, ub, scale, sigma, rv):
    # 创建空白图像
    image = Image.new('RGB', (w, h))
    pixels = image.load()

    # 生成中心点，仅在倒三角区域内
    points = []
    for _ in range(p):
        x = random.randint(0, w - 1)
        y = random.randint(x * h // w, h - 1)  # 确保在倒三角形内
        value = random.randint(lb, ub)  # 使用正态分布生成随机值
        points.append((x, y, value))

    # 为每个像素计算所有中心点的影响
    for x in range(w):
        for y in range(h):
            if y >= x * h // w:  # 确保在倒三角形内
                pixel_value = 0
                for px, py, pvalue in points:
                    dx = scale * (x - px)
                    dy = scale * (y - py)
                    distance = np.sqrt(dx ** 2 + dy ** 2)
                    if distance > 3.0 * sigma:
                        continue
                    contribution = pvalue * rv.pdf((dx, dy))
                    pixel_value += contribution

                pixel_value = min(max(int(pixel_value), 0), 255)
                pixels[x, y] = (255, 255 - pixel_value, 255)  # 将计算值用于红色通道

    image = image.transpose(Image.FLIP_TOP_BOTTOM)

    return image


def generate_noise_image(w, h, noise_ratio):
    # 创建全黑图像
    image = np.zeros((w, h, 3), dtype=np.uint8)

    # 计算噪声点的数量
    total_pixels = w * h
    noise_pixels = int(total_pixels * noise_ratio)

    # 随机选择噪声点的位置
    noise_indices = np.random.choice(total_pixels, noise_pixels, replace=False)

    # 将一部分噪声点置为随机 RGB 颜色
    for idx in noise_indices:
        y, x = divmod(idx, w)  # 将一维索引转换为二维坐标
        image[y, x] = [255, 255, 255]

    # 将 NumPy 数组转换为 PIL 图像对象
    image = Image.fromarray(image)

    return image