#define _SILENCE_AMP_DEPRECATION_WARNINGS

#include "profile.h"
#include <omp.h>
#include <ppl.h>
#include <amp.h>

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#define INF INT16_MAX

#ifdef max
#undef max
#undef min
#endif

vector<vector<bool>> gen_incidence_matrix(int32_t vertex_num, int32_t edges_num) {
    vector<vector<bool>> incidence_matrix(edges_num, vector<bool>(vertex_num, false));

    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> edges_idx(0, edges_num - 1);
    uniform_int_distribution<mt19937::result_type> vertex_idx(0, vertex_num - 1);

    int32_t ei, vi;
    for (size_t i = 0; i < edges_num; i++) {
        vi = vertex_idx(rng);

        incidence_matrix[i][vi] = true;

        do {
            vi = vertex_idx(rng);
        } while (incidence_matrix[i][vi]);

        incidence_matrix[i][vi] = true;
    }

    return incidence_matrix;
}

vector<vector<int32_t>> find_matrix_of_min_routes(const vector<vector<bool>>& inc_matrix, int32_t vertex_num, int32_t edges_num) {
    vector<vector<int32_t>> adj_matrix(vertex_num, vector<int32_t>(vertex_num, 0));

    int32_t idx1, idx2;
    for (size_t i = 0; i < edges_num; i++) {
        idx1 = 0;
        idx2 = 0;
        for (size_t j = 0; j < vertex_num; j++) {
            if (inc_matrix[i][j] && !idx1) {
                idx1 = j;
            }
            else if (inc_matrix[i][j] && idx1) {
                idx2 = j;
                break;
            }
        }
        adj_matrix[idx1][idx2] = true;
        adj_matrix[idx2][idx1] = true;
    }

    vector<vector<int32_t>> d(vertex_num, vector<int32_t>(vertex_num, 0));
    for (int k = 0; k < vertex_num; ++k)
        for (int i = 0; i < vertex_num; ++i)
            for (int j = 0; j < vertex_num; ++j)
                d[i][j] = min(d[i][j], d[i][k] + d[k][j]);

    return d;
}

vector<vector<int32_t>> find_matrix_of_min_routes_omp(const vector<vector<bool>>& inc_matrix, int32_t vertex_num, int32_t edges_num) {
    vector<vector<int32_t>> adj_matrix(vertex_num, vector<int32_t>(vertex_num, 0));

    int32_t idx1, idx2;
#pragma omp parallel for
    for (int64_t i = 0; i < edges_num; i++) {
        idx1 = 0;
        idx2 = 0;
        for (size_t j = 0; j < vertex_num; j++) {
            if (inc_matrix[i][j] && !idx1) {
                idx1 = j;
            }
            else if (inc_matrix[i][j] && idx1) {
                idx2 = j;
                break;
            }
        }
        adj_matrix[idx1][idx2] = true;
        adj_matrix[idx2][idx1] = true;
    }
    vector<vector<int32_t>> d(vertex_num, vector<int32_t>(vertex_num, 0));
#pragma omp parallel for shared(d)
    for (int k = 0; k < vertex_num; ++k)
        for (int i = 0; i < vertex_num; ++i)
            for (int j = 0; j < vertex_num; ++j)
                d[i][j] = min(d[i][j], d[i][k] + d[k][j]);

    return d;
}

vector<vector<int32_t>> find_matrix_of_min_routes_ppl(const vector<vector<bool>>& inc_matrix, int32_t vertex_num, int32_t edges_num) {
    vector<vector<int32_t>> adj_matrix(vertex_num, vector<int32_t>(vertex_num, 0));

    int32_t idx1, idx2;
    concurrency::parallel_for((size_t)0, (size_t)edges_num, [&](size_t i) {
        idx1 = 0;
        idx2 = 0;
        for (size_t j = 0; j < vertex_num; j++) {
            if (inc_matrix[i][j] && !idx1) {
                idx1 = j;
            }
            else if (inc_matrix[i][j] && idx1) {
                idx2 = j;
                break;
            }
        }
        adj_matrix[idx1][idx2] = true;
        adj_matrix[idx2][idx1] = true;
        });

    vector<vector<int32_t>> d(vertex_num, vector<int32_t>(vertex_num, 0));

    concurrency::parallel_for((size_t)0, d.size(), [&](size_t k) {
        for (int i = 0; i < vertex_num; ++i)
            for (int j = 0; j < vertex_num; ++j)
                d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
        });

    return d;
}

vector<int32_t> find_matrix_of_min_routes_amp(const vector<vector<bool>>& inc_matrix, int32_t vertex_num, int32_t edges_num) {
    vector<vector<int32_t>> adj_matrix(vertex_num, vector<int32_t>(vertex_num, 0));

    int32_t idx1, idx2;
    for (size_t i = 0; i < edges_num; i++) {
        idx1 = 0;
        idx2 = 0;
        for (size_t j = 0; j < vertex_num; j++) {
            if (inc_matrix[i][j] && !idx1) {
                idx1 = j;
            }
            else if (inc_matrix[i][j] && idx1) {
                idx2 = j;
                break;
            }
        }
        adj_matrix[idx1][idx2] = true;
        adj_matrix[idx2][idx1] = true;
    }

    vector<int32_t> d(vertex_num * vertex_num, 0);

    concurrency::array_view<int, 2> d_view(vertex_num, vertex_num, d);

    d_view.discard_data();
    concurrency::parallel_for_each(d_view.extent,
        [=](concurrency::index<2> idx) restrict(amp) {
            int row = idx[0];
            int col = idx[1];
            for (int inner = 0; inner < idx[0]; inner++) {
                if (d_view[idx] < d_view(row, inner) + d_view(inner, col)) {
                    d_view[idx] = d_view(row, inner) + d_view(inner, col);
                }
            }
        });
    d_view.synchronize();

    return d;
}

int main() {
    int32_t vertex_num, edges_num;
    cin >> vertex_num >> edges_num;

    auto im = gen_incidence_matrix(vertex_num, edges_num);
    vector<vector<int32_t>> mr;
    vector<int32_t> mr_;
    {
        LOG_DURATION("CPU single core computition");
        mr = find_matrix_of_min_routes(im, vertex_num, edges_num);
    }
    {
        LOG_DURATION("CPU OpenMP multithreading computition");
        mr = find_matrix_of_min_routes_omp(im, vertex_num, edges_num);
    }
    {
        LOG_DURATION("CPU PPL multithreading computition");
        mr = find_matrix_of_min_routes_ppl(im, vertex_num, edges_num);
    }
    {
        LOG_DURATION("GPU computition");
        auto mr_ = find_matrix_of_min_routes_amp(im, vertex_num, edges_num);
    }

    return 0;
}
