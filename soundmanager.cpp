#include "soundmanager.h"
#include <QAudioFormat>
#include <QDebug>
#include "qmediaplayer.h"

SoundManager::SoundManager(Global *global, QObject *parent)
    : QObject(parent), global(global), menuPlayer(new QMediaPlayer(this)),
      gamePlayer(new QMediaPlayer(this)) {
    // 初始化音效文件路径
    soundEffects["experience"] = "qrc:/sounds/sfx/experience";
    soundEffects["griefseed"] = "qrc:/sounds/sfx/griefseed";
    soundEffects["health"] = "qrc:/sounds/sfx/health";
    soundEffects["hit"] = "qrc:/sounds/sfx/hit";
    soundEffects["levelup"] = "qrc:/sounds/sfx/levelup";
    soundEffects["select"] = "qrc:/sounds/sfx/select";
    soundEffects["shoot"] = "qrc:/sounds/sfx/shoot";
    soundEffects["enemy_shoot"] = "qrc:/sounds/sfx/enemy_shoot";
    soundEffects["slash"] = "qrc:/sounds/sfx/slash";
    soundEffects["hurt"] = "qrc:/sounds/sfx/hurt";

    // 创建音频输出设备
    QAudioOutput *menuAudioOutput = new QAudioOutput(this);
    QAudioOutput *gameAudioOutput = new QAudioOutput(this);

    // 设置音频输出设备
    menuPlayer->setAudioOutput(menuAudioOutput);
    gamePlayer->setAudioOutput(gameAudioOutput);

    // 默认背景音乐为菜单音乐
    menuPlayer->setSource(QUrl("qrc:/sounds/bgm/menu"));
    menuPlayer->setLoops(-1);
    gamePlayer->setSource(QUrl("qrc:/sounds/bgm/game"));
    gamePlayer->setLoops(-1);
}

SoundManager::~SoundManager() {
    for (auto *player : mediaPlayers) {
        player->deleteLater();
    }
}

void SoundManager::playSound(const QString &soundName) {
    if (currentPlayingNum[soundName] > MAX_CONCURRENT_SFX_PLAYBACKS) {
        return;
    }

    if (soundEffects.contains(soundName)) {
        // 创建新的实例
        QMediaPlayer *player = new QMediaPlayer(this);
        QAudioOutput *audioOutput = new QAudioOutput(this);
        player->setAudioOutput(audioOutput); // 为当前播放器设置独立的音频输出

        QUrl soundUrl = QUrl(soundEffects[soundName]);
        if (soundUrl.isValid()) {
            player->setSource(soundUrl);
            player->play();

            currentPlayingNum[soundName]++;

            // 连接播放完成后清理播放器
            connect(player,
                    &QMediaPlayer::playbackStateChanged,
                    this,
                    [player, audioOutput, this, soundName](QMediaPlayer::PlaybackState state) {
                        if (state == QMediaPlayer::StoppedState) {
                            player->deleteLater();
                            audioOutput->deleteLater();

                            currentPlayingNum[soundName]--;
                        }
                    });
        }
    } else {
        qDebug() << "Sound not found:" << soundName;
    }
}

void SoundManager::playSfx(const QString &sfxName) {
    if (!global->isSoundEffectOn()) {
        return;
    }

    playSound(sfxName);
}

void SoundManager::playMenuMusic() {
    if (!global->isBackgroundMusicOn()) {
        return;
    }

    if (menuPlayer->playbackState() != QMediaPlayer::PlayingState) {
        menuPlayer->play();
        gamePlayer->stop();
    }
}

void SoundManager::playGameMusic() {
    if (!global->isBackgroundMusicOn()) {
        return;
    }

    if (gamePlayer->playbackState() != QMediaPlayer::PlayingState) {
        gamePlayer->play();
        menuPlayer->stop();
    } else {
        gamePlayer->stop();
        menuPlayer->stop();
        gamePlayer->play();
    }
}

void SoundManager::pauseBackgroundMusic() {
    if (menuPlayer->playbackState() == QMediaPlayer::PlayingState) {
        menuPlayer->pause();
    } else if (gamePlayer->playbackState() == QMediaPlayer::PlayingState) {
        gamePlayer->pause();
    }
}

void SoundManager::stopBackgroundMusic() {
    if (menuPlayer->playbackState() == QMediaPlayer::PlayingState) {
        menuPlayer->stop();
    } else if (gamePlayer->playbackState() == QMediaPlayer::PlayingState) {
        gamePlayer->stop();
    }
}
