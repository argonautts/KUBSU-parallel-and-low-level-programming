//Козин Александр ПМ 25/2 2 курс 02.03.03

#include <iostream>
#include <ctime>
#include <thread>
#include <chrono>
using namespace std;
using namespace std::chrono;

//Есть два очень длинных вектора, длинной в 10 тысяч, A и B.
//Заполнены целочисленными элементами.Сложить по координатно два вектора.
//Можно создать два процесса, один работает с одной половиной векторов, другой - с другой.
//сделать хрономитраж после 

const int n = 10000;

void vecA(int a[], int b[], int c[]) {
	for (int i = 0; i < n / 2; i++) {
		c[i] = a[i] + b[i];
	}
}

void vecB(int a[], int b[], int c[]) {
	for (int i = n / 2; i < n; i++) {
		c[i] = a[i] + b[i];
	}
}

void main() {

	setlocale(LC_ALL, "Ru");
	srand(time(0));

	auto t0 = high_resolution_clock::now();
	int a[n], b[n], c[n];
	for (int i = 0; i < n; i++) {
		a[i] = rand() % 1000;
		b[i] = rand() % 1000;
	}

	thread first(vecA, a, b, c);
	thread second(vecB, a, b, c);

	first.join();
	second.join();

	auto t1 = high_resolution_clock::now();

	cout << "Вектора a:";
	for (int i = 0; i < n; i++) {
		cout << a[i] << " ";
	}
	cout << endl << endl;

	cout << "Вектора b:";
	for (int i = 0; i < n; i++) {
		cout << b[i] << " ";
	}
	cout << endl << endl;

	cout << "Вектора c:";
	for (int i = 0; i < n; i++) {
		cout << c[i] << " ";
	}
	cout << endl << endl;

	cout << "Время вычисления: " << duration_cast<milliseconds>(t1 - t0).count() << "msec\n";
}