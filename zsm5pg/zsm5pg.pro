TEMPLATE = app
CONFIG += c++11

QT += qml quick widgets opengl openglextensions charts

INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lgsl

SOURCES += main.cpp \
    mysimulator.cpp \
    zsm5geometry.cpp \
    distributionanalysis.cpp \
    statistics/statistic.cpp \
    statistics/poresizestatistic.cpp \
    montecarlo.cpp \
    statistics/filestatistic.cpp \
    statistics/porevolumestatistic.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
include(../SimVis/library_deployment.pri)

HEADERS += \
    mysimulator.h \
    zsm5geometry.h \
    distributionanalysis.h \
    statistics/statistic.h \
    statistics/poresizestatistic.h \
    random.h \
    montecarlo.h \
    statistics/statistics.h \
    statistics/filestatistic.h \
    statistics/porevolumestatistic.h
