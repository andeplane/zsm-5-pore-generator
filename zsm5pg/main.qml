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

    MCObject {
        id: thickness
        name: "thickness"
        standardDeviation: monteCarlo.standardDeviation * 0.001
        value: thicknessSlider.value
    }

    MCObject {
        id: scaling
        name: "scaling"
        standardDeviation: monteCarlo.standardDeviation * 0.001
        value: scaleSlider.value
    }

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
            simulator.loadState()
        }
    }

    Concentration {
        id: adsorptionModel
        constant: false
        adsorption: true
        filePath: simulator.filePath
        scalingFactor: scaling.value
        zeoliteThickness: thickness.value
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
        adsorption: false
        filePath: simulator.filePath
        scalingFactor: scaling.value
        zeoliteThickness: thickness.value
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

    CumulativeVolume {
        id: cumulativeVolume
        constant: false
        filePath: simulator.filePath
        name: "cumulativeVolume"
    }

    DVDLogd {
        id: dvdlogd
        constant: false
        filePath: simulator.filePath
        name: "dvdlogd"
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
            saveState()
            Qt.quit()
        }

        statistics: [
            adsorptionModel,
            // desorptionModel,
            adsorptionData,
            // desorptionData,
            poreSizeStatistic,
            cumulativeVolume,
            dvdlogd
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
                adsorptionModel
                // desorptionModel
            ]
            datas: [
                adsorptionData
                // desorptionData
            ]
            mcObjects: [
//                scaling,
//                thickness
            ]
        }
    }

    ChartView {
        id: adsorptionChart
        width: parent.width*0.5
        height: parent.height
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
            borderColor: "red"
            borderWidth: 1
        }

        LineSeries {
            id: desorptionDataSeries
            name: "Data"
            color: "green"
            axisX: _axisX
            axisY: _axisY
        }

        ScatterSeries {
            id: desorptionModelSeries
            name: "Model"
            axisX: _axisX
            axisY: _axisY
            markerSize: 5
            markerShape: ScatterSeries.MarkerShapeRectangle
            color: Qt.rgba(1,1,1,0)
            borderColor: "green"
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
            max: 250.0
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
            tickCount: 10
            titleText: currentStatistic ? currentStatistic.xLabel : ""
        }
        ValueAxis {
            id: ___axisY
            min: 0
            max: 0.5
            tickCount: 5
            titleText: currentStatistic ? currentStatistic.yLabel : ""
        }
    }

    Column {
        Row {
            Label {
                text: "Scale: "
            }

            Slider {
                id: scaleSlider
                minimumValue: 0
                maximumValue: 2.0
                stepSize: 0.01
                value: 1.05
            }

            Button {
                text: "-"
                onClicked: scaleSlider.value -= scaleSlider.stepSize
            }

            Button {
                text: "+"
                onClicked: scaleSlider.value += scaleSlider.stepSize
            }

            Label {
                text: scaleSlider.value.toFixed(2)
            }
        }
        Row {
            Label {
                text: "Thickness: "
            }

            Slider {
                id: thicknessSlider
                minimumValue: 2
                maximumValue: 12.0
                stepSize: 0.01
                value: 8.67
            }

            Button {
                text: "-"
                onClicked: thicknessSlider.value -= thicknessSlider.stepSize
            }

            Button {
                text: "+"
                onClicked: thicknessSlider.value += thicknessSlider.stepSize
            }

            Label {
                text: thicknessSlider.value.toFixed(2)
            }
        }

//        Row {
//            Label {
//                text: "Temperature: "
//            }

//            Slider {
//                id: temperatureSlider
//                minimumValue: -10
//                maximumValue: 0.0
//                stepSize: 1.0
//                value: -1.0
//                // value: Math.log(monteCarlo.temperature) * Math.LOG10E
//                onValueChanged: {
//                    monteCarlo.temperature = Math.pow(10, value)
//                    console.log("Changing value to ", monteCarlo.temperature)
//                }
//            }

//            Button {
//                text: "-"
//                onClicked: temperatureSlider.value -= temperatureSlider.stepSize
//            }

//            Button {
//                text: "+"
//                onClicked: temperatureSlider.value += temperatureSlider.stepSize
//            }

//            Label {
//                text: Math.pow(10,temperatureSlider.value).toFixed(2)
//            }
//        }

    }

}
