TEMPLATE = app
CONFIG += c++11

QT += qml quick widgets opengl openglextensions charts

SOURCES += main.cpp \
    mysimulator.cpp \
    graph.cpp \
    figure.cpp \
    linegraph.cpp \
    zsm5geometry.cpp \
    distributionanalysis.cpp \
    statistics/statistic.cpp \
    statistics/poresizestatistic.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
include(../SimVis/library_deployment.pri)

HEADERS += \
    mysimulator.h \
    linegraph.h \
    figure.h \
    graph.h \
    zsm5geometry.h \
    distributionanalysis.h \
    statistics/statistic.h \
    statistics/poresizestatistic.h \
    random.h
