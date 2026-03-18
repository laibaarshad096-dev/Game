#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// ─── Colors ───────────────────────────────────
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"

// ─── Helpers ──────────────────────────────────
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printDivider() {
    cout << CYAN << "  ══════════════════════════════════════\n" << RESET;
}

void printHeader() {
    clearScreen();
    cout << "\n";
    cout << YELLOW << BOLD;
    cout << "   ██████╗ ██╗   ██╗███████╗███████╗███████╗\n";
    cout << "  ██╔════╝ ██║   ██║██╔════╝██╔════╝██╔════╝\n";
    cout << "  ██║  ███╗██║   ██║█████╗  ███████╗███████╗\n";
    cout << "  ██║   ██║██║   ██║██╔══╝  ╚════██║╚════██║\n";
    cout << "  ╚██████╔╝╚██████╔╝███████╗███████║███████║\n";
    cout << "   ╚═════╝  ╚═════╝ ╚══════╝╚══════╝╚══════╝\n";
    cout << RESET;
    cout << CYAN << "         I T  —  Number Guessing Game\n" << RESET;
    cout << "\n";
}

void printAttemptDots(int total, int used, bool lastWin, bool lastFail) {
    cout << "  Attempts: ";
    for (int i = 0; i < total; i++) {
        if (i < used - 1)
            cout << GREEN << "◆ " << RESET;
        else if (i == used - 1) {
            if (lastWin)       cout << GREEN  << "★ " << RESET;
            else if (lastFail) cout << RED    << "✗ " << RESET;
            else               cout << YELLOW << "◆ " << RESET;
        } else if (i == used)
            cout << YELLOW << "◇ " << RESET;
        else
            cout << CYAN << "◇ " << RESET;
    }
    cout << "\n";
}

void printHistory(const vector<pair<int,char>>& history) {
    if (history.empty()) return;
    cout << "  History: ";
    for (auto& h : history) {
        if (h.second == 'H')
            cout << RED   << "[↓" << h.first << "] " << RESET;
        else
            cout << BLUE  << "[↑" << h.first << "] " << RESET;
    }
    cout << "\n";
}

void printRangeBar(int lo, int hi, int maxRange, int guess = -1) {
    int barWidth = 36;
    float loF  = (float)(lo - 1)  / maxRange;
    float hiF  = (float)hi        / maxRange;
    float gF   = (guess > 0) ? (float)(guess - 1) / maxRange : -1;

    int loPos  = (int)(loF * barWidth);
    int hiPos  = (int)(hiF * barWidth);
    int gPos   = (gF >= 0) ? (int)(gF * barWidth) : -1;

    cout << "  1 [";
    for (int i = 0; i < barWidth; i++) {
        if (i == gPos)
            cout << YELLOW << "●" << RESET;
        else if (i >= loPos && i <= hiPos)
            cout << CYAN << "─" << RESET;
        else
            cout << "\033[90m·\033[0m";
    }
    cout << "] " << maxRange << "\n";
    cout << "  Range: " << GREEN << lo << RESET << " to " << GREEN << hi << RESET << "\n";
}

// ─── Difficulty ───────────────────────────────
struct Difficulty {
    string name;
    int maxNum;
    int tries;
};

Difficulty chooseDifficulty() {
    vector<Difficulty> levels = {
        {"EASY",   50,  8},
        {"MEDIUM", 100, 7},
        {"HARD",   200, 6}
    };

    printHeader();
    printDivider();
    cout << BOLD << "  SELECT DIFFICULTY\n" << RESET;
    printDivider();
    cout << "\n";
    cout << GREEN  << "  [1] EASY   " << RESET << "— 1 to 50   · 8 attempts\n";
    cout << YELLOW << "  [2] MEDIUM " << RESET << "— 1 to 100  · 7 attempts\n";
    cout << RED    << "  [3] HARD   " << RESET << "— 1 to 200  · 6 attempts\n";
    cout << "\n";
    cout << "  Choose (1/2/3): ";

    int choice;
    cin >> choice;
    if (choice < 1 || choice > 3) choice = 2;
    return levels[choice - 1];
}

// ─── Main Game ────────────────────────────────
bool playGame(Difficulty& diff) {
    srand(time(0));
    int secret   = rand() % diff.maxNum + 1;
    int attempts = 0;
    int lo = 1, hi = diff.maxNum;
    int guess;
    vector<pair<int,char>> history;
    bool won = false;

    while (attempts < diff.tries) {
        printHeader();
        printDivider();
        cout << BOLD << "  " << diff.name << " MODE"
             << "  —  1 to " << diff.maxNum << "\n" << RESET;
        printDivider();
        cout << "\n";

        printAttemptDots(diff.tries, attempts,
                         won,
                         attempts > 0 && !won && attempts == diff.tries);
        cout << "\n";
        printRangeBar(lo, hi, diff.maxNum,
                      history.empty() ? -1 : history.back().first);
        cout << "\n";
        printHistory(history);
        cout << "\n";

        int remaining = diff.tries - attempts;
        cout << "  " << YELLOW << remaining << RESET
             << " attempt(s) remaining\n\n";

        cout << "  " << BOLD << "Your guess (1–" << diff.maxNum << "): " << RESET;
        cin >> guess;

        // Validate
        if (guess < 1 || guess > diff.maxNum) {
            cout << RED << "\n  ⚠  Enter a number between 1 and "
                 << diff.maxNum << "!\n" << RESET;
            cin.clear();
            continue;
        }

        attempts++;

        if (guess == secret) {
            won = true;
            break;
        } else if (guess < secret) {
            lo = max(lo, guess + 1);
            history.push_back({guess, 'L'});
        } else {
            hi = min(hi, guess - 1);
            history.push_back({guess, 'H'});
        }

        // Odd/Even hint at halfway
        if (attempts == diff.tries / 2) {
            cout << MAGENTA << "\n  💡 Hint: The number is "
                 << (secret % 2 == 0 ? "EVEN" : "ODD") << RESET << "\n";
            cin.ignore(); cin.get();
        }
    }

    // ─── Result Screen ────────────────────────
    printHeader();
    printDivider();

    if (won) {
        cout << GREEN << BOLD;
        cout << "  ╔══════════════════════════════════╗\n";
        cout << "  ║         YOU WIN!  ★              ║\n";
        cout << "  ╚══════════════════════════════════╝\n";
        cout << RESET << "\n";
        cout << GREEN << "  Secret number: " << BOLD << secret << RESET << "\n";
        cout << GREEN << "  Guessed in:    " << BOLD << attempts
             << " attempt(s)" << RESET << "\n\n";

        if      (attempts == 1)          cout << YELLOW << "  🌟 LEGENDARY! First try!\n"   << RESET;
        else if (attempts <= diff.tries/3)   cout << YELLOW << "  🔥 Outstanding!\n"        << RESET;
        else if (attempts <= diff.tries*2/3) cout << GREEN  << "  👍 Well done!\n"          << RESET;
        else                             cout << CYAN   << "  😅 Just made it!\n"       << RESET;

    } else {
        cout << RED << BOLD;
        cout << "  ╔══════════════════════════════════╗\n";
        cout << "  ║         GAME OVER  ✗             ║\n";
        cout << "  ╚══════════════════════════════════╝\n";
        cout << RESET << "\n";
        cout << RED << "  Secret was: " << BOLD << secret << RESET << "\n";
        cout << RED << "  Better luck next time!\n" << RESET << "\n";
    }

    printDivider();
    printHistory(history);
    cout << "\n";
    cout << "  Play again? (y/n): ";
    char ch; cin >> ch;
    return (ch == 'y' || ch == 'Y');
}

// ─── Entry Point ──────────────────────────────
int main() {
    bool again = true;
    while (again) {
        Difficulty d = chooseDifficulty();
        again = playGame(d);
    }

    clearScreen();
    cout << "\n" << CYAN << BOLD
         << "  Thanks for playing GUESS IT!!\n\n" << RESET;
    return 0;
}