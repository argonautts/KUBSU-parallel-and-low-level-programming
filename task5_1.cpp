#include "profile.h"
#include <iostream>
#include <vector>
#include <random>
#include <atomic>
#include <thread>
using namespace std;
#define INF INT16_MAX

vector<vector<int32_t>> gen_rand_graph(size_t vertex_num, size_t edges_num) {
    vector<vector<int32_t>> adj_matrix(vertex_num, vector<int32_t>(vertex_num, INF));
    if (edges_num <= 0)
        return adj_matrix;
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> dist(0, 10);
    for (int i = 0; i < vertex_num; ++i) {
        adj_matrix[i][i] = 0;
    }
    for (int i = 0; i < vertex_num; ++i) {
        for (int j = i; j < vertex_num; ++j) {
            if (i == j)
                continue;
            else if (!(dist(rng) % 4)) {
                adj_matrix[i][j] = 1;
                adj_matrix[j][i] = 1;
                edges_num--;
            }
            else {
                adj_matrix[i][j] = INF;
            }
            if (edges_num <= 0)
                break;
        }
        if (edges_num <= 0)
            break;
    }
    return adj_matrix;
}

int64_t find_diameter(vector<vector<int32_t>> d) {
    int64_t diameter = -1;
    for (size_t k = 0; k < d.size(); ++k)
        for (size_t i = 0; i < d.size(); ++i)
            for (size_t j = 0; j < d.size(); ++j) {
                d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
                if (d[i][j] != INF && d[i][j] > diameter)
                    diameter = d[i][j];
            }
    return diameter;
}

void process_data_part(vector<vector<int32_t>> d, atomic<int64_t>& diameter, size_t range_begin, size_t ragne_end) {
    for (size_t k = range_begin; k < ragne_end; ++k)
        for (size_t i = 0; i < d.size(); ++i)
            for (size_t j = 0; j < d.size(); ++j) {
                d[i][j] = min(d[i][j], d[i][k] + d[k][j]);
                if (d[i][j] != INF && d[i][j] > diameter)
                    diameter = d[i][j];
            }
}

int64_t find_diameter_multithreading(const vector<vector<int32_t>>& adj_matrix) {
    atomic<int64_t> diameter = -1;
    vector<thread> threads;
    int part_size = adj_matrix.size() / thread::hardware_concurrency();
    vector<int> parallel_data_parts_sizes(thread::hardware_concurrency(), part_size);
    for (int i = adj_matrix.size() - part_size * thread::hardware_concurrency(); i > 0; --i) {
        parallel_data_parts_sizes[i]++;
    }
    int t = 0;
    for (int i = 0; i < thread::hardware_concurrency(); ++i) {
        threads.emplace_back(process_data_part, ref(adj_matrix), ref(diameter), t, t + parallel_data_parts_sizes[i]);
        t += parallel_data_parts_sizes[i];
    }
    for (thread& thread : threads) {
        thread.join();
    }
    return diameter;
}

int main() {
    size_t n;
    int64_t singleThreadAns, multithreadingAns;
    for (n = 8; n <= 1024; n *= 2) {
        cerr << "Graph with " << n << " verteces and " << n / 4 << " edges" << endl;
        auto g = gen_rand_graph(n, n / 4);
        {
            LOG_DURATION("Single thread");
            singleThreadAns = find_diameter(g);
        }
        {
            LOG_DURATION("Multithreading");
            multithreadingAns = find_diameter_multithreading(g);
        }
        try {
            if (singleThreadAns != multithreadingAns) {
                throw runtime_error("DIFFERENT ANSWERS");
            }
        }
        catch (runtime_error& e) {
            cerr << e.what() << endl;
        }
        cout << endl;
    }
    return 0;
}
