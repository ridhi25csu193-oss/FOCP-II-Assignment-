#include <iostream>
using namespace std;

bool check(string s, int n, int k, int l) {
    int count = 0;

    for (int i = 0; i < n; i++) {
        if (s[i] == '1') {
            count++;
            i = i + l - 1; // skip next l-1 bulbs
        }
    }

    return count <= k;
}

int main() {
    int n, k;
    cin >> n >> k;

    string s;
    cin >> s;

    int low = 1, high = n, ans = n;

    while (low <= high) {
        int mid = (low + high) / 2;

        if (check(s, n, k, mid)) {
            ans = mid;
            high = mid - 1; // try smaller l
        } else {
            low = mid + 1;
        }
    }

    cout << ans;
    return 0;
}