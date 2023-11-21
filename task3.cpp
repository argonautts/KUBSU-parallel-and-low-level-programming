#include <iostream>
#include <ctime>
#include <thread>
#include <chrono>
using namespace std;
using namespace std::chrono;

double* gauss(double** a, double* y, int start, int end){
    double* x, max;
    int с = start, index;
    const double eps = 0.00001;// точность
    x = new double[end];
    for (с; с < end; с){
        // Поиск строки с максимальным a[i][с]
        max = abs(a[с][с]);
        index = с;
        for (int i = с + 1; i < end; i++){
            if (abs(a[i][с]) > max){
                max = abs(a[i][с]);
                index = i;
            }
        }
        // Перестановка строк
        if (max < eps){
            // нет ненулевых диагональных элементов
            cout << "Solution cannot be obtained due to null column ";
            cout << index << " matrices A" << endl;
            return 0;
        }
        for (int j = 0; j < end; j++){
            double swap = a[с][j];
            a[с][j] = a[index][j];
            a[index][j] = swap;
        }
        double swap = y[с];
        y[с] = y[index];
        y[index] = swap;
        // Нормализация уравнений
        for (int i = с; i < end; i++){
            double swap = a[i][с];
            if (abs(swap) < eps) continue; // для нулевого коэффициента пропустить
            for (int j = 0; j < end; j++)
                a[i][j] = a[i][j] / swap;
            y[i] = y[i] / swap;
            if (i == с)  continue; // уравнение не вычитать само из себя
            for (int j = 0; j < end; j++)
                a[i][j] = a[i][j] - a[с][j];
            y[i] = y[i] - y[с];
        }
        с++;
    }
    // обратная подстановка
    for (с = end - 1; с >= 0; с--){
        x[с] = y[с];
        for (int i = 0; i < с; i++)
            y[i] = y[i] - a[i][с] * x[с];
    }
    return x;
}


void main() {
    double** a, * y, * x;
    int n;
    cout << "Enter number of equations: ";
    cin >> n;
    a = new double* [n];
    y = new double[n];
    cout << "Do you want to enter values manually or randomly ?" << endl << "1 - manually, 2 - random" << endl;
    int input; cin >> input; cout << endl;
    switch (input) {
        case 1:
            for (int i = 0; i < n; i++){
                a[i] = new double[n];
                for (int j = 0; j < n; j++){
                    cout << "a[" << i << "][" << j << "]= ";
                    cin >> a[i][j];
                }
            }
            cout << endl;
            for (int i = 0; i < n; i++){
                cout << "y[" << i << "]= ";
                cin >> y[i];
            }
            break;

        case 2:
            srand(time(0));
            for (int i = 0; i < n; i++){
                a[i] = new double[n];
                for (int j = 0; j < n; j++){
                    a[i][j] = rand() % 10;
                    //cout << "a[" << i << "][" << j << "]= "<< a[i][j]<<"; ";
                }
            }
            cout << endl;
            for (int i = 0; i < n; i++){
                y[i] = rand() % 10;
                //cout << "y[" << i << "]= "<<y[i];
            }
            break;

        default:
            cout << "Error, bad input, quitting." << endl;
            break;
    }
    auto t0 = high_resolution_clock::now();
    x = gauss(a, y, 0, n);
    auto t1 = high_resolution_clock::now();
    cout << endl << "duration_cast gauss without multithreading: " << duration_cast<milliseconds>(t1 - t0).count() << " milliseconds" << endl;

    thread first(gauss, a, y, 0, n / 2);
    thread second(gauss, a, y, n / 2, n);

    t0 = high_resolution_clock::now();
    first.join(); second.join();
    t1 = high_resolution_clock::now();
    cout << "duration_cast gauss with multithreading: " << duration_cast<milliseconds>(t1 - t0).count() << " milliseconds" << endl << endl;

    //for (int i = 0; i < n; i++) // если нужно проверить ответ
        //cout << "x[" << i << "]=" << x[i] << endl;
    cin.get(); cin.get();
}
