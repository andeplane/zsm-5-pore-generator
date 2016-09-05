import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import QtCharts 2.1
import Zeolite 1.0

Window {
    id: rootWindow
    visible: true
    width: 1500
    height: 900
    property Statistic modelStatistic
    property Statistic dataStatistic
    property Statistic poreSizeDistribution
    property Statistic cumulativeVolume
    property Statistic currentStatistic
    property Statistic dvlogd
    property Statistic lengthRatio
    property NoGUI noGUI

    onCurrentStatisticChanged: {
        if(currentStatistic) {
            currentStatistic.histogramReady.connect(function() {
                currentStatistic.updateSeries(statisticSeries)
            })
            currentStatistic.updateSeries(statisticSeries)
        }
    }

    onModelStatisticChanged: {
        if(modelStatistic) {
            modelStatistic.histogramReady.connect(function() {
                modelStatistic.updateSeries(model)
            })
            modelStatistic.updateSeries(model)
        }
    }

    onDataStatisticChanged: {
        if(dataStatistic) {
            dataStatistic.histogramReady.connect(function() {
                dataStatistic.updateSeries(data)
            })
            dataStatistic.updateSeries(data)
        }
    }

    onNoGUIChanged: {
        timer.start()
        currentStatistic = noGUI.currentStatistic
        // console.log("Current statistic thing: ", noGUI.currentStatistic)
        // currentStatistic = noGUI.cumulativeVolume
        currentStatistic = noGUI.poreSizeDistribution
        // currentStatistic = noGUI.dvlogd
        // currentStatistic = noGUI.lengthRatio
    }

    Timer {
        id: timer
        interval: 1
        repeat: true
        onTriggered: {
            var finished = noGUI.tick()
            if(finished) {
                stop()
                Qt.quit()
            }
        }
    }

    ChartView {
        id: chart
        width: parent.width*0.5
        height: parent.height
        antialiasing: true
        legend.visible: true
        // title: "Pore size distribution"

        ScatterSeries {
            id: model
            name: "Model"
            axisX: _axisX
            axisY: _axisY
            markerSize: 5
            color: Qt.rgba(1,1,1,0)
            borderColor: "black"
            borderWidth: 1
        }

        LineSeries {
            id: data
            name: "Data"
            color: "red"
            axisX: _axisX
            axisY: _axisY
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
            max: 1000.0
            tickCount: 5
            titleText: "c"
        }
    }

    ChartView {
        id: statisticChart
        anchors.left: chart.right
        width: parent.width*0.5
        height: parent.height
        antialiasing: true
        title: currentStatistic ? currentStatistic.name : ""

        LineSeries {
            id: statisticSeries
            name: "Model"
            axisX: __axisX
            axisY: __axisY
        }

        ValueAxis {
            id: __axisX
            min: 0
            max: currentStatistic ? currentStatistic.max : 1
            tickCount: 5
            titleText: currentStatistic ? currentStatistic.xLabel : ""
        }
        ValueAxis {
            id: __axisY
            min: 0
            max: 2.0
            tickCount: 5
            titleText: currentStatistic ? currentStatistic.yLabel : ""
        }
    }

    Button {
        text: "Fit data"
        onClicked: {
            fitData()
        }
    }

    Button {
        text: "Save histogram source"
        onClicked: {
            fileDialogSave.visible = true
        }
    }

    FileDialog {
        id: fileDialogSave
        selectExisting : false
        title: "Please choose a location to save"

        onAccepted: {
            currentStatistic.saveHistogramValues(fileUrl)
        }
    }
    Settings {
        property alias lastOpenedFolderSave: fileDialogSave.folder
    }
}
