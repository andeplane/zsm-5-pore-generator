import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import QtCharts 2.1
import Zeolite 1.0

Window {
    property string iniFilename
    property var currentStatistic
    id: rootWindow
    visible: simulator.visualize
    width: 1500
    height: 900

    onCurrentStatisticChanged: {
        currentStatistic.histogramReady.connect(function() {
            currentStatistic.updateSeries(statisticSeries)
            ___axisX.applyNiceNumbers()
        })
        currentStatistic.updateSeries(statisticSeries)
    }

    IniFile {
        id: iniFile
        filename: iniFilename
        onReadyChanged: {
            simulator.loadIniFile(iniFile)
            geometry.loadIniFile(iniFile)
            monteCarlo.loadIniFile(iniFile)
            adsorptionModel.loadIniFile(iniFile)
            desorptionModel.loadIniFile(iniFile)
            adsorptionData.loadIniFile(iniFile)
            desorptionData.loadIniFile(iniFile)
            poreSizeStatistic.loadIniFile(iniFile)
            currentStatistic = poreSizeStatistic
        }
    }

    Concentration {
        id: adsorptionModel
        constant: false
        filePath: simulator.filePath
        name: "adsorption"
        sourceKey: "adsorptionModel"
        Component.onCompleted: {
            histogramReady.connect(function() {
                updateSeries(adsorptionModelSeries)
            })
            updateSeries(adsorptionModelSeries)
        }
    }

    Concentration {
        id: desorptionModel
        constant: false
        filePath: simulator.filePath
        name: "desorption"
        sourceKey: "desorptionModel"
        Component.onCompleted: {
            histogramReady.connect(function() {
                updateSeries(desorptionModelSeries)
            })
            updateSeries(desorptionModelSeries)
        }
    }

    Statistic {
        id: adsorptionData
        filePath: simulator.filePath
        name: "adsorptionData"
        sourceKey: "adsorptionData"
        Component.onCompleted: {
            histogramReady.connect(function() {
                updateSeries(adsorptionDataSeries)
            })
            updateSeries(adsorptionDataSeries)
        }
    }

    Statistic {
        id: desorptionData
        filePath: simulator.filePath
        name: "desorptionData"
        sourceKey: "desorptionData"
        Component.onCompleted: {
            histogramReady.connect(function() {
                updateSeries(desorptionDataSeries)
            })
            updateSeries(desorptionDataSeries)
        }
    }

    PoreSizeStatistic {
        id: poreSizeStatistic
        constant: false
        filePath: simulator.filePath
        name: "poreSizeDistribution"
    }

    Timer {
        interval: 1
        repeat: true
        running: true
        onTriggered: {
            simulator.tick()
        }
    }

    NoGUI {
        id: simulator
        onFinishedChanged: {
            console.log("Simulation finished")
            Qt.quit()
        }

        statistics: [
            adsorptionModel,
            desorptionModel,
            adsorptionData,
            desorptionData,
            poreSizeStatistic
        ]

        geometry: Geometry {
            id: geometry
            filePath: simulator.filePath
            mode: simulator.mode
        }
        monteCarlo: MonteCarlo {
            id: monteCarlo
            geometry: geometry
            filePath: simulator.filePath
            models: [
                adsorptionModel,
                desorptionModel
            ]
            datas: [
                adsorptionData,
                desorptionData
            ]
        }
    }

    ChartView {
        id: adsorptionChart
        width: parent.width*0.5
        height: parent.height*0.5
        antialiasing: true
        legend.visible: true
        title: "Adsorption"

        LineSeries {
            id: adsorptionDataSeries
            name: "Data"
            color: "red"
            axisX: _axisX
            axisY: _axisY
        }

        ScatterSeries {
            id: adsorptionModelSeries
            name: "Model"
            axisX: _axisX
            axisY: _axisY
            markerSize: 5
            color: Qt.rgba(1,1,1,0)
            borderColor: "black"
            borderWidth: 1
        }

        ValueAxis {
            id: _axisX
            min: 0
            max: 1
            tickCount: 5
            titleText: "P [bar]"
        }
        ValueAxis {
            id: _axisY
            min: 0
            max: 500.0
            tickCount: 5
            titleText: "c"
        }
    }

    ChartView {
        id: desorptionChart
        width: parent.width*0.5
        height: parent.height*0.5
        anchors.top: adsorptionChart.bottom
        antialiasing: true
        legend.visible: true
        title: "Desorption"

        LineSeries {
            id: desorptionDataSeries
            name: "Data"
            color: "red"
            axisX: __axisX
            axisY: __axisY
        }

        ScatterSeries {
            id: desorptionModelSeries
            name: "Model"
            axisX: __axisX
            axisY: __axisY
            markerSize: 5
            color: Qt.rgba(1,1,1,0)
            borderColor: "black"
            borderWidth: 1
        }

        ValueAxis {
            id: __axisX
            min: 0
            max: 1
            tickCount: 5
            titleText: "P [bar]"
        }
        ValueAxis {
            id: __axisY
            min: 0
            max: 500.0
            tickCount: 5
            titleText: "c"
        }
    }

    ChartView {
        id: statisticChart
        anchors.left: adsorptionChart.right
        width: parent.width*0.5
        height: parent.height
        antialiasing: true
        title: currentStatistic ? currentStatistic.name : ""

        LineSeries {
            id: statisticSeries
            axisX: ___axisX
            axisY: ___axisY
        }

        ValueAxis {
            id: ___axisX
            min: 0
            max: currentStatistic ? currentStatistic.max : 1
            tickCount: 5
            titleText: currentStatistic ? currentStatistic.xLabel : ""
        }
        ValueAxis {
            id: ___axisY
            min: 0
            max: 2.0
            tickCount: 5
            titleText: currentStatistic ? currentStatistic.yLabel : ""
        }
    }
}
