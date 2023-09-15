QT += quick
QT += serialport

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        canmsgmodel.cpp \
        main.cpp \
        recentfilemodel.cpp \
        serialportlist.cpp \
        totalmanager.cpp

RESOURCES += qml.qrc \
    icons/openfile.png \

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    canmsgmodel.h \
    recentfilemodel.h \
    serialportlist.h \
    totalmanager.h

DISTFILES += \
    ConnectDialog.qml \
    D2Coding.ttf \
    icons/openfile.png \
    ucanviewmodel.qmodel
