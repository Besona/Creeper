import subprocess
import os
import matplotlib.pyplot as plt
from datetime import datetime
from scipy.stats import multivariate_normal
from image import generate_image, generate_noise_image
import numpy as np

# 基础测试变量
sigma = 1
scale = 0.2
width, height = 100, 100
num_points = 50
low_bound, up_bound = 300, 500

current_timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
input_path = f'./input/{current_timestamp}/'
output_path = f'./output/{current_timestamp}/'
log_path = f'./log/{current_timestamp}/'

os.makedirs(input_path, exist_ok=True)
os.makedirs(output_path, exist_ok=True)
os.makedirs(log_path, exist_ok=True)

cov = [[sigma, 0], [0, sigma]]
rv = multivariate_normal([0, 0], cov)

algo_type = 'detect'
visualization = True
rounds = 10

def generate_test_images(rounds, cover_ratio):
    images = []
    for _ in range(rounds):
        img = generate_image(width, height, num_points, low_bound, up_bound, scale, sigma, rv)
        cover_img = generate_noise_image(width, height, cover_ratio)
        images.append((img, cover_img))
    return images

def run_algorithm_with_images(detection, correction, images):
    cover_scores = []
    for img, cover_img in images:
        img.save(f"{input_path}graph.png")
        cover_img.save(f"{input_path}cover_graph.png")

        command = [
            './executable_files/Creeper',
            '-i', input_path,
            '-o', output_path,
            '-l', log_path,
            '-d', str(detection),
            '-c', str(correction),
            '-r', '1',
            '-t', algo_type,
            '--visualization'
        ]

        result = subprocess.run(command, text=True, capture_output=True)
        if result.stderr:
            print(f"Error running {algo_type}:", result.stderr)
            cover_scores.append(0)
        else:
            print(f"Output for detection={detection}, correction={correction}:", result.stdout)
            try:
                lines = result.stdout.split('\n')
                score_found = False
                for line in lines:
                    if line.strip().isdigit():
                        cover_scores.append(float(line.strip()))
                        score_found = True
                        break
                if not score_found:
                    cover_scores.append(0)
            except Exception as e:
                print(f"Error extracting score: {e}")
                cover_scores.append(0)

    return np.mean(cover_scores)

def main():
    detection_range = range(0, 81, 5)  # [0, 5]

    # 首先计算所有可能的 correction_range 长度，找到最大列数
    all_lengths = []
    for detection in detection_range:
        c_range = range(0, detection+1, 5)
        all_lengths.append(len(list(c_range)))
    max_cols = max(all_lengths)  # 最多多少个 correction 值

    scores_matrix = []

    # 预先生成图片
    images = generate_test_images(rounds, cover_ratio=0)

    for detection in detection_range:
        print(f"Testing detection range: {detection}")
        row_scores = []
        correction_range = range(0, detection + 1, 5)

        for correction in correction_range:
            avg_score = run_algorithm_with_images(detection, correction, images)
            row_scores.append(avg_score)
            print(f"Detection {detection}, Correction {correction}, Avg Score: {avg_score}")

        # 补齐至 max_cols 列
        while len(row_scores) < max_cols:
            row_scores.append(np.nan)
        scores_matrix.append(row_scores)

    scores_matrix = np.array(scores_matrix)

    Y_values = [i*5 for i in range(max_cols)]
    X, Y = np.meshgrid(list(detection_range), Y_values)
    Z = scores_matrix.T

    fig = plt.figure(figsize=(12, 8))
    ax = fig.add_subplot(111, projection='3d')
    surf = ax.plot_surface(X, Y, Z, cmap='viridis', edgecolor='k')

    ax.set_title('Score Surface: Detection vs Correction', fontsize=16)
    ax.set_xlabel('Detection Range', fontsize=14)
    ax.set_ylabel('Correction Steps', fontsize=14)
    ax.set_zlabel('Average Score', fontsize=14)
    fig.colorbar(surf, ax=ax, shrink=0.5, aspect=10)

    plt.savefig(f'{output_path}score_surface.png')
    plt.show()

if __name__ == '__main__':
    main()