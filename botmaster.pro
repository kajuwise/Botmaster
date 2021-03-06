
# -------------------------------------------------
# Project created by QtCreator 2010-05-23T17:50:46
# -------------------------------------------------
QT += network
QT += gui
QT += widgets
TARGET = botmaster
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
QMAKE_CXXFLAGS += -O3 -march=native -ftree-vectorize

DEFINES += XI
SOURCES += main.cpp \
    server.cpp \
    protocolparser.cpp \
    abstractrobot.cpp \
    robotfactory.cpp \
    config.cpp \
    comm.cpp \
    camera.cpp \
    objecttype.cpp \
    image.cpp \
    view.cpp \
    timer.cpp \
    sendthread.cpp \
    robots/neve/nevetest.cpp \
    robots/neve/neve.cpp \
    visiontime.cpp \
    capturethread.cpp \
    imagecalibrationtool.cpp \
    imagecalibrationitem.cpp \
    robots/neve/stateholder.cpp \
    blackoutconf.cpp \
    robottesttool.cpp \
    robottesttoolitem.cpp
HEADERS += server.h \
    protocolparser.h \
    abstractrobot.h \
    robotfactory.h \
    config.h \
    comm.h \
    camera.h \
    objecttype.h \
    image.h \
    view.h \
    timer.h \
    sendthread.h \
    robots/neve/nevetest.h \
    robots/neve/neve.h \
    visiontime.h \
    capturethread.h \
    imagecalibrationtool.h \
    imagecalibrationitem.h \
    robots/neve/stateholder.h \
    blackoutconf.h \
    robottesttool.h \
    robottesttoolitem.h

LIBS += -L/usr/local/lib -lopencv_legacy -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann
LIBS += -lm3api

INCLUDEPATH += /usr/include/opencv
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /home/user/package/include
INCLUDEPATH += /opt/XIMEA/include/
