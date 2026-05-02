/*
 * ============================================
 *     RANDOM STUDENT EXCUSE GENERATOR
 * ============================================
 *
 * This beginner-friendly C++ program generates
 * funny and creative excuses for students.
 *
 * How it works:
 *   1. The user enters a student's name.
 *   2. A random excuse template is picked from
 *      a list of 15 templates.
 *   3. The placeholder {name} in the template
 *      is replaced with the student's actual name.
 *   4. The personalised excuse is displayed.
 *
 * Concepts used:
 *   - vectors, strings, random numbers,
 *     string replacement, input handling
 */

#include <iostream>   // for cin, cout
#include <string>     // for string operations
#include <vector>     // for storing excuse templates
#include <cstdlib>    // for rand() and srand()
#include <ctime>      // for time() — used to seed the random number generator

using namespace std;

// -----------------------------------------------
// Function: replacePlaceholder
// Purpose : Replace every occurrence of {name}
//           in a template string with the actual
//           student name.
// -----------------------------------------------
string replacePlaceholder(string templateStr, const string& name) {
    // The placeholder we want to find
    string placeholder = "{name}";

    // Keep searching for {name} and replacing it
    // until no more occurrences are left.
    size_t pos = templateStr.find(placeholder);
    while (pos != string::npos) {
        templateStr.replace(pos, placeholder.length(), name);
        // Search again starting after the replaced text
        pos = templateStr.find(placeholder, pos + name.length());
    }

    return templateStr;
}

int main() {
    // ------------------------------------------
    // STEP 1: Seed the random number generator
    // ------------------------------------------
    // time(0) returns the current time in seconds.
    // Using it as a seed ensures we get different
    // random numbers on every run.
    srand(static_cast<unsigned int>(time(0)));

    // ------------------------------------------
    // STEP 2: Store excuse templates in a vector
    // ------------------------------------------
    // Each string contains {name} as a placeholder
    // that will be replaced with the user's input.
    vector<string> excuses;

    excuses.push_back("{name} couldn't submit the assignment because their dog ate the laptop charger... and then the laptop.");
    excuses.push_back("{name} was about to start studying but then remembered that procrastination is also a skill.");
    excuses.push_back("{name} would have come to class, but their alarm clock chose to take a personal day.");
    excuses.push_back("{name} tried to do the homework, but Wikipedia sent them down a 6-hour rabbit hole about penguins.");
    excuses.push_back("{name} couldn't attend the lecture because they were stuck in an intense staring contest with their cat.");
    excuses.push_back("{name} was going to submit on time, but their Wi-Fi decided to take a vacation.");
    excuses.push_back("{name} accidentally saved the assignment in a parallel universe and can't access it from this one.");
    excuses.push_back("{name} didn't do the project because they were busy inventing a time machine to get more time.");
    excuses.push_back("{name} swears the assignment was done, but a rogue software update deleted everything at 11:59 PM.");
    excuses.push_back("{name} couldn't focus on studying because the refrigerator kept calling their name.");
    excuses.push_back("{name} was ready for the exam, but their brain decided to install updates and restart.");
    excuses.push_back("{name} missed the deadline because they spent 4 hours choosing the perfect font for the title page.");
    excuses.push_back("{name} tried to email the assignment, but their inbox was full of motivational quotes instead.");
    excuses.push_back("{name} would have finished the project, but Netflix auto-played the next episode... 7 times.");
    excuses.push_back("{name} claims the textbook was written in a language they haven't unlocked yet.");

    // ------------------------------------------
    // STEP 3: Ask the user for the student's name
    // ------------------------------------------
    string name;

    cout << "============================================" << endl;
    cout << "   WELCOME TO THE STUDENT EXCUSE GENERATOR!" << endl;
    cout << "============================================" << endl;
    cout << endl;
    cout << "Enter the student's name: ";

    // Use getline so that names with spaces
    // (e.g. "John Doe") are read properly.
    getline(cin, name);

    // ------------------------------------------
    // STEP 4: Handle empty input (edge case)
    // ------------------------------------------
    // If the user just presses Enter without
    // typing anything, we give them a default name.
    if (name.empty()) {
        name = "Anonymous Student";
        cout << endl;
        cout << "(No name entered, so we'll call them \"" << name << "\")" << endl;
    }

    // ------------------------------------------
    // STEP 5: Pick a random excuse from the list
    // ------------------------------------------
    // rand() % excuses.size() gives a random index
    // between 0 and (number of excuses - 1).
    int randomIndex = rand() % excuses.size();

    // Get the template at that random index
    string selectedExcuse = excuses[randomIndex];

    // ------------------------------------------
    // STEP 6: Replace {name} with the student name
    // ------------------------------------------
    string finalExcuse = replacePlaceholder(selectedExcuse, name);

    // ------------------------------------------
    // STEP 7: Display the excuse
    // ------------------------------------------
    cout << endl;
    cout << "--------------------------------------------" << endl;
    cout << "  TODAY'S EXCUSE:" << endl;
    cout << "--------------------------------------------" << endl;
    cout << endl;
    cout << "  " << finalExcuse << endl;
    cout << endl;
    cout << "--------------------------------------------" << endl;
    cout << "  Good luck, " << name << "! :D" << endl;
    cout << "--------------------------------------------" << endl;

    return 0;
}
