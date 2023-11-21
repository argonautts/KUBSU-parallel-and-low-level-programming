#include <iostream>
#include <vector>
#include <ctime>
#include <thread>
#include <chrono>
#include <algorithm>
using namespace std;
using namespace std::chrono;

int sam(vector<vector<int>> graph, int j) {
    int sum = 0;
    for (int i = 0; i < graph.size(); i++)
        sum += graph[j][i];
    return sum;
}
bool isomorphism(vector<vector<int>> graphf, vector<vector<int>> graphs, int start, int end){
    int k = 0;
    for (int i = start; i < end; i++) {
        vector<int> vfirst;
        for (int j = 0; j < graphf.size(); j++) {
            if (graphf[i][j] == 1)
                vfirst.push_back(sam(graphf, j));
        }
        sort(vfirst.begin(), vfirst.end());
        if (k < i + 1) {
            for (int c = 0; c < graphs.size(); c++) {
                vector<int> vsecond;
                for (int d = 0; d < graphs.size(); d++) {
                    if (graphf[i][d] == 1)
                        vsecond.push_back(sam(graphs, d));
                }
                sort(vsecond.begin(), vsecond.end());
                if (vfirst == vsecond)
                    k++;
            }
        }
    }
    if (graphf.size() == k)
        return true;
    else
        return false;
}

void main() {
    srand(time(0));
    int n;cin >> n; cout << endl;
    vector<vector<int>> graphf(n, vector<int>(n));
    vector<vector<int>> graphs(n, vector<int>(n));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                graphf[i][j] = 0;
                graphs[i][j] = 0;

            }
            else {
                graphf[i][j] = 1; graphf[j][i] = 1;
                graphs[i][j] = 1; graphs[j][i] = 1;
            }
        }
    }

    int r1 = rand() % n; int r2 = rand() % n;
    for (int i = 0; i < n; i++)
    {
        graphf[r1][i] = 0; graphf[i][r1] = 0;
        graphs[r2][i] = 0; graphs[i][r2] = 0;
    }

    int r3 = rand() % n; int r4 = rand() % n;
    graphf[r1][r3] = 1; graphf[r3][r1] = 1;
    graphs[r2][r4] = 1; graphs[r4][r2] = 1;

    auto t0 = high_resolution_clock::now();
    bool ans = isomorphism(graphf, graphs, 0, n);//true,false isomorph
    auto t1 = high_resolution_clock::now();

    cout << "isomorphism is: " << ans << endl;
    cout << "duration_cast vertexes: " <<    duration_cast<milliseconds>(t1 - t0).count() << " msec" << endl;

    thread first(isomorphism, graphf, graphs, 0, n / 2);
    thread second(isomorphism, graphf, graphs, n / 2, n);
    t0 = high_resolution_clock::now();
    first.join(); second.join();
    t1 = high_resolution_clock::now();
    cout << "duration_cast with threads: " << duration_cast<milliseconds>(t1 - t0).count() << " msec" << endl;
} 
