TEMPLATE = app
CONFIG += c++11

QT += qml quick widgets opengl openglextensions charts
INCLUDEPATH += /usr/local/include
QMAKE_CXXFLAGS += -fopenmp
LIBS += -L/usr/local/lib -lgsl -lblas -fopenmp
# DEFINES += POREISCBRT
SOURCES += main.cpp \
    statistics/statistic.cpp \
    statistics/poresizestatistic.cpp \
    montecarlo.cpp \
    statistics/porevolumestatistic.cpp \
    nogui.cpp \
    inifile.cpp \
    statistics/distributionstatistic.cpp \
    statistics/concentration.cpp \
    statistics/cumulativevolume.cpp \
    statistics/lengthratio.cpp \
    statistics/dvdlogd.cpp \
    geometry.cpp \
    mcobject.cpp \
    adsdesreader.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
# include(../SimVis/library_deployment.pri)

HEADERS += \
    statistics/statistic.h \
    statistics/poresizestatistic.h \
    random.h \
    montecarlo.h \
    statistics/statistics.h \
    statistics/porevolumestatistic.h \
    nogui.h \
    inifile.h \
    statistics/distributionstatistic.h \
    statistics/concentration.h \
    statistics/cumulativevolume.h \
    statistics/lengthratio.h \
    statistics/dvdlogd.h \
    geometry.h \
    mcobject.h \
    adsdesreader.h
