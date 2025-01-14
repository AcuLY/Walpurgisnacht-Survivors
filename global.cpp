#include "global.h"

Global::Global(QObject *parent) : QObject{parent} {
    QJsonObject jsonFile = FileUtils::loadJsonFile(saveFilePath)[0].toObject();

    QJsonObject settings = jsonFile.value("settings").toObject();
    backgroundMusic = settings.value("backgroundMusic").toBool();
    soundEffect = settings.value("soundEffect").toBool();
    voiceEffect = settings.value("voiceEffect").toBool();

    QJsonArray keyArray = settings.value("keyboardMapping").toArray();
    for (int i = 0; i < keyArray.size(); ++i) {
        keyboardMapping[static_cast<GameKey>(i)] = keyArray[i].toInt();
    }

    money = jsonFile.value("money").toInt();

    QJsonArray enhancementArray = jsonFile.value("globalEnhancementLevels").toArray();
    for (int i = 0; i < enhancementArray.size(); ++i) {
        globalEnhancementLevels.append(enhancementArray[i].toInt());
    }
}

bool Global::isBackgroundMusicOn() const {
    return backgroundMusic;
}

bool Global::isSoundEffectOn() const {
    return soundEffect;
}

bool Global::isVoiceEffectOn() const {
    return voiceEffect;
}

void Global::setBackgroundMusic() {
    backgroundMusic = !backgroundMusic;
}

void Global::setSoundEffect() {
    soundEffect = !soundEffect;
}

void Global::setVoiceEffect() {
    voiceEffect = !voiceEffect;
}

const QMap<GameKey, int> &Global::getKeyboardMapping() const {
    return keyboardMapping;
}

void Global::setKeyboardMapping(GameKey key, int value) {
    keyboardMapping[key] = value;
}
