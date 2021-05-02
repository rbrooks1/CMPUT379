#include<iostream>

using namespace std;

int main() {
    double pi = 0;
    int c = 0;
    for (float i = 1; i < 1000000000; i = i + 2) {
        if (c == 0) {
            pi = pi + (4/i);
            c++;
        } else {
            pi = pi - (4/i);
            c--;
        }
    }
    return 0;    
}