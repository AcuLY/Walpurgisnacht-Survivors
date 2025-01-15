#include "enhancement.h"

Enhancement::Enhancement(QString type,
                         QString description,
                         QVector<double> &parameters,
                         QObject *parent)
    : QObject{parent}, type(type), description(description), parameters(parameters) {
}

Enhancement::Enhancement(const QJsonObject &json, QObject *parent) : QObject(parent) {
    type = json["type"].toString();
    description = json["description"].toString();

    QJsonArray paramArray = json["parameters"].toArray();
    for (const QJsonValue &value : paramArray) {
        parameters.append(value.toDouble());
    }
}

QString Enhancement::getType() {
    return type;
}

QString Enhancement::getDescription() {
    return description;
}

EnhancementManager::EnhancementManager(MagicalGirl *player, QObject *parent)
    : QObject(parent), player(player) {
    QJsonArray enhancementJsons = FileUtils::loadJsonFile(":/data/enhancement/enhancement_jsons");
    for (const QJsonValue &value : enhancementJsons) {
        QJsonObject json = value.toObject();
        Enhancement *e = new Enhancement(json);
        enhancements.append(e);
    }

    QJsonArray globalEnhancementJsons = FileUtils::loadJsonFile(
        ":/data/enhancement/global_enhancement_jsons");
    for (const QJsonValue &value : globalEnhancementJsons) {
        QJsonObject json = value.toObject();
        Enhancement *e = new Enhancement(json);
        globalEhancements.append(e);
    }
}

QVector<Enhancement *> EnhancementManager::generateEnhancement(MagicalGirl *player) {
    QVector<Enhancement *> randomEnhancements;
    QRandomGenerator *gen = QRandomGenerator::global();

    // 选择三个不同的随机强化
    while (randomEnhancements.size() < 3) {
        int randomIndex = gen->bounded(enhancements.size());
        Enhancement *randomEnhancement = enhancements[randomIndex];

        if ((randomEnhancement->getType() == "bulletSizeEnhancement"
             || randomEnhancement->getType() == "bulletVelocityEnhancement")
            && player->getWeaponType() == Weapon::WeaponType::Melee) {
            continue;
        }

        if (!randomEnhancements.contains(randomEnhancement)) {
            randomEnhancements.append(randomEnhancement);
        }
    }

    return randomEnhancements;
}

const QVector<Enhancement *> EnhancementManager::getGlobalEnhancements() const {
    return globalEhancements;
}

void EnhancementManager::applyEnhancement(Enhancement *e, int index) {
    QString type = e->getType();

    if (type == "damageEnhancement") {
        damageEnhancement(e->parameters[index]);
    }
    if (type == "attackSpeedEnhancement") {
        attackSpeedEnhancement(e->parameters[index]);
    }
    if (type == "attackRangeEnhancement") {
        attackRangeEnhancement(e->parameters[index]);
    }
    if (type == "bulletVelocityEnhancement") {
        bulletVelocityEnhancement(e->parameters[index]);
    }
    if (type == "bulletSizeEnhancement") {
        bulletSizeEnhancement(e->parameters[index]);
    }
    if (type == "multiAttackTimeEnhancement") {
        multiAttackTimeEnhancement(e->parameters[index]);
    }
    if (type == "maxHealthEnhancement") {
        maxHealthEnhancement(e->parameters[index]);
    }
    if (type == "healthRecoverIntervalEnhancement") {
        healthRecoverIntervalEnhancement(e->parameters[index]);
    }
    if (type == "healthRecoverRateEnhancement") {
        healthRecoverRateEnhancement(e->parameters[index]);
    }
    if (type == "healthRecoverManaCostEnhancement") {
        healthRecoverManaCostEnhancement(e->parameters[index]);
    }
    if (type == "outAttackIntervalEnhancement") {
        outAttackIntervalEnhancement(e->parameters[index]);
    }
    if (type == "invincibilityIntervalEnhancement") {
        invincibilityIntervalEnhancement(e->parameters[index]);
    }
    if (type == "maxManaEnhancement") {
        maxManaEnhancement(e->parameters[index]);
    }
    if (type == "manaRecoverEnhancement") {
        manaRecoverEnhancement(e->parameters[index]);
    }
    if (type == "maxVelocityEnhancement") {
        maxVelocityEnhancement(e->parameters[index]);
    }
    if (type == "attackDecayEnhancement") {
        attackDecayEnhancement(e->parameters[index]);
    }
    if (type == "pickRangeEnhancement") {
        pickupRangeEnhancement(e->parameters[index]);
    }
    if (type == "experienceGainEnhancement") {
        experienceGainEnhancement(e->parameters[index]);
    }
}

void EnhancementManager::damageEnhancement(double value) {
    player->getWeapon()->increaseDamage(value);
}

void EnhancementManager::attackSpeedEnhancement(double value) {
    player->increaseAttackSpeed(value);
}

void EnhancementManager::attackRangeEnhancement(double value) {
    player->getWeapon()->increaseRangeSize(value);
}

void EnhancementManager::bulletVelocityEnhancement(double value) {
    dynamic_cast<RemoteWeapon *>(player->getWeapon())->increaseBulletVelocity(value);
}

void EnhancementManager::bulletSizeEnhancement(double value) {
    dynamic_cast<RemoteWeapon *>(player->getWeapon())->increaseBulletSize(value);
}

void EnhancementManager::multiAttackTimeEnhancement(int value) {
    player->increaseMultiAttackTime(value);
}

void EnhancementManager::maxHealthEnhancement(double value) {
    player->increaseMaxHealth(value);
}

void EnhancementManager::healthRecoverIntervalEnhancement(double value) {
    player->decreaseRecoverInterval(value);
}

void EnhancementManager::healthRecoverRateEnhancement(double value) {
    player->increaseRecoverRate(value);
}

void EnhancementManager::healthRecoverManaCostEnhancement(double value) {
    player->decreaseRecoverManaCost(value);
}

void EnhancementManager::outAttackIntervalEnhancement(double value) {
    player->decreaseOutAttackInterval(value);
}

void EnhancementManager::invincibilityIntervalEnhancement(double value) {
    player->increaseInvincibleInterval(value);
}

void EnhancementManager::maxManaEnhancement(double value) {
    player->increaseMaxMana(value);
}

void EnhancementManager::manaRecoverEnhancement(double value) {
    player->increaseManaRecoverBonus(value);
}

void EnhancementManager::maxVelocityEnhancement(double value) {
    player->increaseMaxVelocity(value);
}

void EnhancementManager::attackDecayEnhancement(double value) {
    player->decreaseAttackDecay(value);
}

void EnhancementManager::pickupRangeEnhancement(double value) {
    player->increasePickRange(value);
}

void EnhancementManager::experienceGainEnhancement(double value) {
    player->increaseExperienceBonus(value);
}
