#include <iostream>
#include <ctime>
#include <omp.h>
#include <chrono>
using namespace std;
using namespace std::chrono;

double** copy(double** array, int n) {
    double** newArray = new double* [n];
    for (int i = 0; i < n; i++) {
        newArray[i] = new double[n];
        for (int j = 0; j < n; j++) {
            newArray[i][j] = array[i][j];
        }
    }
    return newArray;
}

double** matrixMultiple(double** matrixA, double** matrixB, int n) {
    double** res = new double* [n];
    for (int i = 0; i < n; i++) {
        res[i] = new double[n];
        for (int j = 0; j < n; j++) {
            res[i][j] = 0;
        }
    }
#pragma omp parallel for shared (res,matrixA,matrixB)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                res[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
    return res;
}

void scalarMultiple(double** matrix, int n, double x) {
#pragma omp parallel for shared (x,matrix)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] *= x;
}
void suma(double** matrixA, double** matrixB, int n) {
#pragma omp parallel for shared (matrixA,matrixB)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrixA[i][j] += matrixB[i][j];
}

void deleteArray(double** array, int n) {
    for (int i = 0; i < n; i++) {
        delete[] array[i];
    }
    delete[] array;
}

double determinant(double** matrix, int n) {
    double** matrixB = copy(matrix, n);
    for (int temp = 0; temp < n - 1; temp++) {
        for (int i = temp + 1; i < n; i++) {
            double coeff = -matrixB[i][temp] / matrixB[temp][temp];
            for (int j = temp; j < n; j++) {
                matrixB[i][j] += matrixB[temp][j] * coeff;
            }
        }
    }
    double Det = 1;
    for (int i = 0; i < n; i++) {
        Det *= matrixB[i][i];
    }
    deleteArray(matrixB, n);
    return Det;
}

void display(double** matrix, int n) {
    cout << "matrix:" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

void main() {
    cout << "Enter number of equations: ";
    int n; cin >> n;
    double** A = new double* [n];
    cout << "Do you want to enter values manually or randomly ?" << endl << "1 - manually, 2 - random" << endl;
    int input; cin >> input; cout << endl;
    switch (input) {
    case 1:
        for (int i = 0; i < n; i++) {
            A[i] = new double[n];
            for (int j = 0; j < n; j++) {
                cin >> A[i][j];
            }
        }
        cout << endl;
        break;
    case 2:
        srand(time(0));
        for (int i = 0; i < n; i++) {
            A[i] = new double[n];
            for (int j = 0; j < n; j++) {
                A[i][j] = rand() % 10;
            }
        }
        cout << endl;
        break;
    default:
        cout << "Error, bad input, quitting." << endl;
        break;
    }

    display(A, n);
    auto t0 = high_resolution_clock::now();
    double num0 = 0, num1 = 0;
    double** copyA = copy(A, n);
    for (size_t i = 0; i < n; i++) {
        double sumj = 0, sumi = 0;
        for (size_t j = 0; j < n; j++) {
            sumi += fabs(copyA[i][j]);
            sumj += fabs(copyA[j][i]);
        }
        num0 = max(sumj, num0);
        num1 = max(sumi, num1);
    }

    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = i + 1; j < n; j++) {
            swap(copyA[j][i], copyA[i][j]);
        }
    }

    double** Ez = new double* [n];
    scalarMultiple(copyA, n, (1 / (num0 * num1)));

    for (int i = 0; i < n; i++) {
        Ez[i] = new double[n];
        for (int j = 0; j < n; j++) {
            if (i == j)
                Ez[i][j] = 2;
            else
                Ez[i][j] = 0;
        }
    }
    double e = 0.001;
    double** inverse = copy(copyA, n);
    if (determinant(A, n) != 0) {
        while (fabs(determinant(matrixMultiple(A, inverse, n), n) - 1) >= e) {
            double** pred = copy(inverse, n);
            inverse = matrixMultiple(A, pred, n);
            scalarMultiple(inverse, n, -1);
            suma(inverse, Ez, n);
            inverse = matrixMultiple(pred, inverse, n);
            deleteArray(pred, n);
        }
        cout << "Inverse ";
        display(inverse, n);
    }
    else
        cout << "Inverse matrix does not exist. ";
    auto t1 = high_resolution_clock::now();
    deleteArray(A, n);
    deleteArray(Ez, n);
    cout << "duration_cast: " << duration_cast<milliseconds>(t1 - t0).count() << " milliseconds" << endl;
}
