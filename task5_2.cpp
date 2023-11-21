#include "profile.h"
#include <omp.h>
#include <ppl.h>
#include <iostream>
#include <vector>
#include <random>
#include <atomic>
#include <queue>
#include <algorithm>
using namespace std;
#define INF INT16_MAX

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

const size_t m = 1000;
const size_t depth = 100;

int8_t min_num_of_fields = 10;
int8_t max_num_of_fields = 50;
int8_t min_size_of_field = 10;
int8_t max_size_of_field = 50;

enum resourse_type {
    NOTHING,
    GAS,
    OIL,
    COAL,
    METAL,
};

#define rt resourse_type

vector<vector<vector<rt>>> ground(m, vector<vector<rt>>(m, vector<rt>(depth)));

void gen_fields() {
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> position(0, m - 1);
    uniform_int_distribution<mt19937::result_type> depth_(0, depth - 1);
    uniform_int_distribution<mt19937::result_type> num_of_fields_(min_num_of_fields, max_num_of_fields);
    uniform_int_distribution<mt19937::result_type> size_of_field_(min_size_of_field, max_size_of_field);
    uniform_int_distribution<mt19937::result_type> res_type_(0, 4);

    cerr << "I'm working" << endl << endl;

    int32_t num_of_fields = num_of_fields_(rng);
    int32_t pos_x, pos_y, pos_z, sz1, sz2, sz3, res_type;
    while (num_of_fields) {
        pos_x = position(rng);
        pos_y = position(rng);
        pos_z = depth_(rng);
        sz1 = size_of_field_(rng);
        sz2 = size_of_field_(rng);
        sz3 = size_of_field_(rng);
        res_type = res_type_(rng);

        for (int32_t i = pos_x; i < min(pos_x + sz1, (int)m); i++)
            for (int32_t j = pos_y; j < min(pos_y + sz2, (int)m); j++)
                for (int32_t k = pos_z; k < min(pos_z + sz3, (int)depth); k++)
                    ground[i][j][k] = (rt)res_type;

        num_of_fields--;
    }
}


int BFS(int stx, int sty, int stz, vector<vector<vector<bool>>>& visited) {
    int field_volume = 0;
    visited[stx][sty][stz] = true;
    rt this_field_type = ground[stx][sty][stz];
    queue<int>q;
    q.push(stx);
    q.push(sty);
    q.push(stz);
    while (!q.empty()) {
        int f1, f2, f3;
        f1 = q.front();
        q.pop();
        f2 = q.front();
        q.pop();
        f3 = q.front();
        q.pop();
        for (int8_t i = -1; i < 2; i++) {
            for (int8_t j = -1; j < 2; j++) {
                for (int8_t k = -1; k < 2; k++) {
                    int v1, v2, v3;
                    v1 = f1 + i;
                    v2 = f2 + j;
                    v3 = f3 + k;
                    if ((v1 >= 0 && v1 < m && v2 >= 0 && v2 < m && v3 >= 0 && v3 < depth)
                        && !visited[v1][v2][v3] && ground[v1][v2][v3] == this_field_type)
                    {
                        field_volume++;
                        q.push(v1);
                        q.push(v2);
                        q.push(v3);
                        visited[v1][v2][v3] = true;
                    }
                }
            }
        }
    }
    return field_volume;
}

//Найдем количество газовых месторождений и объем самого большого из них
pair<int, int> find_filds() {
    vector<vector<vector<bool>>> visited(m, vector<vector<bool>>(m, vector<bool>(depth, false)));
    int32_t max_volume = 0, fields_num = 0;
    for (size_t i = 0; i < m; i++)
        for (size_t j = 0; j < m; j++)
            for (size_t k = 0; k < depth; k++)
                if (ground[i][j][k] == GAS && !visited[i][j][k]) {
                    max_volume = max(BFS(i, j, k, visited), max_volume);
                    fields_num++;
                }
    return { fields_num, max_volume };
}

pair<int, int> find_filds_omp() {
    vector<vector<vector<bool>>> visited(m, vector<vector<bool>>(m, vector<bool>(depth, false)));
    int32_t max_volume = 0, fields_num = 0;

#pragma omp parallel for shared(visited)
    for (int64_t i = 0; i < m; i++)
        for (size_t j = 0; j < m; j++)
            for (size_t k = 0; k < depth; k++)
                if (ground[i][j][k] == GAS && !visited[i][j][k]) {
                    max_volume = max(BFS(i, j, k, visited), max_volume);
                    fields_num++;
                }
    return { fields_num, max_volume };
}

pair<int, int> find_filds_ppl() {
    vector<vector<vector<bool>>> visited(m, vector<vector<bool>>(m, vector<bool>(depth, false)));
    int32_t max_volume = 0, fields_num = 0;
    concurrency::parallel_for((size_t)0, m, [&](size_t i) {
        for (size_t j = 0; j < m; j++)
            for (size_t k = 0; k < depth; k++)
                if (ground[i][j][k] == GAS && !visited[i][j][k]) {
                    max_volume = max(BFS(i, j, k, visited), max_volume);
                    fields_num++;
                }
        });
    return { fields_num, max_volume };
}

int main() {
    gen_fields();
    pair<int, int> answer, answerOMP, answerPPL;
    {
        LOG_DURATION("Single core");
        answer = find_filds();
    }
    {
        LOG_DURATION("OMP");
        answerOMP = find_filds_omp();
    }
    {
        LOG_DURATION("PPL");
        answerPPL = find_filds_ppl();
    }
    return 0;
}
