#include "witch.h"

Witch::Witch(QString name,
             QString texturePath,
             QString texturePathHurt,
             int width,
             int height,
             double maxHealth,
             double maxVelocity,
             double accelerationFactor,
             double attackMoveDecayFactor,
             double reboundFactor,
             int exp,
             int attackWaitTime,
             bool isPlayerDirected,
             Weapon *weapon,
             QWidget *parent)
    : Character(name,
                texturePath,
                texturePathHurt,
                width,
                height,
                maxHealth,
                maxVelocity,
                accelerationFactor,
                attackMoveDecayFactor,
                reboundFactor,
                weapon,
                parent),
      exp(exp), attackWaitTime(attackWaitTime), isPlayerDirected(isPlayerDirected) {
}

WitchEnum Witch::chooseWitch(double progress) {
    double spawnRand = QRandomGenerator::global()->bounded(1.0);
    double chooseIndexRand = QRandomGenerator::global()->bounded(1.0);

    double spawnProbability = 1 / (1 + std::exp(-2 * (progress - 2.5))) + 0.002;

    if (progress < spawnProbLow) {
        // 进度小于20%，最多只能生成怪物witch1
        if (spawnRand < spawnProbability) {
            return WitchEnum::witch1;
        }
    } else if (progress < spawnProbMidLow) {
        // 进度在20%到50%之间，最多生成怪物witch1和witch2
        if (spawnRand < spawnProbability) {
            return chooseIndexRand < thresholdWeakMid ? WitchEnum::witch1 : WitchEnum::witch2;
        }
    } else if (progress < spawnProbMidHigh) {
        // 进度在50%到80%之间，最多生成怪物witch1、witch2和witch3
        if (spawnRand < spawnProbability) {
            if (chooseIndexRand < weakMonsterProb) {
                return WitchEnum::witch1;
            } else if (chooseIndexRand < midMonsterProb) {
                return WitchEnum::witch2;
            } else {
                return WitchEnum::witch3;
            }
        }
    } else {
        // 进度大于80%，可以生成所有种类的怪物
        if (spawnRand < spawnProbability) {
            if (chooseIndexRand < weakMonsterProb) {
                return WitchEnum::witch1;
            } else if (chooseIndexRand < midMonsterProb) {
                return WitchEnum::witch2;
            } else if (chooseIndexRand < strongMonsterProb) {
                return WitchEnum::witch3;
            } else {
                return WitchEnum::witch4;
            }
        }
    }

    // 如果没有生成怪物，返回 noWitch
    return WitchEnum::noWitch;
}

bool Witch::ifDropGriefSeedFragment() {
    double rand = QRandomGenerator::global()->bounded(1.0);

    return rand < griefSeedFragmentPossibility;
}

Witch *Witch::loadWitchFromJson(WitchEnum witchIndex, Map *map) {
    QJsonArray basicJsons = FileUtils::loadJsonFile(":/data/witch/witches_basic_jsons");
    QJsonArray weaponJsons = FileUtils::loadJsonFile(":/data/witch/witches_weapons_jsons");

    int index = static_cast<int>(witchIndex);
    QJsonObject basicJson = basicJsons[index].toObject();
    QJsonObject weaponJson = weaponJsons[index].toObject();

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
        int spanAngle = weaponJson["spanAngle"].toInt();
        bool isPlayerSide = false;
        weapon = new MeleeWeapon(damage, attackInterval, rangeSize, spanAngle, isPlayerSide, map);
    }

    QString name = basicJson["name"].toString();
    QString texturePath = basicJson["texturePath"].toString();
    QString texturePathHurt = basicJson["texturePathHurt"].toString();
    int width = basicJson["width"].toInt();
    int height = basicJson["height"].toInt();
    int maxHealth = basicJson["maxHealth"].toInt();
    double maxVelocity = basicJson["maxVelocity"].toDouble();
    double accelerationFactor = basicJson["accelerationFactor"].toDouble();
    double attackMoveDecayFactor = basicJson["attackMoveDecayFactor"].toDouble();
    double reboundFactor = basicJson["reboundFactor"].toDouble();
    int exp = basicJson["exp"].toInt();
    int attackWaitTime = basicJson["attackWaitTime"].toInt();
    bool isPlayerDirected = basicJson["isPlayerDirected"].toBool();

    Witch *witch = new Witch(name,
                             texturePath,
                             texturePathHurt,
                             width,
                             height,
                             maxHealth,
                             maxVelocity,
                             accelerationFactor,
                             attackMoveDecayFactor,
                             reboundFactor,
                             exp,
                             attackWaitTime,
                             isPlayerDirected,
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
    if (isPlayerDirected || prevDir == Direction::Center) {
        auto [moveX, moveY] = ~dir;
        updateAcceleration(moveX, moveY);
        prevDir = dir;
    } else {
        if (isBlocked) {
            BiDirection moveX = BiDirection::Neutral, moveY = BiDirection::Neutral;
            while (moveX != BiDirection::Neutral || moveY != BiDirection::Neutral) {
                int randX = QRandomGenerator::global()->bounded(3),
                    randY = QRandomGenerator::global()->bounded(3);
                moveX = static_cast<BiDirection>(randX - 1);
                moveY = static_cast<BiDirection>(randY - 1);
            }

            isBlocked = false;
            updateAcceleration(moveX, moveY);
            prevDir = pairBiDirection(moveX, moveY);
        } else {
            auto [moveX, moveY] = ~prevDir;
            updateAcceleration(moveX, moveY);
        }
    }

    updateVelocity();
    updatePosition();
}

void Witch::blocked() {
    isBlocked = true;
}
