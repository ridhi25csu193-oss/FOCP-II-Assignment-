#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std;

class MovieTicket {
private:
    // Maps MovieID -> Set of UserIDs who have a booking
    // Using map/set instead of unordered_map/unordered_set
    map<int, set<int>> movieBookings;
    const int MAX_CAPACITY = 100;

public:
    bool book(int userId, int movieId) {
        set<int>& bookedUsers = movieBookings[movieId];

        // Check if movie is full or user already has a ticket
        if (bookedUsers.size() >= MAX_CAPACITY || bookedUsers.count(userId)) {
            return false;
        }

        bookedUsers.insert(userId);
        return true;
    }

    bool cancel(int userId, int movieId) {
        // Find the movie first to avoid creating an empty set if it doesn't exist
        auto it = movieBookings.find(movieId);
        if (it == movieBookings.end()) {
            return false;
        }

        set<int>& bookedUsers = it->second;
        // Check if user actually has a booking
        if (bookedUsers.erase(userId)) {
            return true; // erase returns 1 if element was found and removed
        }
        
        return false;
    }

    bool isBooked(int userId, int movieId) {
        auto it = movieBookings.find(movieId);
        if (it == movieBookings.end()) {
            return false;
        }
        return it->second.count(userId) > 0;
    }

    int availableTickets(int movieId) {
        auto it = movieBookings.find(movieId);
        if (it == movieBookings.end()) {
            return MAX_CAPACITY;
        }
        return MAX_CAPACITY - (int)it->second.size();
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int Q;
    if (!(cin >> Q)) return 0;

    MovieTicket system;
    string query;

    while (Q--) {
        cin >> query;
        if (query == "BOOK") {
            int x, y;
            cin >> x >> y;
            cout << (system.book(x, y) ? "true" : "false") << "\n";
        } else if (query == "CANCEL") {
            int x, y;
            cin >> x >> y;
            cout << (system.cancel(x, y) ? "true" : "false") << "\n";
        } else if (query == "IS_BOOKED") {
            int x, y;
            cin >> x >> y;
            cout << (system.isBooked(x, y) ? "true" : "false") << "\n";
        } else if (query == "AVAILABLE_TICKETS") {
            int y;
            cin >> y;
            cout << system.availableTickets(y) << "\n";
        }
    }

    return 0;
}