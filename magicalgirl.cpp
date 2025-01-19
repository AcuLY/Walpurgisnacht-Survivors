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

MagicalGirl::~MagicalGirl() {
    delete inAttackTimer;
    delete invincibleTimer;
    delete recoverTimer;
    delete multiAttackTimer;
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
        int spanAngle = weaponJson["spanAngle"].toInt();
        bool isPlayerSide = true;
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

void MagicalGirl::render(QPainter *painter) {
    // 如果处于无敌状态就用红色表示
    if (isInvincible) {
        painter->drawPixmap(this->x() + width / 2 - textureHurt.width() / 2,
                            this->y() + height / 2 - textureHurt.height() / 2,
                            textureHurt);
        return;
    }

    painter->drawPixmap(this->x() + width / 2 - texture.width() / 2,
                        this->y() + height / 2 - texture.height() / 2,
                        texture);
}

int MagicalGirl::getCurrentMana() const {
    return currentMana;
}

int MagicalGirl::getMaxMana() const {
    return maxMana;
}

bool MagicalGirl::getIsReadyToRecover() const {
    return !isInAttack && !isOnRecoverCooldown; // 脱战且回血冷却结束才能回血
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
    double degree = targetDegree == INF ? facingDegree
                                        : targetDegree; // 如果没有目标则向移动方向攻击

    emit attackPerformed(generateAttack(degree));
}

void MagicalGirl::multiAttack() {
    // 连击次数用完，结束
    if (!attackTimeLeft) {
        weapon->setMultiAttackMode(false);
        isAttacking = false;
        return;
    }

    // 如果有能攻击到的目标，就遍历目标的角度
    if (!targetDegrees.empty()) {
        ++targetDegreesIt;

        if (targetDegreesIt == targetDegrees.end()) {
            targetDegreesIt = targetDegrees.begin();
        }

        currentTargetDegree = *targetDegreesIt;
    } else {
        currentTargetDegree = INF;
    }

    // 如果目标列表更新了需要重置当前目标
    if (targetDegreeUpdated && !targetDegrees.empty()) {
        targetDegreesIt = targetDegrees.begin();
        currentTargetDegree = *targetDegreesIt;

        targetDegreeUpdated = false;
    }

    // 武器进入连击模式
    if (attackTimeLeft < multiAttackTime) {
        weapon->setMultiAttackMode(true);
    }

    // 触发第一次连击
    performSingleAttack(currentTargetDegree);
    attackTimeLeft -= 1;
    multiAttackTimer->start();
}

void MagicalGirl::performAttack(QVector<double> targetWitchDegrees) {
    // 检查武器冷却以及是否还在攻击中
    if (!weapon->isCooldownFinished() || isAttacking) {
        return;
    }

    isAttacking = true;

    // 更新连击所需信息
    targetDegrees = targetWitchDegrees;
    targetDegreeUpdated = true;
    targetDegreesIt = targetDegrees.begin();
    attackTimeLeft = multiAttackTime;

    multiAttack();
}

void MagicalGirl::receiveDamage(double damage) {
    // 检查是否仍在无敌状态，否则进入受击无敌状态
    if (isInvincible) {
        return;
    }

    isInvincible = true;
    invincibleTimer->start();

    isReceivingDamage = true;
    receiveDamageTimer->start();

    isInAttack = true;
    inAttackTimer->start();

    // 如果没血了就扣蓝
    if (currentHealth == 0) {
        currentMana -= damage;
    } else if (currentHealth < damage) {
        currentHealth = 0;
    } else {
        currentHealth -= damage;
    }

    emit damageReceived();
}

void MagicalGirl::recoverHealth() {
    // 如果满血或者蓝太少就不回血
    if (currentHealth == maxHealth || currentMana < MIN_RECOVER_HEALTH_MANA) {
        return;
    }

    currentHealth += maxHealth * recoverRate;
    currentHealth = std::min(currentHealth, maxHealth); // 不能超过最大血量

    currentMana -= recoverManaCost;

    isOnRecoverCooldown = true;
    recoverTimer->start();

    emit healed();
}

void MagicalGirl::recoverMana(int mana) {
    currentMana += mana + manaRecoverBonus;
    currentMana = qMin(currentMana, maxMana); // 不能超过最大蓝量
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
    recoverRate = qMin(recoverRate, 1.0);
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
