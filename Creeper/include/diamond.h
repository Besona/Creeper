//
// Created by Ein on 2024/12/6.
//

#ifndef CREEPER_DIAMOND_H
#define CREEPER_DIAMOND_H

#include <vector>

std::tuple<std::vector<bool>, int> creeper_dp_v(int n, int k, int p,
                                                const std::vector<std::vector<int>>& graph,
                                                const std::vector<std::vector<int>>& cover);

std::tuple<std::vector<bool>, int> creeper_no_correction_v(int m, int k, int p,
                                                           const std::vector<std::vector<int>>& graph,
                                                           const std::vector<std::vector<int>>& cover);

std::tuple<std::vector<bool>, int> creeper_correction_v(int m, int k, int p,
                                                        const std::vector<std::vector<int>>& graph,
                                                        const std::vector<std::vector<int>>& cover);

std::tuple<std::vector<bool>, int> creeper_greedy_v(int m, int k, int p,
                                                    const std::vector<std::vector<int>>& graph,
                                                    const std::vector<std::vector<int>>& cover);

std::tuple<std::vector<bool>, int> creeper_cover_v(int m, int k, int p,
                                                   const std::vector<std::vector<int>>& graph,
                                                   const std::vector<std::vector<int>>& cover);


int creeper_no_correction(int m, int k, int p,
                          const std::vector<std::vector<int>>& graph,
                          const std::vector<std::vector<int>>& cover);

int creeper_correction(int m, int k, int p,
                       const std::vector<std::vector<int>>& graph,
                       const std::vector<std::vector<int>>& cover);

int creeper_greedy(int m, int k, int p,
                   const std::vector<std::vector<int>>& graph,
                   const std::vector<std::vector<int>>& cover);

int creeper_dp(int n, int k, int p,
               const std::vector<std::vector<int>>& graph,
               const std::vector<std::vector<int>>& cover);

int creeper_cover(int m, int k, int p,
                  const std::vector<std::vector<int>>& graph,
                  const std::vector<std::vector<int>>& cover);

#endif //CREEPER_DIAMOND_H
