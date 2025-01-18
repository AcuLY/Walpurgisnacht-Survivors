#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QAudioOutput>
#include <QMap>
#include <QMediaPlayer>
#include <QObject>
#include <QUrl>

#include "global.h"

const int MAX_CONCURRENT_SFX_PLAYBACKS = 10; // 一个音效的最多同时播放数

class SoundManager : public QObject {
    Q_OBJECT

public:
    explicit SoundManager(Global *global, QObject *parent = nullptr);
    ~SoundManager();

    void playSfx(const QString &sfxName);
    void playMenuMusic();
    void playGameMusic();
    void pauseBackgroundMusic();
    void stopBackgroundMusic();

private:
    Global *global;

    QMap<QString, QString> soundEffects; // 存储音效名称和文件路径的映射
    QMap<QString, int> currentPlayingNum; // 记录某个音效当前正在播放的数量
    QList<QMediaPlayer *> mediaPlayers;  // 用于存储多个 QMediaPlayer 实例

    QMediaPlayer *menuPlayer; // 菜单背景音乐播放器
    QMediaPlayer *gamePlayer; // 游戏背景音乐播放器

    void playSound(const QString &soundName);
};

#endif // SOUNDMANAGER_H
