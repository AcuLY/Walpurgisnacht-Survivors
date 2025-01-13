#include "magicalgirl.h"

MagicalGirl::MagicalGirl(QString name,
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
    multiAttackTimer->setSingleShot(false);
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

double MagicalGirl::getRecoverRate() const {
    return recoverRate;
}

bool MagicalGirl::getIsReadyToRecover() const {
    return !isInAttack && !isOnRecoverCooldown;
}

CircleRange *MagicalGirl::getPickRange() const {
    return pickRange;
}

void MagicalGirl::performSingleAttack(double targetDegree) {
    double degree = targetDegree == INF ? facingDegree : targetDegree;

    if (Character::weapon->getType() == Weapon::WeaponType::Remote) {
        Bullet *bullet = (Bullet *) this->regularAttack(degree);
        emit attackPerformed(bullet);
    } else {
        Slash *slash = (Slash *) this->regularAttack(degree);
        emit attackPerformed(slash);
    }
}

void MagicalGirl::performAttack(QVector<double> targetWitchDegrees) {
    qDebug() << Character::weapon->isCooldownFinished() << isAttacking;
    if (!Character::weapon->isCooldownFinished() || isAttacking) {
        return;
    }

    isAttacking = true;

    targetDegrees = targetWitchDegrees;
    targetDegreesIt = targetDegrees.begin();
    attackTimeLeft = multiAttackTime;

    auto singleAttack = [&] {
        if (!attackTimeLeft) {
            Character::weapon->setMultiAttackMode(false);
            isAttacking = false;
            multiAttackTimer->stop();
            return;
        }

        if (!targetDegrees.empty()) {
            ++targetDegreesIt;

            if (targetDegreesIt == targetDegrees.end()) {
                targetDegreesIt = targetDegrees.begin();
            }

            currentTargetDegree = *targetDegreesIt;
        }

        if (attackTimeLeft < multiAttackTime) {
            Character::weapon->setMultiAttackMode(true);
        }

        performSingleAttack(currentTargetDegree);
        attackTimeLeft -= 1;
    };

    connect(multiAttackTimer, &QTimer::timeout, this, singleAttack);

    singleAttack();
    multiAttackTimer->start();
}

void MagicalGirl::receiveDamage(double damage) {
    if (isInvincible) {
        return;
    } else {
        isInvincible = true;
        invincibleTimer->start();
    }

    if (currentHealth == 0) {
        currentMana -= damage;
        qDebug() << "receive mana damage: " << damage << ", mana left: " << currentMana;
    } else if (currentHealth < damage) {
        currentHealth = 0;
        qDebug() << "receive damage: " << damage << ", no health left";
    } else {
        currentHealth -= damage;
        qDebug() << "receive damage: " << damage << ", health left: " << currentHealth;
    }

    isInAttack = true;
    inAttackTimer->start();
}

void MagicalGirl::recoverHealth() {
    if (currentHealth == maxHealth) {
        return;
    }

    currentHealth += maxHealth * recoverRate;
    currentHealth = std::min(currentHealth, maxHealth);

    currentMana -= recoverManaCost;

    qDebug() << "recover health, health left:" << currentHealth;

    isOnRecoverCooldown = true;
    recoverTimer->start();
}

void MagicalGirl::recoverMana(int mana) {
    currentMana += mana;
}
