#include "global.h"

Global::Global(QObject *parent) : QObject{parent} {
    // 如果没有 save 文件则创建一个
    if (!QFile::exists(saveFilePath)) {
        QJsonObject settings;
        settings["backgroundMusic"] = true;
        settings["soundEffect"] = true;
        settings["voiceEffect"] = true;

        QJsonArray keyboardMapping;
        keyboardMapping.append(87); // W
        keyboardMapping.append(83); // S
        keyboardMapping.append(65); // A
        keyboardMapping.append(68); // D
        keyboardMapping.append(32); // Space
        keyboardMapping.append(74); // J
        keyboardMapping.append(75); // K
        settings["keyboardMapping"] = keyboardMapping;

        QJsonArray globalEnhancementLevels;
        globalEnhancementLevels.append(0);
        globalEnhancementLevels.append(0);
        globalEnhancementLevels.append(0);
        globalEnhancementLevels.append(0);
        globalEnhancementLevels.append(0);
        globalEnhancementLevels.append(0);

        QJsonObject rootObject;
        rootObject["settings"] = settings;
        rootObject["money"] = 0;
        rootObject["globalEnhancementLevels"] = globalEnhancementLevels;

        QJsonArray outerArray;
        outerArray.append(rootObject);

        QJsonDocument doc(outerArray);

        QFile file(saveFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            file.write(doc.toJson());
            file.close();
        } else {
            qWarning() << "Failed to open file for writing:" << file.errorString();
        }
    }

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

void Global::addGlobalEnhancementLevel(int index) {
    globalEnhancementLevels[index] += 1;
}

int Global::getMoney() const {
    return money;
}

void Global::addMoney(int amount) {
    money += amount;
}

void Global::useMoney(int amount) {
    money -= amount;
}

int Global::getGlobalEnhancementLevel(int index) const {
    return globalEnhancementLevels[index];
}
