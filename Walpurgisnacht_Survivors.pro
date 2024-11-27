QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    attack.cpp \
    attackrange.cpp \
    character.cpp \
    direction.cpp \
    gamelogic.cpp \
    gamewindow.cpp \
    magicalgirl.cpp \
    main.cpp \
    mainwindow.cpp \
    map.cpp \
    third_party/perlinnoise.cpp \
    utils.cpp \
    weapon.cpp \
    witch.cpp

HEADERS += \
    attack.h \
    attackrange.h \
    character.h \
    direction.h \
    gamelogic.h \
    gamewindow.h \
    magicalgirl.h \
    mainwindow.h \
    map.h \
    third_party/perlinnoise.h \
    utils.h \
    weapon.h \
    witch.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
