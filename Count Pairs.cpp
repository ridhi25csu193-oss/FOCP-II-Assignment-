#include <iostream>
using namespace std;

int countPairs(int n) {
    int count = 0;

    // count distinct prime factors
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            count++;
            while (n % i == 0) {
                n /= i;
            }
        }
    }

    // if remaining n is prime
    if (n > 1) count++;

    // answer = 2^count
    int ans = 1;
    for (int i = 0; i < count; i++) {
        ans *= 2;
    }

    return ans;
}

int main() {
    int t;
    cin >> t;

    while (t--) {
        int n;
        cin >> n;
        cout << countPairs(n) << endl;
    }

    return 0;
}