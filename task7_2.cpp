#include <iostream>
#include <cmath>
using namespace std;

double get_PI() {
    double pi;
    __asm {
        fldpi;
        fstp pi;
    }
    return pi;
}

long get_factorial(int x) {
    long factor;
    __asm {
        mov ebx, 1
        mov eax, x
        call fact
        jmp quit

        fact :
        cmp eax, 1
            jle exitfact
            imul ebx, eax
            dec eax
            call fact
            exitfact :
        ret
            quit :
        mov factor, ebx
    }
    return factor;
}

long get_pow(int x, int y) {
    int m = 1;
    __asm
    {
        mov eax, x
        mov ebx, m
        LOOP1 :
        add ebx, 1
            mul x
            cmp ebx, y
            jb LOOP1
            mov x, eax
    }
    return x;
}

double get_add(int x, int y) {
    __asm {
        mov eax, x
        mov ebx, y
        add eax, ebx
        mov x, eax
    }
    return x;
}

double get_mul(int x, int y) {
    __asm {
        mov eax, x
        mov ebx, y
        mul ebx
        mov x, eax
    }
    return x;
}

double get_div(int x, int y) {
    __asm {
        mov eax, x
        mov ecx, y
        sub edx, edx
        div ecx
        mov x, eax
    }
    return x;
}

int main() {
    double e = 2.7183;
    double t; cout << "Enter t = "; cin >> t;
    int end; cout << "Enter end = "; cin >> end;
    double numerator = 0, denominator = 0, sum = 0;
    for (int n = 0; n < end; n++) {
        numerator = get_mul(get_pow(-1, n), get_factorial(2 * n));
        denominator = get_mul(get_factorial(n), get_pow(2 * t, 2 * n));
        sum = get_add(sum, get_div(numerator, denominator));
    }
    double PI = get_PI();
    double first = get_pow(e, get_div(get_pow(-t, 2), (t * sqrt(PI))));
    double x = get_add(first, sum);
    cout << "F(" << t << ") = " << x;   
}
