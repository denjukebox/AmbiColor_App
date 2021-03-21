QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Capture/buffermanager.cpp \
    Capture/averagecolor.cpp \
    Capture/divisionthreader.cpp \
    Capture/framedivider.cpp \
    Capture/framegrabber.cpp \
    Capture/framewrapper.cpp \
    Capture/resultwrapper.cpp \
    Capture/teensythreader.cpp \
    Connector/teensylightconnector.cpp \
    Controls/gridcolorrenderer.cpp \
    Controls/simplerectangle.cpp \
    Util/logger.cpp \
    Util/settings.cpp \
    Util/statistics.cpp \
    Views/mainwindow.cpp \
    Views/previewdialog.cpp \
    Views/settingsdialog.cpp \
    Views/statisticsdialog.cpp \
    main.cpp

HEADERS += \
    Capture/buffermanager.h \
    Capture/averagecolor.h \
    Capture/divisionthreader.h \
    Capture/enums.h \
    Capture/framedivider.h \
    Capture/framegrabber.h \
    Capture/framewrapper.h \
    Capture/resultwrapper.h \
    Capture/teensythreader.h \
    Connector/teensylightconnector.h \
    Controls/gridcolorrenderer.h \
    Controls/simplerectangle.h \
    Util/hidapi.h \
    Util/logger.h \
    Util/settings.h \
    Util/statistics.h \
    Views/configuration.h \
    Views/previewdialog.h \
    Views/settingsdialog.h \
    Views/mainwindow.h \
    Views/statisticsdialog.h

FORMS += \
    Views/mainwindow.ui \
    Views/previewdialog.ui \
    Views/settingsdialog.ui \
    Views/statisticsdialog.ui

unix: LIBS += -lXinerama
unix: LIBS += -lXfixes
unix: LIBS += -lscreen_capture_lite

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Images/SysTray/icon.svg

RESOURCES += \
    resources.qrc
