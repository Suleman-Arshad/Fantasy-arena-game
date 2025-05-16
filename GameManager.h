#pragma once
#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include <vector>
#include "Character.h"
#include "Arena.h"
using namespace std;
namespace FantasyArena {
    class GameManager {
    private:
        vector<Character*> characters;
        vector<Arena> arenas;
        bool gameRunning;
        // Save game data
        struct SaveData {
            int player1Index;
            int player2Index;
            int arenaIndex;
            int battleCount;
        };
        SaveData saveData;
    public:
        GameManager();
        ~GameManager();

        // Character management
        void addCharacter(Character* character);
        void displayCharacters() const;
        Character* selectCharacter(int index) const;

        // Arena management
        void addArena(const Arena& arena);
        void displayArenas() const;
        Arena* selectArena(int index);

        // Game flow
        void initializeGame();
        void runGame();
        void displayMainMenu() const;
        void battleMode();

        // Save/Load game
        void saveGame() const;
        bool loadGame();
        void saveGameMode();

        // Helper methods
        int getValidInput(int min, int max) const;
        void clearScreen() const;
        void pauseScreen() const;
    };

}
#endif  
