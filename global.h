#ifndef GLOBAL_H
#define GLOBAL_H

#include <QMap>
#include <QObject>

#include "utils.h"

const QString saveFilePath = ":/save/save_json";

enum GameKey { up, down, left, right, attack, dodge, skill };

class Global : public QObject {
    Q_OBJECT
public:
    explicit Global(QObject *parent = nullptr);

    bool isBackgroundMusicOn() const;
    bool isSoundEffectOn() const;
    bool isVoiceEffectOn() const;

    void setBackgroundMusic();
    void setSoundEffect();
    void setVoiceEffect();

    const QMap<GameKey, int> &getKeyboardMapping() const;
    void setKeyboardMapping(GameKey key, int value);

private:
    bool backgroundMusic = true;
    bool soundEffect = true;
    bool voiceEffect = true;

    QMap<GameKey, int> keyboardMapping;

    int money = 0;
    QVector<int> globalEnhancementLevels;

signals:
};

#endif // GLOBAL_H
