#define _CRT_SECURE_NO_WARNINGS
#include "Character.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>
using namespace std;
namespace FantasyArena {
    // Initialize static members
    ofstream Character::logFile;
    string Character::environmentName = "Unknown";
    // Character implementation
    Character::Character(const std::string& name, int level, int health, int attack, int defense, int cooldown)
        : name(name), level(level), health(health), maxHealth(health), attack(attack), originalAttack(attack),
        defense(defense), originalDefense(defense), specialAbilityCooldown(cooldown), currentCooldown(0),
        abilityDuration(0), abilityStatus(SpecialAbilityStatus::READY) {
    }
    std::string Character::getName() const {
        return name;
    }
    int Character::getLevel() const {
        return level;
    }
    int Character::getHealth() const {
        return health;
    }
    int Character::getMaxHealth() const {
        return maxHealth;
    }
    int Character::getAttack() const {
        return attack;
    }
    int Character::getDefense() const {
        return defense;
    }
    SpecialAbilityStatus Character::getAbilityStatus() const {
        return abilityStatus;
    }
    int Character::getCurrentCooldown() const {
        return currentCooldown;
    }
    void Character::setHealth(int newHealth) {
        health = (newHealth < 0) ? 0 : newHealth;
    }
    void Character::setAttack(int newAttack) {
        attack = newAttack;
    }
    void Character::setDefense(int newDefense) {
        defense = newDefense;
    }
    void Character::resetCooldown() {
        currentCooldown = specialAbilityCooldown;
        abilityStatus = SpecialAbilityStatus::COOLDOWN;
    }
    void Character::decrementCooldown() {
        if (currentCooldown > 0) {
            currentCooldown--;
            if (currentCooldown == 0) {
                abilityStatus = SpecialAbilityStatus::READY;
            }
        }
    }
    bool Character::isAlive() const {
        return health > 0;
    }
    // Operator overloading
    int operator+(const Character& lhs, const Character& rhs) {
        // Return combined attack power
        return lhs.attack + rhs.attack;
    }
    bool operator==(const Character& lhs, const Character& rhs) {
        // Compare total power (attack + defense + health)
        int lhsPower = lhs.attack + lhs.defense + lhs.health;
        int rhsPower = rhs.attack + rhs.defense + rhs.health;
        return lhsPower == rhsPower;
    }std::ostream& operator<<(std::ostream& os, const Character& character) {
        os << "Name: " << character.name << "\n"
            << "Class: " << character.getClassName() << "\n"
            << "Level: " << character.level << "\n"
            << "Health: " << character.health << "/" << character.maxHealth << "\n"
            << "Attack: " << character.attack << "\n"
            << "Defense: " << character.defense << "\n";
        if (character.abilityStatus == SpecialAbilityStatus::READY) {
            os << "Special Ability: Ready\n";
        }
        else {
            os << "Special Ability: Cooldown (" << character.currentCooldown << " turns remaining)\n";
        }
        return os;
    }
    // Static methods for logging
    void Character::openLogFile() {
        // Create a timestamp for the log file name
        std::time_t now_time = std::time(nullptr);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", std::localtime(&now_time));
        // Include environment name in the filename
        std::string filename = environmentName + "_battle_log_" + std::string(buffer) + ".txt";
        logFile.open(filename);
        if (logFile.is_open()) {
            logFile << "=== Fantasy Arena Battle Log: " << environmentName << " Environment ===" << endl;
            logFile << "Log started at: " << put_time(localtime(&now_time), "%Y-%m-%d %H:%M:%S") << endl;
            logFile << "===============================" << endl << endl;
        }
        else {
            cout << "Error: Could not open log file." << endl;
        }
    }
    void Character::closeLogFile() {
        if (logFile.is_open()) {
            time_t now_time = time(nullptr);
            logFile << endl << "Log closed at: " << put_time(localtime(&now_time), "%Y-%m-%d %H:%M:%S") << endl;
            logFile.close();
        }
    }
    void Character::logAction(const string& action) {
        if (logFile.is_open()) {
            time_t now_time = time(nullptr);
            logFile << "[" << put_time(localtime(&now_time), "%H:%M:%S") << "] " << action << endl;
        }
    }
    // Warrior implementation
    Warrior::Warrior(const string& name, int level)
        : Character(name, level, 100 + (level * 20), 15 + (level * 3), 10 + (level * 2), 3), // Changed cooldown to 3
        transparentActive(false), transparentDuration(0) {
    }

    void Warrior::attackTarget(Character& target) {
        int damage = attack - (target.getDefense() / 2);
        if (damage < 1) damage = 1;

        target.setHealth(target.getHealth() - damage);

        string attackLog = name + " attacks " + target.getName() + " for " + to_string(damage) + " damage";
        if (transparentActive) {
            attackLog += " (Transparent active!)";
        }
        cout << attackLog << endl;
        logAction(attackLog);
    }

    void Warrior::useSpecialAbility() {
        if (abilityStatus == SpecialAbilityStatus::READY) {
            transparentActive = true;
            transparentDuration = 1; // Lasts for 1 turn
            string abilityLog = name + " becomes Transparent! Immune to attacks for 1 turn.";
            cout << abilityLog << endl;
            logAction(abilityLog);

            // Set cooldown
            resetCooldown();
            string cooldownStartMsg = name + "'s Transparent ability is now on cooldown for " + to_string(currentCooldown) + " turns.";
            cout << cooldownStartMsg << endl;
            logAction(cooldownStartMsg);
        }
        else {
            string cooldownMsg = name + "'s Transparent ability is on cooldown (" + to_string(currentCooldown) + " turns remaining)";
            cout << cooldownMsg << endl;
            logAction(cooldownMsg);
        }
    }



    std::string Warrior::getClassName() const {
        return "Warrior";
    }

    std::string Warrior::getSpecialAbilityName() const {
        return "Transparent";
    }

    bool Warrior::isTransparentActive() const {
        return transparentActive;
    }

    void Warrior::deactivateTransparent() {
        if (transparentActive) {
            transparentActive = false;
            transparentDuration = 3;
            string deactivateLog = name + "'s Transparent ability ends. No longer immune to attacks.";
            cout << deactivateLog << endl;
            logAction(deactivateLog);
        }
    }
    // Mage implementation
    Mage::Mage(const std::string& name, int level)
        : Character(name, level, 70 + (level * 15), 20 + (level * 3), 5 + (level * 1), 3),
        mirrorImageActive(false), mirrorImageDuration(0) {
    }
    void Mage::attackTarget(Character& target) {
        int damage = attack - (target.getDefense() / 3);
        if (damage < 1) damage = 1;
        target.setHealth(target.getHealth() - damage);
        string attackLog = name + " attacks " + target.getName() + " for " + to_string(damage) + " damage";
        if (mirrorImageActive) {
            attackLog += " (Mirror Image active!)";
        }
        logAction(attackLog);
    }
    void Mage::useSpecialAbility() {
        if (abilityStatus == SpecialAbilityStatus::READY) {
            // Mirror Image: Creates an illusory clone to make the next attack miss
            mirrorImageActive = true;
            mirrorImageDuration = 1; // Lasts for 1 turn

            string abilityLog = name + " creates a Mirror Image! The next attack will miss completely.";
            cout << abilityLog << endl;
            logAction(abilityLog);

            // Set cooldown
            resetCooldown();
            string cooldownStartMsg = name + "'s Mirror Image ability is now on cooldown for " + to_string(currentCooldown) + " turns.";
            cout << cooldownStartMsg << endl;
            logAction(cooldownStartMsg);
        }
        else {
            string cooldownMsg = name + "'s Mirror Image is on cooldown (" + to_string(currentCooldown) + " turns remaining)";
            cout << cooldownMsg << endl;
            logAction(cooldownMsg);
        }
    }


    std::string Mage::getClassName() const {
        return "Mage";
    }

    std::string Mage::getSpecialAbilityName() const {
        return "Mirror Image";
    }

    bool Mage::isMirrorImageActive() const {
        return mirrorImageActive;
    }

    void Mage::deactivateMirrorImage() {
        if (mirrorImageActive) {
            mirrorImageActive = false;
            mirrorImageDuration = 3;

            string deactivateLog = name + "'s Mirror Image fades away. No longer protected from attacks.";
            cout << deactivateLog << endl;
            logAction(deactivateLog);
        }
    }

    // Archer implementation
    Archer::Archer(const string& name, int level)
        : Character(name, level, 80 + (level * 15), 18 + (level * 3), 7 + (level * 2), 3), // Changed cooldown to 3
        evasiveRollActive(false), evasiveRollDuration(0) {
        srand(static_cast<unsigned int>(time(nullptr)));
    }

    void Archer::attackTarget(Character& target) {
        int targetDefense = target.getDefense();
        int effectiveDefense;

        // If Piercing Arrow is active, ignore 50% of target's defense
        if (evasiveRollActive) {
            effectiveDefense = static_cast<int>(targetDefense * (1.0f - 0.5f));
        }
        else {
            effectiveDefense = targetDefense;
        }

        int damage = attack - (effectiveDefense / 4);
        if (damage < 1) damage = 1;

        target.setHealth(target.getHealth() - damage);

        string attackLog = name + " attacks " + target.getName() + " for " + to_string(damage) + " damage";
        if (evasiveRollActive) {
            attackLog += " (Evasive Roll active!)";
        }
        logAction(attackLog);

        // Deactivate after one use
        if (evasiveRollActive) {
            deactivateEvasiveRoll();
        }
    }

    void Archer::useSpecialAbility() {
        if (abilityStatus == SpecialAbilityStatus::READY) {
            evasiveRollActive = true;
            evasiveRollDuration = 1; // Lasts for 1 turn

            string abilityLog = name + " performs an Evasive Roll! Will dodge the next attack completely.";
            cout << abilityLog << endl;
            logAction(abilityLog);

            // Set cooldown
            resetCooldown();
            string cooldownStartMsg = name + "'s Evasive Roll ability is now on cooldown for " + to_string(currentCooldown) + " turns.";
            cout << cooldownStartMsg << endl;
            logAction(cooldownStartMsg);
        }
        else {
            string cooldownMsg = name + "'s Evasive Roll is on cooldown (" + to_string(currentCooldown) + " turns remaining)";
            cout << cooldownMsg << endl;
            logAction(cooldownMsg);
        }
    }

    std::string Archer::getClassName() const {
        return "Archer";
    }

    std::string Archer::getSpecialAbilityName() const {
        return "Evasive Roll";
    }

    bool Archer::isEvasiveRollActive() const {
        return evasiveRollActive;
    }

    void Archer::deactivateEvasiveRoll() {
        if (evasiveRollActive) {
            evasiveRollActive = false;
            evasiveRollDuration = 0;

            string deactivateLog = name + "'s Evasive Roll ends. No longer able to dodge attacks.";
            cout << deactivateLog << endl;
            logAction(deactivateLog);
        }
    }

    // LegendaryCharacter implementation
    LegendaryCharacter::LegendaryCharacter(const std::string& name, int level)
        : Character(name, level, 120 + (level * 25), 22 + (level * 4), 12 + (level * 2), 0), // No cooldown for passive ability
        revived(false), resurrectionHealthPercent(0.25f) {
    }

    void LegendaryCharacter::attackTarget(Character& target) {
        int damage = attack - (target.getDefense() / 3);
        if (damage < 1) damage = 1;

        target.setHealth(target.getHealth() - damage);

        string attackLog = name + " attacks " + target.getName() + " for " + to_string(damage) + " damage";
        logAction(attackLog);
    }

    void LegendaryCharacter::useSpecialAbility() {
        // Resurrection is a passive ability that triggers automatically
        string abilityLog = name + "'s Resurrection ability is passive and will trigger automatically upon death.";
        cout << abilityLog << endl;
        logAction(abilityLog);
    }

    bool LegendaryCharacter::checkResurrection() {
        if (health <= 0 && !revived) {
            // Resurrect with 25% health
            health = static_cast<int>(maxHealth * resurrectionHealthPercent);
            revived = true;
            string resurrectionLog = name + " RESURRECTS with " + to_string(health) + " health!";
            cout << "\n*** " << resurrectionLog << " ***\n" << endl;
            logAction(resurrectionLog);

            return true;
        }
        return false;
    }

    string LegendaryCharacter::getClassName() const {
        return "LegendaryCharacter";
    }

    string LegendaryCharacter::getSpecialAbilityName() const {
        return "Resurrection";
    }

    bool LegendaryCharacter::hasResurrected() const {
        return revived;
    }

    // MirrorStriker implementation
    MirrorStriker::MirrorStriker(const string& name, int level)
        : Character(name, level, 90 + (level * 18), 16 + (level * 3), 9 + (level * 2), 3),
        mirrorStrikeActive(false), reflectionPercent(0.25f) {
    }

    void MirrorStriker::attackTarget(Character& target) {
        int damage = attack - (target.getDefense() / 3);
        if (damage < 1) damage = 1;

        target.setHealth(target.getHealth() - damage);

        string attackLog = name + " attacks " + target.getName() + " for " + to_string(damage) + " damage";
        if (mirrorStrikeActive) {
            attackLog += " (Mirror Strike active!)";
        }
        logAction(attackLog);
    }

    void MirrorStriker::useSpecialAbility() {
        if (abilityStatus == SpecialAbilityStatus::READY) {
            mirrorStrikeActive = true;

            string abilityLog = name + " activates Mirror Strike! Will reflect " +
                to_string(static_cast<int>(reflectionPercent * 100)) +
                "% of incoming damage back to attackers.";
            cout << abilityLog << endl;
            logAction(abilityLog);

            resetCooldown();
        }
        else {
            string cooldownMsg = name + "'s Mirror Strike is on cooldown (" + to_string(currentCooldown) + " turns remaining)";
            cout << cooldownMsg << endl;
            logAction(cooldownMsg);
        }
    }

    void MirrorStriker::reflectDamage(int damage, Character& attacker) {
        if (mirrorStrikeActive) {
            int reflectedDamage = static_cast<int>(damage * reflectionPercent);
            if (reflectedDamage < 1) reflectedDamage = 1;

            attacker.setHealth(attacker.getHealth() - reflectedDamage);

            string reflectLog = name + "'s Mirror Strike reflects " + to_string(reflectedDamage) +
                " damage back to " + attacker.getName() + "!";
            cout << reflectLog << endl;
            logAction(reflectLog);
        }
    }

    string MirrorStriker::getClassName() const {
        return "MirrorStriker";
    }

    string MirrorStriker::getSpecialAbilityName() const {
        return "Mirror Strike";
    }

    bool MirrorStriker::isMirrorStrikeActive() const {
        return mirrorStrikeActive;
    }

    void MirrorStriker::deactivateMirrorStrike() {
        if (mirrorStrikeActive) {
            mirrorStrikeActive = false;

            string deactivateLog = name + "'s Mirror Strike ends.";
            cout << deactivateLog << endl;
            logAction(deactivateLog);
        }
    }

} // namespace FantasyArena
