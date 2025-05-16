#include <iostream>
#include <cstdlib>
#include <ctime>
#include "GameManager.h"
using namespace std;
int main() {
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(nullptr)));
    // Display welcome message
    cout << "=========================================" << endl;
    cout << "   Welcome to Fantasy Arena Battle Game  " << endl;
    cout << "=========================================" << endl;
    cout << "A console-based turn-based battle game" << endl;
    cout << endl;
    // Create and run the game
    FantasyArena::GameManager gameManager;
    gameManager.runGame();
    system("pause");
    return 0;
}
