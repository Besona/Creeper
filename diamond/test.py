import subprocess

from scipy.stats import multivariate_normal
from image import generate_image, generate_noise_image

# 测试样例生成 相关变量
sigma = 1  # 控制中心点的控制范围
scale = 0.2  # 缩放参数，因为 gaussian 分布如果只采样整数点会丢失很多细节
width = 100  # 图片宽度
height = 100  # 图片高度
num_points = 50  # 中心点数量
low_bound = 100  # 中心点权重下界
up_bound = 500  # 中心点权重上界

num_graphs = 10  # 一次生成多少张图片
cover = True  # 是否生成蒙版图
cover_ratio = 0.1  # 蒙版率

# 不需要配置
cov = [[sigma, 0], [0, sigma]]
rv = multivariate_normal([0, 0], cov)

# IO 相关变量
input_path = './input/'
output_path = './output/'
log_path = './log/'

# 算法 相关变量
detection = 15  # 探测距离
correction = 5  # 走几步之后修正
rounds = 10  # 每个测试样例跑几遍
visualization = True  # 是否绘制路线图


def run_algorithm(algo_type, v=False, precise='nano'):
    print("Running Algorithm ... ")
    command = [
        './executable_files/Creeper',
        '-i', input_path,
        '-o', output_path,
        '-l', log_path,
        '-d', str(detection),
        '-c', str(correction),
        '-r', str(rounds),
        '-t', algo_type,
        '-p', precise
    ]

    if v:
        command.append('--visualization')

    result = subprocess.run(command, text=True, capture_output=True)
    if result.stderr:
        print("Error running DivideAndConquer:", result.stderr)
    else:
        print("output:", result.stdout)


def generate_test_examples():
    for i in range(0, num_graphs):
        print(f"Generating {i} / {num_graphs} graph")
        img = generate_image(width, height, num_points, low_bound, up_bound, scale, sigma, rv)
        img.save(f"{input_path}g_{width}_{height}_{num_points}_{low_bound}_{up_bound}_{i}.png")
        if cover:
            img = generate_noise_image(width, height, cover_ratio)
            img.save(f"{input_path}cover_g_{width}_{height}_{num_points}_{low_bound}_{up_bound}_{i}.png")


def run_all_algorithm(precise='nano'):
    algo_types = ['detect',     # 探测前方 k 格，走 p 格后修正道路
                  'dp',         # 常规 dp
                  'greedy',     # 常规贪心
                  'detect_p',   # 探测前方 k 格，不修正路径
                  'cover'       # 蒙图版
                  ]

    for algo_type in algo_types:
        run_algorithm(algo_type, False, precise)
        run_algorithm(algo_type, True, precise)


if __name__ == '__main__':
    generate_test_examples()
    run_all_algorithm()
