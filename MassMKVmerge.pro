#-------------------------------------------------
#
# Project created by QtCreator 2016-10-28T16:38:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MassMKVmerge
TEMPLATE = app


SOURCES += main.cpp\
        massmkvmerge.cpp \
    filesbox.cpp \
    mergefiles.cpp

HEADERS  += massmkvmerge.h \
    filesbox.h \
    mergefiles.h

FORMS    += massmkvmerge.ui \
    form.ui
