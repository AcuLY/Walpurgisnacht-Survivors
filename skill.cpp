#include "skill.h"

SkillManager::SkillManager(int manaCost,
                           int cooldownTime,
                           QVector<double> &effectParameters,
                           QWidget *parent)
    : QWidget(parent), manaCost(manaCost), cooldownTime(cooldownTime),
      effectParameters(effectParameters) {
    cooldownTimer.setInterval(cooldownTime);
    cooldownTimer.setSingleShot(true);
    connect(&cooldownTimer, &QTimer::timeout, this, [this]() { this->isOnCooldown = false; });
}

int SkillManager::getManaCost() const {
    return manaCost;
}

bool SkillManager::getIsOnCooldown() const {
    return isOnCooldown;
}

QVector<double> &SkillManager::getEffectParameters() {
    return effectParameters;
}

void SkillManager::setCooldown() {
    isOnCooldown = true;
    cooldownTimer.start();
}
