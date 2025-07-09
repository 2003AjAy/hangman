#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <map>
#include <fstream>

using namespace std;

struct WordEntry {
    string word;
    string hint;
    string category;
};

struct Score {
    int wins = 0;
    int losses = 0;
    int highScore = 0;
};

vector<string> hangmanStates = {
    "\n\n\n\n\n\n========\n",
    "\n |\n |\n |\n |\n |\n========\n",
    " +---+\n |   |\n |\n |\n |\n |\n========\n",
    " +---+\n |   |\n |   O\n |\n |\n |\n========\n",
    " +---+\n |   |\n |   O\n |   |\n |\n |\n========\n",
    " +---+\n |   |\n |   O\n |  /|\n |\n |\n========\n",
    " +---+\n |   |\n |   O\n |  /|\\\n |\n |\n========\n",
    " +---+\n |   |\n |   O\n |  /|\\\n |  / \n |\n========\n",
    " +---+\n |   |\n |   O\n |  /|\\\n |  / \\\n |\n========\n"
};

void printCurrentState(const string& word, const vector<char>& guessed) {
    for (char c : word) {
        if (find(guessed.begin(), guessed.end(), c) != guessed.end()) {
            cout << c << ' ';
        } else {
            cout << "_ ";
        }
    }
    cout << endl;
}

void printMissedLetters(const string& word, const vector<char>& guessed) {
    cout << "Missed letters: ";
    for (char c : guessed) {
        if (word.find(c) == string::npos) {
            cout << c << ' ';
        }
    }
    cout << endl;
}

bool isWordGuessed(const string& word, const vector<char>& guessed) {
    for (char c : word) {
        if (find(guessed.begin(), guessed.end(), c) == guessed.end())
            return false;
    }
    return true;
}

vector<WordEntry> getWordsByCategory(const string& category) {
    vector<WordEntry> words;
    if (category == "Animals") {
        words = {
            {"elephant", "Largest land animal", "Animals"},
            {"giraffe", "Tallest animal", "Animals"},
            {"kangaroo", "Australian marsupial", "Animals"},
            {"penguin", "Flightless bird in Antarctica", "Animals"}
        };
    } else if (category == "Countries") {
        words = {
            {"canada", "Country with maple leaf flag", "Countries"},
            {"brazil", "Largest country in South America", "Countries"},
            {"japan", "Land of the Rising Sun", "Countries"},
            {"egypt", "Home of the pyramids", "Countries"}
        };
    } else if (category == "Sports") {
        words = {
            {"football", "Known as soccer in the US", "Sports"},
            {"cricket", "Popular in India and England", "Sports"},
            {"tennis", "Played with rackets and a yellow ball", "Sports"},
            {"hockey", "Played on ice or field", "Sports"}
        };
    } else if (category == "Technology") {
        words = {
            {"computer", "Electronic device for processing data", "Technology"},
            {"keyboard", "Input device with keys", "Technology"},
            {"internet", "Global network of computers", "Technology"},
            {"programming", "Writing code", "Technology"}
        };
    }
    return words;
}

vector<string> getCategories() {
    return {"Animals", "Countries", "Sports", "Technology"};
}

WordEntry getRandomWord(const vector<WordEntry>& words) {
    srand(time(0));
    int idx = rand() % words.size();
    return words[idx];
}

void saveScore(const Score& score) {
    ofstream ofs("score.txt");
    if (ofs) {
        ofs << score.wins << ' ' << score.losses << ' ' << score.highScore << endl;
    }
}

Score loadScore() {
    Score score;
    ifstream ifs("score.txt");
    if (ifs) {
        ifs >> score.wins >> score.losses >> score.highScore;
    }
    return score;
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void playGame(const WordEntry& entry, int maxWrong, Score& score, bool showHint, bool animate) {
    string word = entry.word;
    vector<char> guessed;
    int wrongGuesses = 0;
    
    while (wrongGuesses < maxWrong) {
        clearScreen();
        cout << "Category: " << entry.category << endl;
        cout << hangmanStates[wrongGuesses] << endl;
        printCurrentState(word, guessed);
        printMissedLetters(word, guessed);
        cout << "Guessed letters: ";
        for (char g : guessed) cout << g << ' ';
        cout << endl;
        if (showHint) {
            cout << "Hint: " << entry.hint << endl;
        }
        cout << "Enter your guess (or '!' for hint): ";
        char guess;
        cin >> guess;
        guess = tolower(guess);
        if (guess == '!') {
            cout << "Hint: " << entry.hint << endl;
            showHint = true;
            continue;
        }
        if (!isalpha(guess)) {
            cout << "Please enter a valid letter!\n";
            continue;
        }
        if (find(guessed.begin(), guessed.end(), guess) != guessed.end()) {
            cout << "You already guessed that letter!\n";
            continue;
        }
        guessed.push_back(guess);
        if (word.find(guess) == string::npos) {
            cout << "Wrong guess!\n";
            wrongGuesses++;
        } else {
            cout << "Good guess!\n";
        }
        if (isWordGuessed(word, guessed)) {
            cout << "\nCongratulations! You guessed the word: " << word << endl;
            score.wins++;
            if (score.wins > score.highScore) score.highScore = score.wins;
            saveScore(score);
            return;
        }
    }
    cout << hangmanStates[wrongGuesses] << endl;
    cout << "\nGame Over! The word was: " << word << endl;
    score.losses++;
    saveScore(score);
}

void playCustomWord(Score& score) {
    string customWord, customHint;
    cout << "Enter a word for your friend to guess: ";
    cin >> customWord;
    cout << "Enter a hint for this word: ";
    cin.ignore();
    getline(cin, customHint);
    WordEntry entry = {customWord, customHint, "Custom"};
    playGame(entry, hangmanStates.size() - 1, score, false, false);
}

void mainMenu() {
    Score score = loadScore();
    while (true) {
        clearScreen();
        cout << "=== Hangman Game ===" << endl;
        cout << "1. Play (Random Word)" << endl;
        cout << "2. Play (Choose Category)" << endl;
        cout << "3. Play (Custom Word)" << endl;
        cout << "4. Show Score" << endl;
        cout << "5. Exit" << endl;
        cout << "Choose an option: ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            // Random category
            vector<string> categories = getCategories();
            string category = categories[rand() % categories.size()];
            vector<WordEntry> words = getWordsByCategory(category);
            WordEntry entry = getRandomWord(words);
            playGame(entry, hangmanStates.size() - 1, score, false, false);
        } else if (choice == 2) {
            // Choose category
            vector<string> categories = getCategories();
            cout << "Available categories:\n";
            for (int i = 0; i < categories.size(); ++i) {
                cout << i + 1 << ". " << categories[i] << endl;
            }
            cout << "Select category: ";
            int catChoice;
            cin >> catChoice;
            if (catChoice < 1 || catChoice > categories.size()) {
                cout << "Invalid category!\n";
                continue;
            }
            vector<WordEntry> words = getWordsByCategory(categories[catChoice - 1]);
            WordEntry entry = getRandomWord(words);
            playGame(entry, hangmanStates.size() - 1, score, false, false);
        } else if (choice == 3) {
            playCustomWord(score);
        } else if (choice == 4) {
            cout << "Wins: " << score.wins << ", Losses: " << score.losses << ", High Score: " << score.highScore << endl;
            cout << "Press Enter to continue...";
            cin.ignore();
            cin.get();
        } else if (choice == 5) {
            break;
        } else {
            cout << "Invalid option!\n";
        }
    }
}

int main() {
    srand(time(0));
    mainMenu();
    return 0;
}
