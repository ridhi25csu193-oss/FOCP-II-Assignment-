#include <iostream>
#include <string>
#include <map>

using namespace std;

class Bank {
private:
    // map stores UserID as key and Balance as value
    // Operations are O(log N)
    map<string, long long> accounts;

public:
    // CREATE X Y
    bool create(string x, long long y) {
        // Find if user exists
        auto it = accounts.find(x);
        if (it == accounts.end()) {
            accounts[x] = y;
            return true;
        } else {
            // User exists, add balance and return false
            it->second += y;
            return false;
        }
    }

    // DEBIT X Y
    bool debit(string x, long long y) {
        auto it = accounts.find(x);
        if (it != accounts.end() && it->second >= y) {
            it->second -= y;
            return true;
        }
        return false;
    }

    // CREDIT X Y
    bool credit(string x, long long y) {
        auto it = accounts.find(x);
        if (it != accounts.end()) {
            it->second += y;
            return true;
        }
        return false;
    }

    // BALANCE X
    long long balance(string x) {
        auto it = accounts.find(x);
        if (it != accounts.end()) {
            return it->second;
        }
        return -1;
    }
};

int main() {
    // Optimize I/O operations
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int Q;
    if (!(cin >> Q)) return 0;

    Bank myBank;

    while (Q--) {
        string type;
        cin >> type;

        if (type == "CREATE") {
            string x;
            long long y;
            cin >> x >> y;
            cout << (myBank.create(x, y) ? "true" : "false") << "\n";

        } else if (type == "DEBIT") {
            string x;
            long long y;
            cin >> x >> y;
            cout << (myBank.debit(x, y) ? "true" : "false") << "\n";

        } else if (type == "CREDIT") {
            string x;
            long long y;
            cin >> x >> y;
            cout << (myBank.credit(x, y) ? "true" : "false") << "\n";

        } else if (type == "BALANCE") {
            string x;
            cin >> x;
            cout << myBank.balance(x) << "\n";
        }
    }

    return 0;
}