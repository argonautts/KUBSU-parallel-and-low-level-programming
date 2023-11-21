#include <iostream>
#include <ctime>
#include <thread>
#include <chrono>
using namespace std;
using namespace std::chrono;

void firstplace(int** FirstArray, int** SecondArray, int** ThirdArray, int n) {
    for (int i = 0; i < n / 2; i++){
        for (int j = 0; j < n / 2; j++){
            for (int k = 0; k < n / 2; k++) {
                ThirdArray[i][j] += FirstArray[i][k] * SecondArray[k][j];
            }
        }
    }
}


void secondplace(int** FirstArray, int** SecondArray, int** ThirdArray, int n) {
    for (int i = n / 2; i < n; i++){
        for (int j = n / 2; j < n; j++){
            for (int k = n / 2; k < n; k++) {
                ThirdArray[i][j] += FirstArray[i][k] * SecondArray[k][j];
            }
        }
    }
}

void place(int** FirstArray, int** SecondArray, int** FourArray, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                FourArray[i][j] += FirstArray[i][k] * SecondArray[k][j];
            }
        }
    }
}

void main() {
    setlocale(LC_ALL, "Ru");
    srand(time(0));
    auto t0 = high_resolution_clock::now();
    cout << "Введите размер:"; int n; cin >> n;
    int** FirstArray; FirstArray = new int* [n];
    int** SecondArray; SecondArray = new int* [n];
    int** ThirdArray; ThirdArray = new int* [n];
    int** FourArray; FourArray = new int* [n];

    for (int i = 0; i < n; i++) {
        FirstArray[i] = new int[n];
        SecondArray[i] = new int[n];
        ThirdArray[i] = new int[n];
        FourArray[i] = new int[n];
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            FirstArray[i][j] = rand() % 15 + 3;
            SecondArray[i][j] = rand() % 15 + 3;
            ThirdArray[i][j] = 0;
            FourArray[i][j] = 0;
        }
    }

    thread first(firstplace, FirstArray, SecondArray, ThirdArray, n);
    first.join();

    thread second(secondplace, FirstArray, SecondArray, ThirdArray, n);
    second.join();

    auto t1 = high_resolution_clock::now();

    thread third(place, FirstArray, SecondArray, FourArray, n);
    third.join();

    auto t2 = high_resolution_clock::now();


    cout << "Время вычисления параллельного: " << duration_cast<milliseconds>(t1 - t0).count() << "msec\n";
    cout << "Время вычисления прямого: " << duration_cast<milliseconds>(t2 - t0).count() << "msec\n";
    for (int i = 0; i < n; i++) {
        delete[] FirstArray[i];
        delete[] SecondArray[i];
        delete[] ThirdArray[i];
        delete[] FourArray[i];
    }
    delete[] FirstArray, SecondArray, ThirdArray, FourArray;
}
