#pragma once
#ifndef ARENA_H
#define ARENA_H
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include "Character.h"
using namespace std;
namespace FantasyArena {
    enum class EnvironmentType {
        FIRE,
        ICE,
        JUNGLE,
        DESERT,
        MOUNTAIN
    };
    class Arena {
    private:
        string name;
        EnvironmentType environmentType;
        string logFileName;
        bool isLogFileOpen;
        int playerChoice; // Store player's action choice
    public:
        Arena(const string& name, EnvironmentType environmentType);
        ~Arena();
        // Getters
        string getName() const;
        EnvironmentType getEnvironmentType() const;
        string getEnvironmentName() const;
        // Apply environmental effects to characters
        void applyEnvironmentalEffects(Character* character);
        // Battle methods
        void startBattle(Character* player1, Character* player2);
        void processTurn(Character* attacker, Character* defender, int turnNumber);
        // Logging methods
        void openLogFile();
        void logEvent(const string& event);
        void closeLogFile();
        // Ability management methods
        void checkAndDeactivateAbilities(Character* character);
        void checkAndDeactivateAbilitiesWithoutCooldown(Character* character);
        void displayActiveAbilities(Character* character);
        // Display arena info
        friend std::ostream& operator<<(std::ostream& os, const Arena& arena);
    };
} // namespace FantasyArena
#endif // ARENA_H
