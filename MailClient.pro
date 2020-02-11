QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

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
    src/debugconsole.cpp \
    src/imap_connection.cpp \
    src/imap_mailbox.cpp \
    src/imap_message.cpp \
    src/imap_parsers.cpp \
    src/imap_structures.cpp \
    src/logindialog.cpp \
    src/maillistmodel.cpp \
    src/main.cpp \
    src/mainwindow.cpp

HEADERS += \
    src/debugconsole.h \
    src/imap_connection.h \
    src/imap_mailbox.h \
    src/imap_message.h \
    src/imap_parsers.h \
    src/imap_structures.h \
    src/logindialog.h \
    src/maillistmodel.h \
    src/mainwindow.h

FORMS += \
    src/debugconsole.ui \
    src/logindialog.ui \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
