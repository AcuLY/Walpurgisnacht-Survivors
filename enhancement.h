#ifndef ENHANCEMENT_H
#define ENHANCEMENT_H

#include <QObject>
#include <QVector>

#include "magicalgirl.h"

class Enhancement : public QObject { // 单个强化词条类
    Q_OBJECT

private:
    QString type;               // 强化类型
    QString description;        // 描述
    QVector<double> parameters; // 参数

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

class EnhancementManager : public QObject { // 强化执行类
    Q_OBJECT

public:
    explicit EnhancementManager(MagicalGirl *player, QObject *parent);
    ~EnhancementManager();

    const QVector<Enhancement *> getGlobalEnhancements() const;

    QVector<Enhancement *> generateEnhancement(MagicalGirl *player); // 随机返回三个强化
    void applyEnhancement(Enhancement *e, int index);                // 执行强化

private:
    MagicalGirl *player;

    QVector<Enhancement *> enhancements;      // 存储所有局内强化词条
    QVector<Enhancement *> globalEhancements; // 全局强化

    void damageEnhancement(double value);      // 提高攻击力
    void attackSpeedEnhancement(double value); // 提高攻击速度
    void attackRangeEnhancement(double value); // 增加攻击范围
    void bulletVelocityEnhancement(double value); // 提高子弹速度
    void bulletSizeEnhancement(double value);  // 增加子弹大小
    void spanAngleEnhancement(double value);   // 增大斩击张角
    void multiAttackTimeEnhancement(int value); // 增加攻击次数
    void maxHealthEnhancement(double value);             // 提高最大生命值
    void healthRecoverIntervalEnhancement(double value); // 提高回血速度
    void healthRecoverRateEnhancement(double value);     // 增加每次回血量
    void healthRecoverManaCostEnhancement(double value); // 增加回血耗蓝
    void outAttackIntervalEnhancement(double value);     // 增加脱战速度
    void invincibilityIntervalEnhancement(double value); // 增加无敌时间长度
    void maxManaEnhancement(double value);               // 提高最大蓝量
    void manaRecoverEnhancement(double value);           // 增加回蓝速度
    void maxVelocityEnhancement(double value); // 提高最大移动速度
    void attackDecayEnhancement(double value); // 减少攻击衰减
    void pickupRangeEnhancement(double value);    // 增加拾取范围
    void experienceGainEnhancement(double value); // 增加经验获取
};

#endif // ENHANCEMENT_H
