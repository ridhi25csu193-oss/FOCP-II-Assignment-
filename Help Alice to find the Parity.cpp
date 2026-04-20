#include <iostream>
using namespace std;

int main() {
    long long L, R;
    cin >> L >> R;

    // count of odd numbers in range
    long long oddCount = (R + 1) / 2 - (L / 2);

    if (oddCount % 2 == 0)
        cout << "even";
    else
        cout << "odd";

    return 0;
}