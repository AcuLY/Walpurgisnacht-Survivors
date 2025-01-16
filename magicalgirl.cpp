#include "magicalgirl.h"

MagicalGirl::MagicalGirl(QString name,
                         QString texturePath,
                         QString texturePathHurt,
                         int width,
                         int height,
                         double maxHealth,
                         double maxVelocity,
                         double accelerationFactor,
                         double attackMoveDecayFactor,
                         double reboundFactor,
                         int maxMana,
                         double recoverRate,
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
      maxMana(maxMana), recoverRate(recoverRate) {
    currentMana = maxMana;
    // 脱战回血计时器
    inAttackTimer = new QTimer(this);
    inAttackTimer->setInterval(outAttackInterval);
    inAttackTimer->setSingleShot(true);
    connect(inAttackTimer, &QTimer::timeout, this, [this] { isInAttack = false; });
    // 受击后无敌帧计时器
    invincibleTimer = new QTimer(this);
    invincibleTimer->setInterval(invincibleFrameInterval);
    invincibleTimer->setSingleShot(true);
    connect(invincibleTimer, &QTimer::timeout, this, [this] { isInvincible = false; });
    // 回血间隔计算器
    recoverTimer = new QTimer(this);
    recoverTimer->setInterval(recoverInterval);
    recoverTimer->setSingleShot(true);
    connect(recoverTimer, &QTimer::timeout, this, [this] { isOnRecoverCooldown = false; });
    // 连击间隔计算器
    multiAttackTimer = new QTimer(this);
    multiAttackTimer->setInterval(multiAttackInterval);
    multiAttackTimer->setSingleShot(true);
    connect(multiAttackTimer, &QTimer::timeout, this, &MagicalGirl::multiAttack);
}

MagicalGirl *MagicalGirl::loadMagicalGirlFromJson(MagicalGirlEnum playerSelection, Map *map) {
    QJsonArray basicJsons = FileUtils::loadJsonFile(
        ":/data/magical_girls/magical_girls_basic_jsons");
    QJsonArray weaponJsons = FileUtils::loadJsonFile(
        ":/data/magical_girls/magical_girls_weapons_jsons");

    int index = static_cast<int>(playerSelection);
    QJsonObject basicJson = basicJsons[index].toObject();
    QJsonObject weaponJson = weaponJsons[index].toObject();

    Weapon *weapon;
    if (weaponJson["isRemoteWeapon"].toBool()) {
        int bulletVelocity = weaponJson["bulletVelocity"].toInt();
        int bulletSize = weaponJson["bulletSize"].toInt();
        int damage = weaponJson["damage"].toInt();
        int attackInterval = weaponJson["attackInterval"].toInt();
        int rangeSize = weaponJson["rangeSize"].toInt();
        bool isPlayerSide = true;
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
        bool isPlayerSide = true;
        weapon = new MeleeWeapon(damage, attackInterval, rangeSize, isPlayerSide, map);
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
    int maxMana = basicJson["maxMana"].toInt();
    double recoverRate = basicJson["recoverRate"].toDouble();

    MagicalGirl *player = new MagicalGirl(name,
                                          texturePath,
                                          texturePathHurt,
                                          width,
                                          height,
                                          maxHealth,
                                          maxVelocity,
                                          accelerationFactor,
                                          attackMoveDecayFactor,
                                          reboundFactor,
                                          maxMana,
                                          recoverRate,
                                          weapon,
                                          map);
    return player;
}

int MagicalGirl::getCurrentMana() const {
    return currentMana;
}

int MagicalGirl::getMaxMana() const {
    return maxMana;
}

double MagicalGirl::getRecoverRate() const {
    return recoverRate;
}

bool MagicalGirl::getIsReadyToRecover() const {
    return !isInAttack && !isOnRecoverCooldown;
}

int MagicalGirl::getExperienceBonus() const {
    return experienceBonus;
}

CircleRange *MagicalGirl::getPickRange() const {
    return pickRange;
}

void MagicalGirl::initHealthAndMana() {
    currentHealth = maxHealth;
    currentMana = maxMana;
}

void MagicalGirl::performSingleAttack(double targetDegree) {
    double degree = targetDegree == INF ? facingDegree : targetDegree;

    if (weapon->getType() == Weapon::WeaponType::Remote) {
        Bullet *bullet = (Bullet *) this->regularAttack(degree);
        emit attackPerformed(bullet);
    } else {
        Slash *slash = (Slash *) this->regularAttack(degree);
        emit attackPerformed(slash);
    }
}

void MagicalGirl::multiAttack() {
    if (!attackTimeLeft) {
        weapon->setMultiAttackMode(false);
        isAttacking = false;
        return;
    }

    if (!targetDegrees.empty()) {
        ++targetDegreesIt;

        if (targetDegreesIt == targetDegrees.end()) {
            targetDegreesIt = targetDegrees.begin();
        }

        currentTargetDegree = *targetDegreesIt;
    }

    if (targetDegreeUpdated && !targetDegrees.empty()) {
        targetDegreesIt = targetDegrees.begin();
        currentTargetDegree = *targetDegreesIt;

        targetDegreeUpdated = false;
    }

    if (attackTimeLeft < multiAttackTime) {
        weapon->setMultiAttackMode(true);
    }

    performSingleAttack(currentTargetDegree);
    attackTimeLeft -= 1;
    multiAttackTimer->start();
}

void MagicalGirl::performAttack(QVector<double> targetWitchDegrees) {
    if (!weapon->isCooldownFinished() || isAttacking) {
        return;
    }

    isAttacking = true;

    targetDegrees = targetWitchDegrees;
    targetDegreeUpdated = true;
    targetDegreesIt = targetDegrees.begin();
    attackTimeLeft = multiAttackTime;

    multiAttack();
}

void MagicalGirl::receiveDamage(double damage) {
    if (isInvincible) {
        return;
    }

    isInvincible = true;
    invincibleTimer->start();

    isReceivingDamage = true;
    receiveDamageReceiveTimer->start();

    if (currentHealth == 0) {
        currentMana -= damage;
    } else if (currentHealth < damage) {
        currentHealth = 0;
    } else {
        currentHealth -= damage;
    }

    isInAttack = true;
    inAttackTimer->start();

    emit damageReceived();
}

void MagicalGirl::recoverHealth() {
    if (currentHealth == maxHealth) {
        return;
    }

    currentHealth += maxHealth * recoverRate;
    currentHealth = std::min(currentHealth, maxHealth);

    currentMana -= recoverManaCost;

    isOnRecoverCooldown = true;
    recoverTimer->start();

    emit healed();
}

void MagicalGirl::recoverMana(int mana) {
    currentMana += mana + manaRecoverBonus;
    currentMana = qMin(currentMana, maxMana);
}

void MagicalGirl::increaseAttackSpeed(double value) {
    weapon->decreaseAttackInterval(value);

    multiAttackInterval *= value;
    multiAttackTimer->setInterval(multiAttackInterval);
}

void MagicalGirl::increaseMultiAttackTime(int value) {
    multiAttackTime += value;
}

void MagicalGirl::increaseMaxHealth(int value) {
    maxHealth += value;
}

void MagicalGirl::decreaseRecoverInterval(double value) {
    recoverInterval *= value;
    recoverTimer->setInterval(recoverInterval);
}

void MagicalGirl::increaseRecoverRate(double value) {
    recoverRate += value;
}

void MagicalGirl::decreaseRecoverManaCost(int value) {
    recoverManaCost -= value;
    recoverManaCost = qMax(recoverManaCost, 1);
}

void MagicalGirl::decreaseOutAttackInterval(double value) {
    outAttackInterval *= value;
    inAttackTimer->setInterval(outAttackInterval);
}

void MagicalGirl::increaseInvincibleInterval(double value) {
    invincibleFrameInterval *= value;
    invincibleTimer->setInterval(invincibleFrameInterval);
}

void MagicalGirl::increaseManaRecoverBonus(int value) {
    manaRecoverBonus += value;
}

void MagicalGirl::increaseMaxMana(int value) {
    maxMana += value;
}

void MagicalGirl::increaseMaxVelocity(int value) {
    maxVelocity += value;
}

void MagicalGirl::decreaseAttackDecay(double value) {
    attackMoveDecayFactor += value;
    attackMoveDecayFactor = fmin(1, attackMoveDecayFactor);
}

void MagicalGirl::increasePickRange(double value) {
    pickRangeSize *= value;
    delete pickRange;
    pickRange = new CircleRange(pickRangeSize);
}

void MagicalGirl::increaseExperienceBonus(int value) {
    experienceBonus += value;
}
