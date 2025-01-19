#ifndef GLOBAL_H
#define GLOBAL_H

#include <QMap>
#include <QObject>

const QString saveFilePath = "save.json";

enum GameKey { up, down, left, right, attack, dodge, skill };

class Global : public QObject { // 全局设置、强化管理类
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

    int getGlobalEnhancementLevel(int index) const;
    void addGlobalEnhancementLevel(int index);

    int getMoney() const;
    void addMoney(int amount);
    void useMoney(int amount);

private:
    bool backgroundMusic = true; // bgm
    bool soundEffect = true;     // 音效
    bool voiceEffect = true;     // 语音，没做

    QMap<GameKey, int> keyboardMapping; // 键位映射

    int money = 0;                        // 金币
    QVector<int> globalEnhancementLevels; // 全局强化的等级

signals:
};

#endif // GLOBAL_H
