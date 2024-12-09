import subprocess
import os
import matplotlib.pyplot as plt
from datetime import datetime
from scipy.stats import multivariate_normal
from image import generate_image, generate_noise_image
import numpy as np

# 基础测试变量
sigma = 1  # 高斯分布范围
scale = 0.2  # 缩放比例
width, height = 100, 100  # 图片大小
num_points = 50  # 中心点数量
low_bound, up_bound = 300, 500  # 权重下界和上界

# 动态路径，基于当前时间生成
current_timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
input_path = f'./input/{current_timestamp}/'
output_path = f'./output/{current_timestamp}/'
log_path = f'./log/{current_timestamp}/'

# 创建目录
os.makedirs(input_path, exist_ok=True)
os.makedirs(output_path, exist_ok=True)
os.makedirs(log_path, exist_ok=True)

# 高斯分布初始化
cov = [[sigma, 0], [0, sigma]]
rv = multivariate_normal([0, 0], cov)

# 测试变量
algo_type = 'cover'  # 蒙图算法
detection = 15  # 探测距离
correction = 5  # 修正步数
visualization = True  # 启用可视化
rounds = 10  # 每个蒙图率测试的次数


def generate_test_image(cover_ratio):
    """生成测试图片和对应蒙图"""
    # 生成无蒙版的图像
    img = generate_image(width, height, num_points, low_bound, up_bound, scale, sigma, rv)
    img.save(f"{input_path}graph.png")

    # 生成蒙版图像
    cover_img = generate_noise_image(width, height, cover_ratio)
    cover_img.save(f"{input_path}cover_graph.png")


def run_algorithm():
    """运行蒙图算法并提取得分"""
    print(f"Running {algo_type} algorithm for cover ratio...")

    command = [
        './executable_files/Creeper',
        '-i', input_path,
        '-o', output_path,
        '-l', log_path,
        '-d', str(detection),
        '-c', str(correction),
        '-r', '1',  # 单次测试
        '-t', algo_type,
        '--visualization'
    ]

    result = subprocess.run(command, text=True, capture_output=True)
    if result.stderr:
        print(f"Error running {algo_type}:", result.stderr)
        return None
    else:
        print(f"Output for {algo_type}:", result.stdout)

        # 提取输出中的得分
        try:
            lines = result.stdout.split('\n')  # 按行分割输出
            for line in lines:
                # 确保是纯数字行（过滤空行和非得分行）
                if line.strip().isdigit():
                    return float(line.strip())
        except Exception as e:
            print(f"Error extracting score: {e}")
            return None

    return None

def main():
    """主程序：测试不同蒙图率并绘制曲线"""
    scores = []  # 存储每个蒙图率的平均得分
    cover_ratios = [i / 100 for i in range(1, 101)]  # 1% 到 100%的蒙图率

    # 遍历蒙图率
    for cover_ratio in cover_ratios:
        print(f"Testing cover ratio: {cover_ratio * 100:.1f}%")
        cover_scores = []  # 存储每次测试的得分

        for _ in range(rounds):
            generate_test_image(cover_ratio)  # 生成测试数据
            score = run_algorithm()  # 运行算法并获取得分

            if score is not None:
                cover_scores.append(score)
            else:
                cover_scores.append(0)  # 如果得分获取失败，设置为 0

        # 计算平均分
        avg_score = np.mean(cover_scores)
        print(f"Cover Ratio: {cover_ratio * 100:.1f}%, Average Score: {avg_score}")
        scores.append(avg_score)

    # 绘制曲线
    plt.figure(figsize=(12, 6))
    plt.plot(cover_ratios, scores, label='Average Score vs Cover Ratio', color='blue')
    plt.title('Effect of Cover Ratio on Final Score', fontsize=16)
    plt.xlabel('Cover Ratio (%)', fontsize=14)
    plt.ylabel('Average Final Score', fontsize=14)
    plt.grid(True)
    plt.legend()
    plt.savefig(f'{output_path}cover_ratio_vs_score.png')
    plt.show()


if __name__ == '__main__':
    main()