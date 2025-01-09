#include "witch.h"

Witch::Witch(QString name,
             int width,
             int height,
             double maxHealth,
             double maxVelocity,
             double accelerationFactor,
             double attackMoveDecayFactor,
             double reboundFactor,
             int exp,
             int attackWaitTime,
             Weapon *weapon,
             QWidget *parent)
    : Character(name,
                width,
                height,
                maxHealth,
                maxVelocity,
                accelerationFactor,
                attackMoveDecayFactor,
                reboundFactor,
                weapon,
                parent),
      exp(exp), attackWaitTime(attackWaitTime) {
}

int Witch::chooseWitch(double progress) {
    //
    double rand = QRandomGenerator::global()->bounded(1.0);

    if (rand < progress) {
        return 0;
    }

    return -1;
}

Witch *Witch::loadWitchFromJson(int typeIndex, Map *map) {
    QJsonArray basicJsons = FileUtils::loadJsonFile(":/data/witch/witches_basic_jsons");
    QJsonArray weaponJsons = FileUtils::loadJsonFile(":/data/witch/witches_weapons_jsons");

    QJsonObject basicJson = basicJsons[typeIndex].toObject();
    QJsonObject weaponJson = weaponJsons[typeIndex].toObject();

    Weapon *weapon;
    if (weaponJson["isRemoteWeapon"].toBool()) {
        int bulletVelocity = weaponJson["bulletVelocity"].toInt();
        int bulletSize = weaponJson["bulletSize"].toInt();
        int damage = weaponJson["damage"].toInt();
        int attackInterval = weaponJson["attackInterval"].toInt();
        int rangeSize = weaponJson["rangeSize"].toInt();
        bool isPlayerSide = false;
        weapon = new RemoteWeapon(bulletVelocity,
                                  bulletSize,
                                  damage,
                                  attackInterval,
                                  rangeSize,
                                  isPlayerSide,
                                  map);
    } else {
        int damage = weaponJson["damage"].toInt();
        int attackInterval = weaponJson["attackInterval"].toInt();
        int rangeSize = weaponJson["rangeSize"].toInt();
        bool isPlayerSide = false;
        weapon = new MeleeWeapon(damage, attackInterval, rangeSize, isPlayerSide, map);
    }

    QString name = basicJson["name"].toString();
    int width = basicJson["width"].toInt();
    int height = basicJson["height"].toInt();
    int maxHealth = basicJson["maxHealth"].toInt();
    double maxVelocity = basicJson["maxVelocity"].toDouble();
    double accelerationFactor = basicJson["accelerationFactor"].toDouble();
    double attackMoveDecayFactor = basicJson["attackMoveDecayFactor"].toDouble();
    double reboundFactor = basicJson["reboundFactor"].toDouble();
    int exp = basicJson["exp"].toInt();
    int attackWaitTime = basicJson["attackWaitTime"].toInt();

    Witch *witch = new Witch(name,
                             width,
                             height,
                             maxHealth,
                             maxVelocity,
                             accelerationFactor,
                             attackMoveDecayFactor,
                             reboundFactor,
                             exp,
                             attackWaitTime,
                             weapon,
                             map);
    return witch;
}

int Witch::getExp() {
    return exp;
};

bool Witch::getValidity() {
    return isValid;
}

int Witch::getAttackWaitTime() {
    return attackWaitTime;
}

void Witch::setValidity() {
    isValid = !isValid;
}

void Witch::performAttack(Character *player) {
    if (!weapon->isCooldownFinished() || isAttacking || !isValid) {
        return;
    }

    AttackRange *range = weapon->getRange();
    QPointF witchPos = this->getPos(), playerPos = player->getPos();
    if (!range->contains(witchPos, player->geometry())) {
        return;
    }

    double degree = MathUtils::calculateDegree(witchPos, playerPos);
    isAttacking = true;

    QTimer::singleShot(attackWaitTime, this, [this, degree]() {
        if (weapon->getType() == Weapon::WeaponType::Remote) {
            Bullet *bullet = (Bullet *) this->regularAttack(degree);
            emit attackPerformed(bullet);
        } else {
            Slash *slash = (Slash *) this->regularAttack(degree);
            emit attackPerformed(slash);
        }

        isAttacking = false;
    });
}

void Witch::moveActively(Direction dir) {
    // 防卡死
    if (dir == Direction::Center) {
        dir = prevDir;
    } else {
        prevDir = dir;
    }

    auto [moveX, moveY] = ~dir;
    updateAcceleration(moveX, moveY);
    updateVelocity();
    updatePosition();
}
