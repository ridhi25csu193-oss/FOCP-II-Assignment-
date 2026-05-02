/*
 * ============================================
 *        FUNNY ROAST MESSAGE GENERATOR
 * ============================================
 *
 * This beginner-friendly C++ program generates
 * funny roast messages for a given user name.
 *
 * How it works:
 *   1. The user enters their name.
 *   2. A random roast template is picked from
 *      a list of 15 templates.
 *   3. The placeholder {name} in the template
 *      is replaced with the user's actual name.
 *   4. The personalised roast is displayed.
 *
 * Concepts used:
 *   - vectors, strings, random numbers,
 *     string replacement, input handling
 */

#include <iostream>   // for cin, cout
#include <string>     // for string operations
#include <vector>     // for storing roast templates
#include <cstdlib>    // for rand() and srand()
#include <ctime>      // for time() — used to seed the random number generator

using namespace std;

// -----------------------------------------------
// Function: replacePlaceholder
// Purpose : Replace every occurrence of {name}
//           in a template string with the actual
//           user name.
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
    // STEP 2: Store roast templates in a vector
    // ------------------------------------------
    // Each string contains {name} as a placeholder
    // that will be replaced with the user's input.
    vector<string> roasts;

    roasts.push_back("Hey {name}, you bring everyone so much joy... when you leave the room.");
    roasts.push_back("{name}, if brains were dynamite, you wouldn't have enough to blow your nose.");
    roasts.push_back("{name}, you're proof that even evolution makes mistakes sometimes.");
    roasts.push_back("{name}, I'd explain it to you, but I left my crayons at home.");
    roasts.push_back("{name}, you're like a cloud. Everything brightens up when you disappear.");
    roasts.push_back("Hey {name}, I'd agree with you, but then we'd both be wrong.");
    roasts.push_back("{name}, you have something on your chin... no, the third one.");
    roasts.push_back("{name}, if you were any more average, you'd be a median.");
    roasts.push_back("{name}, you're not stupid. You just have bad luck when thinking.");
    roasts.push_back("Hey {name}, your secrets are always safe with me. I never even listen when you tell me them.");
    roasts.push_back("{name}, I'm not saying you're boring, but you make plain toast look exciting.");
    roasts.push_back("{name}, if laziness were an Olympic sport, you'd come in fourth because you'd be too lazy to get a medal.");
    roasts.push_back("{name}, you're the reason they put instructions on shampoo bottles.");
    roasts.push_back("Hey {name}, roses are red, violets are blue. I have five fingers, and the middle one is for you.");
    roasts.push_back("{name}, you're like a software update. Every time I see you, I think 'not now.'");

    // ------------------------------------------
    // STEP 3: Ask the user for their name
    // ------------------------------------------
    string name;

    cout << "============================================" << endl;
    cout << "     WELCOME TO THE ROAST GENERATOR!" << endl;
    cout << "============================================" << endl;
    cout << endl;
    cout << "Enter your name: ";

    // Use getline so that names with spaces
    // (e.g. "John Doe") are read properly.
    getline(cin, name);

    // ------------------------------------------
    // STEP 4: Handle empty input (edge case)
    // ------------------------------------------
    // If the user just presses Enter without
    // typing anything, we give them a default name.
    if (name.empty()) {
        name = "Mystery Person";
        cout << endl;
        cout << "(You didn't enter a name, so we'll call you \"" << name << "\")" << endl;
    }

    // ------------------------------------------
    // STEP 5: Pick a random roast from the list
    // ------------------------------------------
    // rand() % roasts.size() gives a random index
    // between 0 and (number of roasts - 1).
    int randomIndex = rand() % roasts.size();

    // Get the template at that random index
    string selectedRoast = roasts[randomIndex];

    // ------------------------------------------
    // STEP 6: Replace {name} with the user's name
    // ------------------------------------------
    string finalRoast = replacePlaceholder(selectedRoast, name);

    // ------------------------------------------
    // STEP 7: Display the roast
    // ------------------------------------------
    cout << endl;
    cout << "--------------------------------------------" << endl;
    cout << "  HERE'S YOUR ROAST:" << endl;
    cout << "--------------------------------------------" << endl;
    cout << endl;
    cout << "  " << finalRoast << endl;
    cout << endl;
    cout << "--------------------------------------------" << endl;
    cout << "  Thanks for playing, " << name << "! :P" << endl;
    cout << "--------------------------------------------" << endl;

    return 0;
}
