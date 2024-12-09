//
// Created by Ein on 2024/12/6.
//

#include "diamond.h"
#include <iostream>

void _dp_value(int sx, int sy, int n,
               const std::vector<std::vector<int>>& graph,
               std::vector<std::vector<int>>& f,
               std::vector<std::vector<int>>& prev)
{
    for (int i = 1; i <= n; i ++ ) {
        for (int j = 1; j <= n - i + 1; j++) {
            int value;
            if (i + sx - 1 >= graph.size() || j + sy - 1 >= graph.size()) value = 0;
            else value = graph[i + sx - 1][j + sy - 1];
            f[i][j] = std::max(f[i - 1][j], f[i][j - 1]) + value;
            prev[i][j] = f[i - 1][j] > f[i][j - 1] ? 0 : 1;
        }
    }
}


std::pair<std::vector<bool>, int> _dp_route(int sx, int sy, int n,
                                            const std::vector<std::vector<int>>& graph)
{
    std::vector<std::vector<int>> f(n + 1, std::vector<int>(n + 1));
    std::vector<std::vector<int>> prev(n + 1, std::vector<int>(n + 1));
    std::vector<bool> route(n - 1);

    _dp_value(sx, sy, n, graph, f, prev);

    int res = 0, posX;
    for (int i = 1; i <= n; i++) {
        if (res <= f[i][n - i + 1]) {
            res = std::max(res, f[i][n - i + 1]);
            posX = i;
        }
    }

    int num_steps = n - 1;
    int cur_x = posX, curY = n - cur_x + 1;
    while (num_steps -- ) {
        route[num_steps] = prev[cur_x][curY];
        if (prev[cur_x][curY]) curY -- ;
        else cur_x -- ;
    }

    return { route, res };
}

std::tuple<int, int, int> _dp_dest(int sx, int sy, int n,
                                   const std::vector<std::vector<int>>& graph)
{
    std::vector<std::vector<int>> f(n + 1, std::vector<int>(n + 1));
    std::vector<std::vector<int>> prev(n + 1, std::vector<int>(n + 1));

    _dp_value(sx, sy, n, graph, f, prev);

    int res = 0, posX;
    for (int i = 1; i <= n; i++) {
        if (res <= f[i][n - i + 1]) {
            res = std::max(res, f[i][n - i + 1]);
            posX = i;
        }
    }

    return { sx + posX - 1, sy + n - posX, res };
}

std::tuple<std::vector<bool>, int> _creeper_oh_man_correction_v(int m, int k, int p,
                                                                const std::vector<std::vector<int>>& graph)
{
    p = std::max(1, std::min(p, k));
    std::vector<bool> route;

    int total_value = graph[0][0];
    int cur_x = 0, cur_y = 0;
    while (cur_x + cur_y < m - 1) {
        auto [tmp_route, value] = _dp_route(cur_x, cur_y, k, graph);
        for (int i = 0; i < p && cur_x + cur_y < m - 1; i ++ ) {
            if (tmp_route[i]) cur_y ++ ;
            else cur_x ++ ;
            route.push_back(tmp_route[i]);
            total_value += graph[cur_x][cur_y];
        }
    }

    return { route, total_value };
}

std::tuple<std::vector<bool>, int> _creeper_oh_man_no_correction_v(int m, int k,
                                                                   const std::vector<std::vector<int>>& graph)
{
    return _creeper_oh_man_correction_v(m, k, k, graph);
}

std::tuple<std::vector<bool>, int> _creeper_cover_v(int m,
                   const std::vector<std::vector<int>>& graph,
                   const std::vector<std::vector<int>>& cover)
{
    std::vector<std::vector<int>> tmp_graph = graph;

    for (int i = 0; i < m; i ++ ) {
        for (int j = 0; j < m; j ++ ) {
            if (cover[i][j] == 0) tmp_graph[i][j] = 0;
        }
    }

    auto [route, res] = _dp_route(0, 0, m, tmp_graph);

    int cur_x = 0, cur_y = 0;
    int total_value = graph[cur_x][cur_y];
    for (auto dir : route) {
        if (dir) cur_y ++ ;
        else cur_x ++ ;
        total_value += graph[cur_x][cur_y];
    }

    return { route, total_value };
}

int _creeper_oh_man_no_correction(int m, int k,
                                  const std::vector<std::vector<int>>& graph)
{
    int total_value = graph[0][0];
    int curX = 0, curY = 0;
    while (curX + curY < m - 1) {
        auto [ex, ey, value] = _dp_dest(curX, curY, k, graph);
        total_value += value - graph[curX][curY];
        curX = ex, curY = ey;
    }

    return total_value;
}


int _creeper_oh_man_correction(int m, int k, int p,
                               const std::vector<std::vector<int>>& graph)
{
    p = std::max(1, std::min(p, k));
    int total_value = graph[0][0];
    int cur_x = 0, cur_y = 0;
    while (cur_x + cur_y < m - 1) {
        auto [tmp_route, value] = _dp_route(cur_x, cur_y, k, graph);
        for (int i = 0; i < p && cur_x + cur_y < m - 1; i ++ ) {
            if (tmp_route[i]) cur_y ++ ;
            else cur_x ++ ;
            total_value += graph[cur_x][cur_y];
        }
    }

    return total_value;
}

int _creeper_cover(int m,
              const std::vector<std::vector<int>>& graph,
              const std::vector<std::vector<int>>& cover)
{
    std::vector<std::vector<int>> tmp_graph = graph;

    for (int i = 0; i < m; i ++ ) {
        for (int j = 0; j < m; j ++ ) {
            if (cover[i][j] == 0) tmp_graph[i][j] = 0;
        }
    }

    auto [route, res] = _dp_route(0, 0, m, tmp_graph);

    int cur_x = 0, cur_y = 0;
    int total_value = graph[cur_x][cur_y];
    for (auto dir : route) {
        if (dir) cur_y ++ ;
        else cur_x ++ ;
        total_value += graph[cur_x][cur_y];
    }

    return total_value;
}

std::tuple<std::vector<bool>, int> creeper_dp_v(int n, int k, int p,
                                                const std::vector<std::vector<int>>& graph,
                                                const std::vector<std::vector<int>>& cover)
{
    return _dp_route(0, 0, n, graph);
}

std::tuple<std::vector<bool>, int> creeper_no_correction_v(int m, int k, int p,
                                                           const std::vector<std::vector<int>>& graph,
                                                           const std::vector<std::vector<int>>& cover)
{
    return _creeper_oh_man_no_correction_v(m, k, graph);
}

std::tuple<std::vector<bool>, int> creeper_correction_v(int m, int k, int p,
                                                        const std::vector<std::vector<int>>& graph,
                                                        const std::vector<std::vector<int>>& cover)
{
    return _creeper_oh_man_correction_v(m, k, p, graph);
}

std::tuple<std::vector<bool>, int> creeper_greedy_v(int m, int k, int p,
                                                    const std::vector<std::vector<int>>& graph,
                                                    const std::vector<std::vector<int>>& cover)
{
    return _creeper_oh_man_no_correction_v(m, 2, graph);
}

std::tuple<std::vector<bool>, int> creeper_cover_v(int m, int k, int p,
                                                   const std::vector<std::vector<int>>& graph,
                                                   const std::vector<std::vector<int>>& cover)
{
    return _creeper_cover_v(m, graph, cover);
}

int creeper_no_correction(int m, int k, int p,
                          const std::vector<std::vector<int>>& graph,
                          const std::vector<std::vector<int>>& cover)
{
    return _creeper_oh_man_no_correction(m, k, graph);
}

int creeper_correction(int m, int k, int p,
                       const std::vector<std::vector<int>>& graph,
                       const std::vector<std::vector<int>>& cover)
{
    return _creeper_oh_man_correction(m, k, p, graph);
}

int creeper_greedy(int m, int k, int p,
                   const std::vector<std::vector<int>>& graph,
                   const std::vector<std::vector<int>>& cover)
{
    int curX = 0, curY = 0;
    int total_value = graph[curX][curY];
    while (curX + curY < m - 1) {
        if (graph[curX + 1][curY] < graph[curX][curY + 1]) curY ++ ;
        else curX ++ ;
        total_value += graph[curX][curY];
    }

    return total_value;
}

int creeper_dp(int n, int k, int p,
               const std::vector<std::vector<int>>& graph,
               const std::vector<std::vector<int>>& cover)
{
    return std::get<2>(_dp_dest(0, 0, n, graph));
}

int creeper_cover(int m, int k, int p,
                  const std::vector<std::vector<int>>& graph,
                  const std::vector<std::vector<int>>& cover)
{
    return _creeper_cover(m, graph, cover);
}