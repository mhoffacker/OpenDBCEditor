#-------------------------------------------------
#
# Project created by QtCreator 2015-10-17T03:26:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenDBCEditor
TEMPLATE = app

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O0
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_CXXFLAGS_RELEASE -= -Os

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O0

SOURCES += src/main.cpp\
        src/opendbceditor.cpp \
    src/treeitem.cpp \
    src/libcandbc/dbcModel.c \
    src/libcandbc/dbcReader.c \
    src/libcandbc/dbcWriter.c \
    src/libcandbc/lexer.c \
    src/libcandbc/parser.c \
    src/dialogabout.cpp


HEADERS  += include/opendbceditor.h \
    include/treeitem.h \
    include/dialogabout.h

FORMS    += ui/opendbceditor.ui \
    ui/dialogabout.ui

RESOURCES += \
    resources.qrc
