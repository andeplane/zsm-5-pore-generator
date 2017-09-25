import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import QtCharts 2.1
import Zeolite 1.0

Window {
    property string iniFilename
    property var currentStatistic
    property var probabilities: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    id: rootWindow
    visible: true
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
            console.log("Starting to load stuff")
            simulator.loadIniFile(iniFile)
            geometry.loadIniFile(iniFile)
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
        scalingFactor: scaleSlider.value
        zeoliteThickness: thicknessSlider.value
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
        scalingFactor: scaleSlider.value
        zeoliteThickness: thicknessSlider.value
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

    NoGUI {
        id: simulator
        onFinishedChanged: {
            saveState()
            Qt.quit()
        }

        statistics: [
            adsorptionModel,
            desorptionModel,
            adsorptionData,
            desorptionData,
            poreSizeStatistic,
            cumulativeVolume,
            dvdlogd
        ]

        geometry: Geometry {
            id: geometry
            filePath: simulator.filePath
            mode: simulator.mode
        }
    }

    Row {
        id: topRow
        height: parent.height*0.2
        width: parent.width
        Column {
            Row {
                Label {
                    width: 80
                    text: "Scale: "
                }

                QQC1.Slider {
                    id: scaleSlider
                    width: 200
                    minimumValue: 0
                    maximumValue: 10.0
                    stepSize: 0.01
                    value: 1.0
                    onValueChanged: {
                        simulator.compute()
                    }
                }

                Button {
                    height: 1.5*label.height
                    width: 1.5*label.height
                    text: "-"
                    onClicked: scaleSlider.value -= scaleSlider.stepSize
                }

                Button {
                    height: 1.5*label.height
                    width: 1.5*label.height
                    text: "+"
                    onClicked: scaleSlider.value += scaleSlider.stepSize
                }

                Label {
                    text: scaleSlider.value.toFixed(2)
                }
            }

            Row {
                Label {
                    id: label
                    width: 80
                    text: "Thickness: "
                }

                QQC1.Slider {
                    width: 200
                    id: thicknessSlider
                    minimumValue: 0
                    maximumValue: 8.0
                    stepSize: 0.01
                    value: 2.0
                    onValueChanged: {
                        simulator.compute()
                    }
                }

                Button {
                    height: 1.5*label.height
                    width: 1.5*label.height
                    text: "-"
                    onClicked: thicknessSlider.value -= thicknessSlider.stepSize
                }

                Button {
                    height: 1.5*label.height
                    width: 1.5*label.height
                    text: "+"
                    onClicked: thicknessSlider.value += thicknessSlider.stepSize
                }

                Label {
                    text: thicknessSlider.value.toFixed(2)
                }
            }
            Row {
                Button {
                    text: "Compute"
                    onClicked: simulator.compute()
                }
                Button {
                    text: "New geometry"
                    onClicked: {
                        geometry.generateFromPSD(rootWindow.probabilities)
                        simulator.compute()
                    }
                }
            }
        }
        Row {
            Repeater {
                model: 19
                Column {
                    QQC1.Slider {
                        height: 100
                        orientation: Qt.Vertical
                        minimumValue: 0
                        maximumValue: 100
                        stepSize: 1
                        onValueChanged: {
                            if(index==0) return;
                            rootWindow.probabilities[index] = value
                            geometry.generateFromPSD(rootWindow.probabilities)
                            simulator.compute()
                        }
                    }
                    Label {
                        text: (index+1)
                    }
                }
            }
        }
    }

    Row {
        anchors.top: topRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

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
    }
}
