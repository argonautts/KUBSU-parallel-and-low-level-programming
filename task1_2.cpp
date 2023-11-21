#include <iostream>
#include <ctime>
#include <thread>
#include <chrono>
using namespace std;
using namespace std::chrono;

void dot_product1(int a[], int b[], int c[],int n) {
	for (int i = 0; i < n / 2; i++)
		c[i] += a[i] * b[i];
}

void dot_product2(int a[], int b[], int c[],int n) {
	for (int i = n / 2; i < n; i++)
		c[i] += a[i] * b[i];
}

void dot_product3(int a[], int b[], int d[],int n) {
	for (int i = 0; i < n; i++)
		d[i] += +a[i] * b[i];
}

void main() {
	setlocale(LC_ALL, "Ru");
	srand(time(0));
	cout << "Введите размер: "; int n; cin >> n;
	auto t0 = high_resolution_clock::now();
	int* a = new int[n]; int* b = new int[n];
	int* c = new int[n]; int* d = new int[n];
	for (int i = 0; i < n; i++) {
		a[i] = rand() % 10;
		b[i] = rand() % 10;
		c[i] = 0; d[i] = 0;
	}

	thread first(dot_product1, a, b, c,n); first.join();
	thread second(dot_product2, a, b, c,n);second.join();
	auto t1 = high_resolution_clock::now();
	thread third(dot_product3, a, b, d,n); third.join();
	auto t2 = high_resolution_clock::now();

	int sum = 0;
	for (int i = 0; i < n; i++) {
		sum += c[i];
	}
	cout << "sum: " << sum << endl << endl;
	cout << "Время вычисления параллельного: " << duration_cast<milliseconds>(t1 - t0).count() << "msec\n";
	cout << "Время вычисления прямого: " << duration_cast<milliseconds>(t2 - t0).count() << "msec\n";
	delete[] a, b, c, d;
}
