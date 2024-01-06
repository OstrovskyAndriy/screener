QT       += core gui sql concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Це конфігурація запуску утиліти windeployqt, яка при релізі програми збере усі необхідні dll
# If release-buid -> run windeploy applications, that will collect all the dlls
CONFIG(release, debug|release) {
    QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt $$OUT_PWD/release
}

# Ця конфігурація розносить вихідні файли проекту при компіляції у різні підкаталоги: moc, rcc, ui, win32.
# Share all project output files by directories
MOC_DIR = moc
RCC_DIR = rcc
UI_DIR = ui
unix:OBJECTS_DIR = unix
win32:OBJECTS_DIR = win32
macx:OBJECTS_DIR = mac

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dbmanager.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    dbmanager.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    recources.qrc

