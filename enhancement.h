#ifndef ENHANCEMENT_H
#define ENHANCEMENT_H

#include <QObject>
#include <QVector>

#include "character.h"
#include "magicalgirl.h"
#include "utils.h"

class Enhancement : public QObject {
    Q_OBJECT

private:
    QString type;
    QString description;
    QVector<double> parameters;

    friend class EnhancementManager;

public:
    explicit Enhancement(QString type,
                         QString description,
                         QVector<double> &parameters,
                         QObject *parent = nullptr);
    explicit Enhancement(const QJsonObject &json, QObject *parent = nullptr);

    QString getType();
    QString getDescription();
};

class EnhancementManager : public QObject {
    Q_OBJECT

public:
    explicit EnhancementManager(MagicalGirl *player, QObject *parent);

    QVector<Enhancement *> generateEnhancement(MagicalGirl *player);

    const QVector<Enhancement *> getGlobalEnhancements() const;

    void applyEnhancement(Enhancement *e, int index);

private:
    MagicalGirl *player;

    QVector<Enhancement *> enhancements;
    QVector<Enhancement *> globalEhancements;

    // 攻击类
    void damageEnhancement(double value);      // 提高攻击力
    void attackSpeedEnhancement(double value); // 提高攻击速度
    void attackRangeEnhancement(double value); // 增加攻击范围
    void bulletVelocityEnhancement(double value); // 提高子弹速度
    void bulletSizeEnhancement(double value);  // 增加子弹大小
    void multiAttackTimeEnhancement(int value); // 增加攻击次数

    // 生存类
    void maxHealthEnhancement(double value);             // 提高最大生命值
    void healthRecoverIntervalEnhancement(double value); // 提高回血速度
    void healthRecoverRateEnhancement(double value);     // 增加每次回血量
    void healthRecoverManaCostEnhancement(double value); // 增加回血耗蓝
    void outAttackIntervalEnhancement(double value);     // 增加脱战速度
    void invincibilityIntervalEnhancement(double value); // 增加无敌时间长度
    void maxManaEnhancement(double value);               // 提高最大蓝量
    void manaRecoverEnhancement(double value);           // 增加回蓝速度

    // 移动类
    void maxVelocityEnhancement(double value); // 提高最大移动速度
    void attackDecayEnhancement(double value); // 减少攻击衰减

    // 升级类
    void pickupRangeEnhancement(double value);    // 增加拾取范围
    void experienceGainEnhancement(double value); // 增加经验获取
};

#endif // ENHANCEMENT_H
