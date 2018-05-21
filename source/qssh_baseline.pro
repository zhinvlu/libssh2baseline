HEADERS       = \
    $${PWD}/sshbaseline/inc/sshbaselinewindow.h
SOURCES       = \
    $${PWD}/main/src/main.cpp \
    $${PWD}/sshbaseline/src/sshbaselinewindow.cpp
RESOURCES    += \
    $${PWD}/resource/sshbaseline.qrc
QT           += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../3rdparty/lib/release/ -lssh2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../3rdparty/lib/debug/ -lssh2
else:unix:!macx: LIBS += -L$$PWD/../3rdparty/lib/ -lssh2

INCLUDEPATH += $$PWD/../3rdparty/include
DEPENDPATH += $$PWD/../3rdparty/include

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rdparty/lib/release/ssh2.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rdparty/lib/debug/ssh2.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/../3rdparty/lib/libssh2.a
