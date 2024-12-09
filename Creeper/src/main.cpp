#include <iostream>
#include <unordered_map>
#include <fstream>
#include <filesystem>

#include "argparse.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "diamond.h"


using func = int (*)(int, int, int, const std::vector<std::vector<int>>&, const std::vector<std::vector<int>>&);
using func_v = std::tuple<std::vector<bool>, int> (*)(int, int, int, const std::vector<std::vector<int>>&, const std::vector<std::vector<int>>&);


static std::unordered_map<std::string, std::variant<func, func_v>> funcMap = {
        {"dp", creeper_dp },
        {"greedy", creeper_greedy },
        {"cover", creeper_cover},
        {"detect", creeper_correction },
        {"detect_p", creeper_no_correction },
        {"dp_v", creeper_dp_v },
        {"greedy_v", creeper_greedy_v },
        {"cover_v", creeper_cover_v},
        {"detect_v", creeper_correction_v },
        {"detect_p_v", creeper_no_correction_v },

};

int64_t GetDuration(const std::chrono::steady_clock::time_point& start,
                    const std::chrono::steady_clock::time_point& end,
                    const std::string& precise)
{
    if (precise == "nano")
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    else if (precise == "milli")
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    else if (precise == "micro")
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    else exit(1);
}

int64_t foo(func f, int m, int k, int p,
            const std::vector<std::vector<int>>& graph,
            const std::vector<std::vector<int>>& cover,
            const std::string& precise)
{
    auto start = std::chrono::high_resolution_clock::now();
    f(m, k, p, graph, cover);
    auto end = std::chrono::high_resolution_clock::now();

    return GetDuration(start, end, precise);
}

std::vector<std::vector<int>> loadFromImgData(const unsigned char* img_data, int width, int height, int channels)
{
    std::vector<std::vector<int>> img(height, std::vector<int>(height));
    for (int y = 0; y < height; y ++ ) {
        for (int x = 0; x < width; x ++ ) {
            img[y][x] = 255 - img_data[(y * width + x) * channels + 1];
        }
    }

    return std::move(img);
}


void ArgparseInit(argparse::ArgumentParser& program, int argc, char* argv[])
{
    program.add_argument("-l", "--log")
            .help("The log path")
            .default_value("./log/");

    program.add_argument("-r", "--rounds")
            .help("Num of each test case runs")
            .default_value(10)
            .scan<'i', int>();

    program.add_argument("-t", "--type")
            .help("The algorithm used, dp, greedy, detect, detect_p, cover")
            .default_value("detect_p");

    program.add_argument("-p", "--precise")
            .help("The precise of recorded time: micro, milli, nano")
            .default_value("nano");

    program.add_argument("-i", "--input")
            .help("Input graph path")
            .default_value("./input/");

    program.add_argument("-o", "--output")
            .help("output graph path")
            .default_value("./output/");

    program.add_argument("-v", "--visualization")
            .help("Output path details")
            .default_value(true)
            .implicit_value(true);

    program.add_argument("-d", "--detection")
            .help("Is detect n foward path")
            .default_value(86)
            .scan<'i', int>();

    program.add_argument("-c", "--correction")
            .help("Is correct path after n movements")
            .default_value(72)
            .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    /* Init */
    argparse::ArgumentParser program("Algorithm");
    ArgparseInit(program, argc, argv);

    auto numRounds = program.get<int>("rounds");
    auto inputPath = program.get<std::string>("input");
    auto outputPath= program.get<std::string>("output");
    auto logPath = program.get<std::string>("log");
    auto funcType = program.get<std::string>("type");
    auto precise = program.get<std::string>("precise");

    auto visualization = program.get<bool>("visualization");
    auto detection = std::max(2, program.get<int>("detection"));
    auto correction = program.get<int>("correction");

    if (visualization) {
        numRounds = 1;
        logPath = "";
        funcType += "_v";
    }

    std::vector<std::string> inputGraphs;
    for (const auto& entry : std::filesystem::directory_iterator(inputPath)) {
        if (entry.is_regular_file() && entry.path().string().find("cover") == std::string::npos) {
            inputGraphs.push_back(entry.path().string());
        }
    }

    /* Algorithm */

    std::vector<uint64_t> record;

    for (auto& graph_path : inputGraphs) {

        int width_origin, height_origin, channel_origin;
        std::vector<std::vector<int>> graph;
        std::vector<std::vector<int>> cover;

        unsigned char* image_data = stbi_load(graph_path.c_str(), &width_origin, &height_origin,
                                             &channel_origin, 3);
        graph = loadFromImgData(image_data, width_origin, height_origin, channel_origin);

        if (funcType.find("cover") != std::string::npos) {
            int width_cover, height_cover, channel_cover;

            auto tmp = graph_path;
            tmp.insert(inputPath.size(), "cover_");
            unsigned char* image_data_cover = stbi_load((tmp).c_str(), &width_cover, &height_cover,
                                                    &channel_cover, 3);
            cover = loadFromImgData(image_data_cover, width_cover, height_cover, channel_cover);
        }

        if (visualization) {
            auto function = std::get<func_v>(funcMap[funcType]);
            auto [route, res] = function(width_origin, detection, correction, graph, cover);

            int cur_x = 0, cur_y = 0;
            int cur_index = (cur_y * width_origin + cur_x) * channel_origin;
            image_data[cur_index] = image_data[cur_index + 1] = image_data[cur_index + 2] = 0;

            for (auto dir : route) {
                if (dir) cur_y ++ ;
                else cur_x ++ ;

                cur_index = (cur_x * width_origin + cur_y) * channel_origin;
                image_data[cur_index] = image_data[cur_index + 1] = image_data[cur_index + 2] = 0;
            }

            graph_path.replace(0, inputPath.size(), outputPath);
            auto it = graph_path.find(".png");
            graph_path.insert(it, "_" + funcType + "_" + std::to_string(res));

            stbi_write_png((graph_path).c_str(), width_origin, height_origin, channel_origin,
                           image_data, width_origin * channel_origin);
            std::cout << res << ' ';
        } else {
            auto function = std::get<func>(funcMap[funcType]);
            for (int i = 0; i < numRounds; i ++ ) {
                auto res = foo(function, width_origin, detection, correction, graph, cover, precise);
                record.push_back(res);
            }
        }
    }


    /* Output */
    if (!visualization) {
        std::ofstream lfs(logPath + funcType + "_log" + ".md");

        lfs << "Time Precise: " << precise << "second" << '\n';
        size_t tot_max = 0, tot_avg = 0;
        for (int i = 0; i < inputGraphs.size(); i ++ ) {
            lfs << "Time: ";
            size_t max = 0, avg = 0;
            lfs << inputGraphs[i] << ": ";
            for (int j = 0; j < numRounds; j ++ ) {
                auto& t = record[i * numRounds + j];
                max = t > max ? t : max;
                avg += t / numRounds;
            }
            lfs << std::setw(5) << "AVG: " << avg << ' ';
            lfs << std::setw(5) << "MAX: " << max << '\n';
            tot_max = std::max(max, tot_max);
            tot_avg += avg / inputGraphs.size();
        }
        lfs << "TOT_AVG: " << tot_avg << ' ' << "TOT_MAX " << tot_max << '\n';
    }

    return 0;
}
