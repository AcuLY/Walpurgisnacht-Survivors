#ifndef ENHANCEMENT_H
#define ENHANCEMENT_H

#include <QObject>
#include <QVector>

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

private:
    MagicalGirl *player;

    QVector<Enhancement *> normalEnhancements;

    void damageEnhancement(double enhancement);
    void healthEnhancement(double enhancement);

public:
    explicit EnhancementManager(MagicalGirl *player, QObject *parent);

    Enhancement *generateNormalEnhancement(int level);

    void applyEnhancement(Enhancement e);
};

#endif // ENHANCEMENT_H
