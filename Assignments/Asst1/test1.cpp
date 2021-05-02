#include<iostream>

using namespace std;

int main() {
    for (int i = 0; i < 10000000000; i++) {
        if (i % 1000000000 == 0) {
            cout << "yes" << endl;
        }
    }
    return 0;
}