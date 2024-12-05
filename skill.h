#ifndef SKILL_H
#define SKILL_H

#include <QTimer>
#include <QWidget>

class SkillManager : public QWidget {
    Q_OBJECT

protected:
    int manaCost;
    int cooldownTime;

    bool isOnCooldown = false;
    QTimer cooldownTimer;

    QVector<double> effectParameters;

public:
    explicit SkillManager(int manaCost,
                          int cooldownTime,
                          QVector<double> &effectParameters,
                          QWidget *parent = nullptr);

    int getManaCost() const;
    bool getIsOnCooldown() const;
    QVector<double> &getEffectParameters();

    void setCooldown();
};

#endif // SKILL_H
