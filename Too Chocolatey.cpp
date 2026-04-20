#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

int main() {
    int T, N;
    cin >> T;

    while(T--) {
        cin >> N;

        vector<int> A(N);
        for(int i = 0; i < N; i++) {
            cin >> A[i];
        }

        sort(A.begin(), A.end(), greater<int>());

        int a = 0, b = 0;
        bool turn = true;

        int count = 1;

        for(int i = 0; i < N; i++) {

            if(i > 0) {
                if(A[i] == A[i-1]) count++;
                else count = 1;
            }

            if(count > 2) continue; 

            if(turn) a += A[i];
            else b += A[i];

            turn = !turn;
        }

        if(a > b) cout << "Alex\n";
        else cout << "Bob\n";
    }

    return 0;
}