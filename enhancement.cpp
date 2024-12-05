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
    QJsonArray normalEnhancementJsons = FileUtils::loadJsonFile(
        "./data/enhancement/normal_enhancements.json");
    for (const QJsonValue &value : normalEnhancementJsons) {
        QJsonObject json = value.toObject();
        Enhancement *e = new Enhancement(json);
        normalEnhancements.append(e);
    }
}

Enhancement *EnhancementManager::generateNormalEnhancement(int level) {
    return normalEnhancements[0];
}

void EnhancementManager::applyEnhancement(Enhancement e) {
    if (e.type == "damageEnhancement") {
        damageEnhancement(e.parameters[0]);
    }
    if (e.type == "healthEnhancement") {
        damageEnhancement(e.parameters[0]);
    }
}

void EnhancementManager::damageEnhancement(double enhancement) {
    Weapon *weapon = player->getWeapon();
    double curDamage = weapon->getDamage();
    weapon->setDamage(curDamage + enhancement);
}

void EnhancementManager::healthEnhancement(double enhancement) {
    double curMaxHealth = player->getMaxHealth();
    player->setMaxHealth(curMaxHealth + enhancement);
}
