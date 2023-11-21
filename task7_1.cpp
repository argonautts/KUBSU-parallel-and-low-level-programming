#include <iostream>
#include <Windows.h>
using namespace std;
bool isPrime(int n) {
    if (n <= 1)
        return false;

    for (int i = 2; i < n; i++)
        if (n % i == 0)
            return false;

    return true;
}
bool isComposite(int k) {
    for (int i = 2; i < k; i++)
        if (k % i == 0)
            return true;

    return false;
}
void main() {
    int n, k;
    cout << "Enter the Number to check Prime: "; cin >> n;
    if (isPrime(n)) {
        cout << n << " is a prime number" << endl << endl;
        __asm {
            mov eax, n;
            mov ebx, k;
            mov ebx, eax;
            add ebx, 1;
            mov k, ebx;
        }

        for (k; k <= 10000; k++) {
            if (isComposite(k)) {
                cout << k << " is a Composite number" << endl;
            }
            else {
                cout << endl << "Next prime number " << k << endl;
                cout << "Interval between prime numbers " << "[" << n << "; " << k << "]" << endl;
                break;
            }
        }
    }
    else
        cout << n << " is NOT a prime number" << endl;
}