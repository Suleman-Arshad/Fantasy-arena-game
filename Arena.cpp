#define _CRT_SECURE_NO_WARNINGS //For warnings
#include "Arena.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <limits>
#include<string>
using namespace std;
namespace FantasyArena {
    Arena::Arena(const string& name, EnvironmentType environmentType)
        : name(name), environmentType(environmentType), isLogFileOpen(false), playerChoice(1) {
        // Generate a log file name based on arena name
        logFileName = "battle_log_" + name + ".txt";
        // We'll open the log file when needed
    }
    Arena::~Arena() {
        // No need to close anything here
    }
    string Arena::getName() const {
        return name;
    }
    EnvironmentType Arena::getEnvironmentType() const {
        return environmentType;
    }
    std::string Arena::getEnvironmentName() const {
        switch (environmentType) {
        case EnvironmentType::FIRE:
            return "Fire";
        case EnvironmentType::ICE:
            return "Ice";
        case EnvironmentType::JUNGLE:
            return "Jungle";
        case EnvironmentType::DESERT:
            return "Desert";
        case EnvironmentType::MOUNTAIN:
            return "Mountain";
        default:
            return "Unknown";
        }
    }
    void Arena::applyEnvironmentalEffects(Character* character) {
        string effectDescription;
        switch (environmentType) {
        case EnvironmentType::FIRE:
            // Fire arenas boost attack but reduce defense
            character->setAttack(character->getAttack() * 1.2);
            character->setDefense(character->getDefense() * 0.9);
            effectDescription = "The scorching heat of the fire arena boosts " + character->getName() +
                "'s attack but weakens their defense!";
            break;
        case EnvironmentType::ICE:
            // Ice arenas reduce attack speed but increase defense
            character->setAttack(character->getAttack() * 0.9);
            character->setDefense(character->getDefense() * 1.2);
            effectDescription = "The freezing cold of the ice arena slows " + character->getName() +
                "'s attacks but hardens their defense!";
            break;
        case EnvironmentType::JUNGLE:
            // Jungle arenas provide balanced stats
            character->setAttack(character->getAttack() * 1.1);
            character->setDefense(character->getDefense() * 1.1);
            effectDescription = "The lush jungle environment provides " + character->getName() +
                " with balanced stat boosts!";
            break;
        case EnvironmentType::DESERT:
            // Desert arenas increase attack but reduce health
            character->setAttack(character->getAttack() * 1.3);
            character->setHealth(character->getHealth() * 0.9);
            effectDescription = "The harsh desert sun empowers " + character->getName() +
                "'s attacks but drains their health!";
            break;
        case EnvironmentType::MOUNTAIN:
            // Mountain arenas increase defense but reduce attack
            character->setAttack(character->getAttack() * 0.8);
            character->setDefense(character->getDefense() * 1.4);
            effectDescription = "The high altitude of the mountain arena reduces " + character->getName() +
                "'s attack power but greatly enhances their defense!";
            break;
        }
        cout << effectDescription << endl;
        logEvent(effectDescription);
    }
   
    void Arena::startBattle(Character* player1, Character* player2) {
        Character::setEnvironmentName(getEnvironmentName());
        Character::openLogFile();

        string battleStart = "Battle started in " + name + " (" + getEnvironmentName() + " environment) between " +
            player1->getName() + " (" + player1->getClassName() + ") and " +
            player2->getName() + " (" + player2->getClassName() + ")";

        cout << "\n=== BATTLE START ===" << endl;
        cout << battleStart << endl;
        cout << "===================" << endl;
        Character::logAction(battleStart);

        applyEnvironmentalEffects(player1);
        applyEnvironmentalEffects(player2);

        cout << "\nInitial Stats:" << endl;
        cout << player1->getName() << ": " << player1->getHealth() << "/" << player1->getMaxHealth() << " HP" << endl;
        cout << player2->getName() << ": " << player2->getHealth() << "/" << player2->getMaxHealth() << " HP" << endl;

        cout << "\nPress Enter to start the battle...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        int turnNumber = 1;
        Character* currentAttacker = player1;
        Character* currentDefender = player2;

        while (player1->isAlive() && player2->isAlive()) {
            //  Decrement cooldown from second turn onward
            if (turnNumber > 1) {
                currentAttacker->decrementCooldown();
            }

            checkAndDeactivateAbilitiesWithoutCooldown(currentAttacker);
            processTurn(currentAttacker, currentDefender, turnNumber);

            if (!currentDefender->isAlive()) {
                if (currentDefender->getClassName() == "LegendaryCharacter") {
                    LegendaryCharacter* legendary = dynamic_cast<LegendaryCharacter*>(currentDefender);
                    if (legendary && !legendary->hasResurrected() && legendary->checkResurrection()) {
                        cout << "\n*** The battle continues! ***" << endl;
                    }
                    else {
                        break;
                    }
                }
                else {
                    break;
                }
            }

            cout << "\nPress Enter for next turn...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            std::swap(currentAttacker, currentDefender);
            ++turnNumber;
        }

        Character* winner = player1->isAlive() ? player1 : player2;
        Character* loser = player1->isAlive() ? player2 : player1;

        string battleEnd = "Battle ended! " + winner->getName() + " (" + winner->getClassName() + ") has defeated " +
            loser->getName() + " (" + loser->getClassName() + ") with " +
            to_string(winner->getHealth()) + " health remaining!";

        cout << "\n=== BATTLE END ===" << endl;
        cout << battleEnd << endl;
        cout << "=================" << endl;

        Character::logAction(battleEnd);
        Character::closeLogFile();
    }

    
void Arena::processTurn(Character* attacker, Character* defender, int turnNumber) {
    cout << "\n--- Turn " << turnNumber << " ---" << endl;
    cout << attacker->getName() << "'s turn" << endl;

    // Show current active or cooldown status
    displayActiveAbilities(attacker);

    Character::logAction("Turn " + std::to_string(turnNumber) + ": " + attacker->getName() + "'s turn");

    cout << "1. Attack" << endl;

    if (attacker->getAbilityStatus() == SpecialAbilityStatus::READY) {
        cout << "2. Use Special Ability: " << attacker->getSpecialAbilityName() << " (READY)" << endl;
    }
    else {
        cout << "Special Ability: " << attacker->getSpecialAbilityName()
            << " (COOLDOWN: " << attacker->getCurrentCooldown() << " turns remaining)" << endl;
    }

    int choice;
    if (attacker->getAbilityStatus() == SpecialAbilityStatus::READY) {
        cout << "Enter your choice (1-2): ";
        while (!(cin >> choice) || (choice != 1 && choice != 2)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter 1 or 2: ";
        }
    }
    else {
        cout << "Enter your choice (1): ";
        while (!(cin >> choice) || choice != 1) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Special ability is on cooldown. Please enter 1 to attack: ";
        }
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // ===== PERFORM ACTION =====
    if (choice == 1) {
        // Check for defensive abilities on defender
        bool skipAttack = false;

        if (defender->getClassName() == "Warrior") {
            Warrior* w = dynamic_cast<Warrior*>(defender);
            if (w && w->isTransparentActive()) {
                cout << attacker->getName() << "'s attack passes through " << defender->getName() << "'s transparent form!" << endl;
                Character::logAction(attacker->getName() + "'s attack missed due to Transparent.");
                skipAttack = true;
            }
        }
        else if (defender->getClassName() == "Mage") {
            Mage* m = dynamic_cast<Mage*>(defender);
            if (m && m->isMirrorImageActive()) {
                cout << attacker->getName() << "'s attack is fooled by " << defender->getName() << "'s mirror image!" << endl;
                Character::logAction(attacker->getName() + "'s attack missed due to Mirror Image.");
                m->deactivateMirrorImage();
                skipAttack = true;
            }
        }
        else if (defender->getClassName() == "Archer") {
            Archer* a = dynamic_cast<Archer*>(defender);
            if (a && a->isEvasiveRollActive()) {
                cout << defender->getName() << " dodges the attack with an Evasive Roll!" << endl;
                Character::logAction(attacker->getName() + "'s attack missed due to Evasive Roll.");
                a->deactivateEvasiveRoll();
                skipAttack = true;
            }
        }

        if (!skipAttack) {
            int beforeHP = defender->getHealth();
            attacker->attackTarget(*defender);
            int damage = beforeHP - defender->getHealth();

            if (defender->getClassName() == "MirrorStriker") {
                MirrorStriker* ms = dynamic_cast<MirrorStriker*>(defender);
                if (ms && ms->isMirrorStrikeActive()) {
                    ms->reflectDamage(damage, *attacker);
                }
            }
        }
    }
    else {
        // Use special ability
        cout << attacker->getName() << " uses " << attacker->getSpecialAbilityName() << "!" << endl;
        Character::logAction(attacker->getName() + " uses special ability: " + attacker->getSpecialAbilityName());
        attacker->useSpecialAbility();

        // Handle Archer auto-attack after activating ability
        if (attacker->getClassName() == "Archer") {
            Archer* archer = dynamic_cast<Archer*>(attacker);
            if (archer && archer->isEvasiveRollActive()) {
                cout << attacker->getName() << " attacks while in evasive stance!" << endl;
                int beforeHP = defender->getHealth();
                attacker->attackTarget(*defender);
                int damage = beforeHP - defender->getHealth();
                if (defender->getClassName() == "MirrorStriker") {
                    MirrorStriker* ms = dynamic_cast<MirrorStriker*>(defender);
                    if (ms && ms->isMirrorStrikeActive()) {
                        ms->reflectDamage(damage, *attacker);
                    }
                }
            }
        }

        // Reset cooldown only if ability was used
        attacker->resetCooldown();
        Character::logAction(attacker->getName() + "'s ability is now on cooldown (" +
            to_string(attacker->getCurrentCooldown()) + " turns).");
    }

    // ===== Update Display =====
    cout << "\nUpdated Stats:" << endl;
    cout << attacker->getName() << ": " << attacker->getHealth() << "/" << attacker->getMaxHealth() << " HP" << endl;
    cout << defender->getName() << ": " << defender->getHealth() << "/" << defender->getMaxHealth() << " HP" << endl;

    Character::logAction(attacker->getName() + " HP: " + to_string(attacker->getHealth()) + "/" + to_string(attacker->getMaxHealth()));
    Character::logAction(defender->getName() + " HP: " + to_string(defender->getHealth()) + "/" + to_string(defender->getMaxHealth()));
}

    void Arena::openLogFile() {
        // Open the log file for writing
        std::ofstream logFile(logFileName);

        if (logFile.is_open()) {
            // Get current time
            time_t now = time(0);
            char timeStr[100];
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));

            logFile << "=== Fantasy Arena Battle Log ===" << endl;
            logFile << "Arena: " << name << " (" << getEnvironmentName() << " environment)" << endl;
            logFile << "Log started at: " << timeStr << endl;
            logFile << "===============================" << std::endl << std::endl;

            logFile.close();
            isLogFileOpen = true;
        }
        else {
            std::cerr << "Error: Could not open log file." << std::endl;
        }
    }

    void Arena::logEvent(const std::string& event) {
        // Open the log file in append mode
        std::ofstream logFile(logFileName, std::ios::app);

        if (logFile.is_open()) {
            // Get current time
            time_t now = time(0);
            char timeStr[100];
            strftime(timeStr, sizeof(timeStr), "%H:%M:%S", localtime(&now));

            logFile << "[" << timeStr << "] " << event << std::endl;
            logFile.close();
        }
    }

    void Arena::closeLogFile() {
        if (isLogFileOpen) {
            // Open the log file in append mode
            std::ofstream logFile(logFileName, std::ios::app);

            if (logFile.is_open()) {
                // Get current time
                time_t now = time(0);
                char timeStr[100];
                strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));

                logFile << std::endl << "Log closed at: " << timeStr << std::endl;
                logFile.close();
            }

            isLogFileOpen = false;
        }
    }
    std::ostream& operator<<(std::ostream& os, const Arena& arena) {
        os << "Arena: " << arena.name << "\n"
            << "Environment: " << arena.getEnvironmentName() << "\n";
        // Add environment-specific descriptions
        switch (arena.environmentType) {
        case EnvironmentType::FIRE:
            os << "Effects: Increases attack by 20%, reduces defense by 10%\n";
            break;
        case EnvironmentType::ICE:
            os << "Effects: Reduces attack by 10%, increases defense by 20%\n";
            break;
        case EnvironmentType::JUNGLE:
            os << "Effects: Increases attack and defense by 10%\n";
            break;
        case EnvironmentType::DESERT:
            os << "Effects: Increases attack by 30%, reduces health by 10%\n";
            break;
        case EnvironmentType::MOUNTAIN:
            os << "Effects: Reduces attack by 20%, increases defense by 40%\n";
            break;
        }
        return os;
    }
    void Arena::checkAndDeactivateAbilities(Character* character) {
        // Decrement cooldown for all characters
        character->decrementCooldown();

        // Check and deactivate abilities
        checkAndDeactivateAbilitiesWithoutCooldown(character);
    }

    // New method that checks and deactivates abilities without affecting cooldown
    void Arena::checkAndDeactivateAbilitiesWithoutCooldown(Character* character) {
        // Check for Warrior's Transparent ability
        if (character->getClassName() == "Warrior") {
            Warrior* warrior = dynamic_cast<Warrior*>(character);
            if (warrior && warrior->isTransparentActive()) {
                // Deactivate Transparent after one turn
                warrior->deactivateTransparent();
            }
        }
        // Check for Mage's Mirror Image - deactivate at end of turn if not used
        else if (character->getClassName() == "Mage") {
            Mage* mage = dynamic_cast<Mage*>(character);
            if (mage && mage->isMirrorImageActive()) {
                mage->deactivateMirrorImage();
            }
        }
        // Check for Archer's Evasive Roll - deactivate at end of turn if not used
        else if (character->getClassName() == "Archer") {
            Archer* archer = dynamic_cast<Archer*>(character);
            if (archer && archer->isEvasiveRollActive()) {
                archer->deactivateEvasiveRoll();
            }
        }
        // Check for MirrorStriker's Mirror Strike
        else if (character->getClassName() == "MirrorStriker") {
            MirrorStriker* mirrorStriker = dynamic_cast<MirrorStriker*>(character);
            if (mirrorStriker && mirrorStriker->isMirrorStrikeActive()) {
                mirrorStriker->deactivateMirrorStrike();
            }
        }
    }

    void Arena::displayActiveAbilities(Character* character) {
        cout << "\n=== " << character->getName() << "'s Status ===" << endl;
        cout << "Health: " << character->getHealth() << "/" << character->getMaxHealth() << endl;

        // Display cooldown status regardless of active abilities
        if (character->getAbilityStatus() == SpecialAbilityStatus::COOLDOWN) {
            cout << "Ability Cooldown: " << character->getCurrentCooldown() << " turns remaining" << endl;
        }

        bool hasActiveAbility = false;

        // Check for Warrior's Transparent ability
        if (character->getClassName() == "Warrior") {
            Warrior* warrior = dynamic_cast<Warrior*>(character);
            if (warrior && warrior->isTransparentActive()) {
                cout << "[Active] Transparent - Immune to all attacks for this turn!" << endl;
                hasActiveAbility = true;
            }
            else if (warrior) {
                if (character->getAbilityStatus() == SpecialAbilityStatus::COOLDOWN) {
                    cout << "[Cooldown] Transparent - Ready in " << character->getCurrentCooldown() << " turns" << endl;
                }
                else {
                    cout << "[Ready] Transparent - Use special ability to become immune to attacks" << endl;
                }
                hasActiveAbility = true;
            }
        }
        // Check for Mage's Mirror Image
        else if (character->getClassName() == "Mage") {
            Mage* mage = dynamic_cast<Mage*>(character);
            if (mage && mage->isMirrorImageActive()) {
                cout << "[Active] Mirror Image - Next attack will miss completely!" << endl;
                hasActiveAbility = true;
            }
            else if (mage) {
                if (character->getAbilityStatus() == SpecialAbilityStatus::COOLDOWN) {
                    cout << "[Cooldown] Mirror Image - Ready in " << character->getCurrentCooldown() << " turns" << endl;
                }
                else {
                    cout << "[Ready] Mirror Image - Use special ability to make the next attack miss" << endl;
                }
                hasActiveAbility = true;
            }
        }
        // Check for Archer's Evasive Roll
        else if (character->getClassName() == "Archer") {
            Archer* archer = dynamic_cast<Archer*>(character);
            if (archer && archer->isEvasiveRollActive()) {
                cout << "[Active] Evasive Roll - Next attack will be dodged completely!" << endl;
                hasActiveAbility = true;
            }
            else if (archer) {
                if (character->getAbilityStatus() == SpecialAbilityStatus::COOLDOWN) {
                    cout << "[Cooldown] Evasive Roll - Ready in " << character->getCurrentCooldown() << " turns" << endl;
                }
                else {
                    cout << "[Ready] Evasive Roll - Use special ability to dodge the next attack" << endl;
                }
                hasActiveAbility = true;
            }
        }
        // Check for LegendaryCharacter's Resurrection
        else if (character->getClassName() == "LegendaryCharacter") {
            LegendaryCharacter* legendary = dynamic_cast<LegendaryCharacter*>(character);
            if (legendary) {
                if (legendary->hasResurrected()) {
                    cout << "[Used] Resurrection - Already used once this battle" << endl;
                }
                else {
                    cout << "[Passive] Resurrection - Will revive once with 25% health upon death" << endl;
                }
                hasActiveAbility = true;
            }
        }
        // Check for MirrorStriker's Mirror Strike
        else if (character->getClassName() == "MirrorStriker") {
            MirrorStriker* mirrorStriker = dynamic_cast<MirrorStriker*>(character);
            if (mirrorStriker && mirrorStriker->isMirrorStrikeActive()) {
                cout << "[Active] Mirror Strike - Reflects 25% of damage back to attacker!" << endl;
                hasActiveAbility = true;
            }
            else if (mirrorStriker) {
                if (character->getAbilityStatus() == SpecialAbilityStatus::COOLDOWN) {
                    cout << "[Cooldown] Mirror Strike - Ready in " << character->getCurrentCooldown() << " turns" << endl;
                }
                else {
                    cout << "[Ready] Mirror Strike - Use special ability to activate" << endl;
                }
                hasActiveAbility = true;
            }
        }

        if (!hasActiveAbility) {
            cout << "No active abilities" << endl;
        }

        cout << "====================" << endl;
    }
} // namespace FantasyArena
