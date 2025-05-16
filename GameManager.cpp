#define _CRT_SECURE_NO_WARNINGS
#include "GameManager.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <fstream>
using namespace std;
namespace FantasyArena {
    GameManager::GameManager() : gameRunning(false) {
        // Initialize save data
        saveData.player1Index = -1;
        saveData.player2Index = -1;
        saveData.arenaIndex = -1;
        saveData.battleCount = 0;
    }
    GameManager::~GameManager() {
        // Clean up dynamically allocated characters
        for (auto character : characters) {
            delete character;
        }
        characters.clear();
    }
    void GameManager::addCharacter(Character* character) {
        characters.push_back(character);
    }
    void GameManager::displayCharacters() const {
        cout << "\n=== Available Characters ===" << endl;
        for (size_t i = 0; i < characters.size(); ++i) {
            cout << i + 1 << ". " << characters[i]->getName()
                << " (Level " << characters[i]->getLevel() << " "
                << characters[i]->getClassName() << ")" << endl;
        }
        std::cout << "===========================" << std::endl;
    }
    Character* GameManager::selectCharacter(int index) const {
        if (index >= 0 && static_cast<size_t>(index) < characters.size()) {
            return characters[index];
        }
        return nullptr;
    }
    void GameManager::addArena(const Arena& arena) {
        arenas.push_back(arena);
    }
    void GameManager::displayArenas() const {
        cout << "\n=== Available Arenas ===" << endl;
        for (size_t i = 0; i < arenas.size(); ++i) {
            cout << i + 1 << ". " << arenas[i].getName()
                << " (" << arenas[i].getEnvironmentName() << " environment)" << endl;
        }
        cout << "=======================" << endl;
    }
    Arena* GameManager::selectArena(int index) {
        if (index >= 0 && static_cast<size_t>(index) < arenas.size()) {
            return &arenas[index];
        }
        return nullptr;
    }
    void GameManager::initializeGame() {
        // Create default characters
        addCharacter(new Warrior("Aragorn", 5));
        addCharacter(new Warrior("Gimli", 4));
        addCharacter(new Mage("Gandalf", 6));
        addCharacter(new Mage("Saruman", 5));
        addCharacter(new Archer("Legolas", 5));
        addCharacter(new Archer("Hawkeye", 4));
        // Create arenas
        addArena(Arena("Mordor", EnvironmentType::FIRE));
        addArena(Arena("Frozen Wastes", EnvironmentType::ICE));
        addArena(Arena("Fangorn Forest", EnvironmentType::JUNGLE));
        addArena(Arena("Harad Desert", EnvironmentType::DESERT));
        addArena(Arena("Misty Mountains", EnvironmentType::MOUNTAIN));
        gameRunning = true;
    }
    void GameManager::runGame() {
        initializeGame();
        while (gameRunning) {
            clearScreen();
            displayMainMenu();
            int choice = getValidInput(1, 4);
            switch (choice) {
            case 1:
                battleMode();
                break;
            case 2:
                // Display game information
                clearScreen();
                cout << "\n=== Fantasy Arena Game Information ===" << endl;
                cout << "Fantasy Arena is a turn-based battle game where you can choose characters" << endl;
                cout << "from different classes and battle in various environments." << endl;
                cout << endl;
                cout << "Each character has unique abilities:" << endl;
                cout << "- Warriors: Tough fighters with Transparency ability to boost defense" << endl;
                cout << "- Mages: Powerful spellcasters with Mirror Image ability to boost defense" << endl;
                cout << "- Archers: Agile fighters with Precise Aim for guaranteed critical hits" << endl;
                cout << endl;
                cout << "Different arenas provide different environmental effects:" << endl;
                cout << "- Fire: Boosts attack, reduces defense" << endl;
                cout << "- Ice: Reduces attack, boosts defense" << endl;
                cout << "- Jungle: Balanced boost to attack and defense" << endl;
                cout << "- Desert: Major attack boost, slight health reduction" << endl;
                cout << "- Mountain: Major defense boost, attack reduction" << endl;
                cout << "======================================" << endl;
                pauseScreen();
                break;
            case 3:
                // Load saved game
                if (loadGame()) {
                    saveGameMode();
                }
                else {
                    cout << "No saved game found or error loading save file." << endl;
                    pauseScreen();
                }
                break;
            case 4:
                gameRunning = false;
                cout << "Thank you for playing Fantasy Arena!" << endl;
                break;
            }
        }
    }
    void GameManager::displayMainMenu() const {
        cout << "\n=== FANTASY ARENA ===" << endl;
        cout << "1. Battle Mode" << endl;
        cout << "2. Game Information" << endl;
        cout << "3. Load Saved Game" << endl;
        cout << "4. Exit Game" << endl;
        cout << "===================" << endl;
        cout << "Enter your choice (1-4): ";
    }
    void GameManager::battleMode() {
        clearScreen();
        cout << "\n=== BATTLE MODE ===" << endl;
        cout << "\nPlayer 1, select your character:" << endl;
        displayCharacters();
        cout << "Enter your choice (1-" << characters.size() << "): ";
        int player1Choice = getValidInput(1, characters.size()) - 1;
        Character* player1Character = selectCharacter(player1Choice);
        cout << "\nPlayer 2, select your character:" << endl;
        displayCharacters();
        cout << "Enter your choice (1-" << characters.size() << "): ";
        int player2Choice;
        do {
            player2Choice = getValidInput(1, characters.size()) - 1;
            if (player2Choice == player1Choice) {
                cout << "Please select a different character than Player 1." << endl;
            }
        } while (player2Choice == player1Choice);
        Character* player2Character = selectCharacter(player2Choice);
        cout << "\nSelect battle arena:" << endl;
        displayArenas();
        cout << "Enter your choice (1-" << arenas.size() << "): ";
        int arenaChoice = getValidInput(1, arenas.size()) - 1;
        Arena* selectedArena = selectArena(arenaChoice);
        saveData.player1Index = player1Choice;
        saveData.player2Index = player2Choice;
        saveData.arenaIndex = arenaChoice;
        saveData.battleCount++;
        cout << "\nDo you want to save this battle setup? (1: Yes, 2: No): ";
        int saveChoice = getValidInput(1, 2);
        if (saveChoice == 1) {
            saveGame();
            cout << "Game saved successfully!" << endl;
            pauseScreen();
        }
        clearScreen();
        cout << "\n=== BATTLE SETUP ===" << endl;
        cout << "Player 1: " << *player1Character << endl;
        cout << "Player 2: " << *player2Character << endl;
        cout << "Arena: " << *selectedArena << endl;
        cout << "===================" << endl;
        cout << "Press Enter to start the battle...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        clearScreen();
        selectedArena->startBattle(player1Character, player2Character);
        pauseScreen();
    }
    int GameManager::getValidInput(int min, int max) const {
        int choice;
        while (!(cin >> choice) || choice < min || choice > max) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between " << min << " and " << max << ": ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return choice;
    }
    void GameManager::clearScreen() const {
        // For Windows
#ifdef _WIN32
        system("cls");
        // For Unix/Linux
#else
        system("clear");
#endif
    }
    void GameManager::pauseScreen() const {
        cout << "\nPress Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    void GameManager::saveGame() const {
        ofstream saveFile("fantasy_arena_save.dat", ios::binary);
        if (saveFile.is_open()) {
            // Validate data before saving
            if (saveData.player1Index >= 0 && static_cast<size_t>(saveData.player1Index) < characters.size() &&
                saveData.player2Index >= 0 && static_cast<size_t>(saveData.player2Index) < characters.size() &&
                saveData.arenaIndex >= 0 && static_cast<size_t>(saveData.arenaIndex) < arenas.size()) {
                saveFile.write(reinterpret_cast<const char*>(&saveData), sizeof(SaveData));
                saveFile.close();
            }
            else {
                cout << "Error: Invalid save data." << endl;
                saveFile.close();
            }
        }
        else {
            cout << "Error: Could not create save file." << endl;
        }
    }
    bool GameManager::loadGame() {
        ifstream saveFile("fantasy_arena_save.dat", ios::binary);
        if (saveFile.is_open()) {
            // Check if file is empty
            saveFile.seekg(0, ios::end);
            if (saveFile.tellg() < static_cast<std::streampos>(sizeof(SaveData))) {
                cout << "Error: Save file is corrupted or incomplete." << endl;
                saveFile.close();
                return false;
            }
            // Reset to beginning of file and read data
            saveFile.seekg(0, ios::beg);
            saveFile.read(reinterpret_cast<char*>(&saveData), sizeof(SaveData));
            saveFile.close();
            // Validate the loaded data
            if (saveData.player1Index >= 0 && static_cast<size_t>(saveData.player1Index) < characters.size() &&
                saveData.player2Index >= 0 && static_cast<size_t>(saveData.player2Index) < characters.size() &&
                saveData.arenaIndex >= 0 && static_cast<size_t>(saveData.arenaIndex) < arenas.size() &&
                saveData.battleCount > 0) {
                return true;
            }
            else {
                cout << "Error: Save file contains invalid data." << endl;
            }
        }
        else {
            cout << "Error: Could not open save file." << endl;
        }
        return false;
    }
    void GameManager::saveGameMode() {
        clearScreen();
        cout << "\n=== SAVED GAME ===" << endl;
        // Display saved battle information
        cout << "\nLoaded saved battle #" << saveData.battleCount << ":" << endl;
        cout << "Player 1: " << characters[saveData.player1Index]->getName() <<
            " (" << characters[saveData.player1Index]->getClassName() << ")" << endl;
        cout << "Player 2: " << characters[saveData.player2Index]->getName() <<
            " (" << characters[saveData.player2Index]->getClassName() << ")" << endl;
        cout << "Arena: " << arenas[saveData.arenaIndex].getName() <<
            " (" << arenas[saveData.arenaIndex].getEnvironmentName() << ")" << endl;
        cout << "\nDo you want to start this battle? (1: Yes, 2: No): ";
        int choice = getValidInput(1, 2);
        if (choice == 1) {
            Character* player1Character = selectCharacter(saveData.player1Index);
            Character* player2Character = selectCharacter(saveData.player2Index);
            Arena* selectedArena = selectArena(saveData.arenaIndex);
            // Display selected characters and arena
            clearScreen();
            cout << "\n=== BATTLE SETUP ===" << endl;
            cout << "Player 1: " << *player1Character << endl;
            cout << "Player 2: " << *player2Character << endl;
            cout << "Arena: " << *selectedArena << endl;
            cout << "===================" << endl;
            cout << "Press Enter to start the battle...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            // Start the battle
            clearScreen();
            selectedArena->startBattle(player1Character, player2Character);
            pauseScreen();
        }
    }
} // namespace FantasyArena
