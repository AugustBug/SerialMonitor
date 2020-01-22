#-------------------------------------#
#                                     #
# Ahmert - KTM - KATSIS - 21.07.2016  #
#                                     #
#-------------------------------------#

QT += serialport
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SeriMonitor
TEMPLATE = app


SOURCES += main.cpp\
    message.cpp \
    stringprocessor.cpp \
    serimonitor.cpp

HEADERS  += \
    message.h \
    stringprocessor.h \
    serimonitor.h

FORMS    += \
    serimonitor.ui
