#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QAudioOutput>
#include <QMap>
#include <QMediaPlayer>
#include <QObject>
#include <QUrl>

#include "global.h"

const int MAX_CONCURRENT_SFX_PLAYBACKS = 10; // 一个音效的最多同时播放数

class SoundManager : public QObject { // 音频控制类
    Q_OBJECT

public:
    explicit SoundManager(Global *global, QObject *parent = nullptr);
    ~SoundManager();

    void playSfx(const QString &sfxName); // 播放音效
    void playMenuMusic();                 // 播放菜单音乐
    void playGameMusic();                 // 播放游戏内音乐
    void pauseBackgroundMusic();          // 暂停音乐
    void stopBackgroundMusic();           // 终止音乐

private:
    Global *global;

    QMap<QString, QString> soundEffects; // 存储音效名称和文件路径的映射
    QMap<QString, int> currentPlayingNum; // 记录某个音效当前正在播放的数量
    QList<QMediaPlayer *> mediaPlayers;  // 用于存储多个 QMediaPlayer 实例

    QMediaPlayer *menuPlayer; // 菜单音乐播放器
    QMediaPlayer *gamePlayer; // 游戏音乐播放器

    void playSound(const QString &soundName); // 播放一个声音
};

#endif // SOUNDMANAGER_H
