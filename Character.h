#pragma once
#ifndef CHARACTER_H
#define CHARACTER_H
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
namespace FantasyArena {
    enum class SpecialAbilityStatus {
        READY,
        COOLDOWN
    };
    // Forward declaration for attack reflection
    class Character;
    class Character {
    protected:
        string name;
        int level;
        int health;
        int maxHealth;
        int attack;
        int originalAttack;  // Store original attack value
        int defense;
        int originalDefense; // Store original defense value
        int specialAbilityCooldown;
        int currentCooldown;
        int abilityDuration; // Duration of ability effects in turns
        SpecialAbilityStatus abilityStatus;
        static ofstream logFile; // Static log file for all characters
        static string environmentName; // Static environment name for log file
    public:
        Character(const string& name, int level, int health, int attack, int defense, int cooldown);
        virtual ~Character() = default;
        // Getters
        std::string getName() const;
        int getLevel() const;
        int getHealth() const;
        int getMaxHealth() const;
        int getAttack() const;
        int getDefense() const;
        SpecialAbilityStatus getAbilityStatus() const;
        int getCurrentCooldown() const;
        // Setters
        void setHealth(int health);
        void setAttack(int attack);
        void setDefense(int defense);
        void resetCooldown();
        void decrementCooldown();
        // Pure virtual methods
        virtual void attackTarget(Character& target) = 0;
        virtual void useSpecialAbility() = 0;
        virtual string getClassName() const = 0;
        virtual string getSpecialAbilityName() const = 0;
        // Static method to open and close log file
        static void openLogFile();
        static void closeLogFile();
        static void logAction(const string& action);
        static void setEnvironmentName(const string& envName) { environmentName = envName; }
        // Check if character is alive
        bool isAlive() const;
        // Operator overloading
        friend int operator+(const Character& lhs, const Character& rhs);
        friend bool operator==(const Character& lhs, const Character& rhs);
        friend ostream& operator<<(ostream& os, const Character& character);
    };

    class Warrior : public Character {
    private:
        bool transparentActive; // Flag to track if invisibility is active
        int transparentDuration; // Duration counter for invisibility
    public:
        Warrior(const string& name, int level);
        void attackTarget(Character& target) override;
        void useSpecialAbility() override;
        string getClassName() const override;
        string getSpecialAbilityName() const override;
        bool isTransparentActive() const; // Check if invisibility is active
        void deactivateTransparent(); // Deactivate invisibility
    };

    class Mage : public Character {
    private:
        bool mirrorImageActive; // Flag to track if Mirror Image is active
        int mirrorImageDuration; // Duration counter for Mirror Image
    public:
        Mage(const string& name, int level);
        void attackTarget(Character& target) override;
        void useSpecialAbility() override;
        string getClassName() const override;
        string getSpecialAbilityName() const override;
        bool isMirrorImageActive() const; // Check if Mirror Image is active
        void deactivateMirrorImage(); // Deactivate Mirror Image
    };

    class Archer : public Character {
    private:
        bool evasiveRollActive; // Flag to track if Evasive Roll is active
        int evasiveRollDuration; // Duration counter for Evasive Roll
    public:
        Archer(const string& name, int level);
        void attackTarget(Character& target) override;
        void useSpecialAbility() override;
        string getClassName() const override;
        string getSpecialAbilityName() const override;
        bool isEvasiveRollActive() const; // Check if Evasive Roll is active
        void deactivateEvasiveRoll(); // Deactivate Evasive Roll
    };

    class LegendaryCharacter : public Character {
    private:
        bool revived;
        float resurrectionHealthPercent;
    public:
        LegendaryCharacter(const string& name, int level);
        void attackTarget(Character& target) override;
        void useSpecialAbility() override; // Resurrection is passive
        string getClassName() const override;
        string getSpecialAbilityName() const override;
        bool checkResurrection(); // Check if character should resurrect
        bool hasResurrected() const;
    };

    class MirrorStriker : public Character {
    private:
        bool mirrorStrikeActive;
        float reflectionPercent;
    public:
        MirrorStriker(const string& name, int level);
        void attackTarget(Character& target) override;
        void useSpecialAbility() override;
        string getClassName() const override;
        string getSpecialAbilityName() const override;
        bool isMirrorStrikeActive() const;
        void reflectDamage(int damage, Character& attacker); // Reflect damage back to attacker
        void deactivateMirrorStrike();
    };
} // namespace FantasyArena
#endif // CHARACTER_H
