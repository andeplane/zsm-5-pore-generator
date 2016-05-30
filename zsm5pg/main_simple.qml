import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
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

    onNoGUIChanged: {
        timer.start()
        console.log("No GUI: " + noGUI)
        console.log("noGUI.cumulativeVolume: " + noGUI.cumulativeVolume)
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
            }
        }
    }

    function updateModel() {
        model.clear()
        for(var i=0; i<modelStatistic.bins; i++) {
            var x = modelStatistic.xValues[i]
            var y = modelStatistic.yValues[i]
            model.append(x,y)
        }
    }

    function updatePSD() {
        if(currentStatistic == undefined) return
        psd.clear()

        for(var i=0; i<currentStatistic.bins; i++) {
            var x = currentStatistic.xValues[i]
            var y = currentStatistic.yValues[i]
            psd.append(x,y)
        }
    }

    function updateData() {
        data.clear()
        for(var i=0; i<dataStatistic.bins; i++) {
            var x = dataStatistic.xValues[i]
            var y = dataStatistic.yValues[i]
            data.append(x,y)
        }
    }

    onModelStatisticChanged: {
        updateModel()

        modelStatistic.histogramReady.connect(function() {
            updateModel()
        })
    }

    onPoreSizeDistributionChanged: {
        updatePSD()

        poreSizeDistribution.histogramReady.connect(function() {
            updatePSD()
        })
    }

    onCumulativeVolumeChanged: {
        updatePSD()

        cumulativeVolume.histogramReady.connect(function() {
            updatePSD()
        })
    }

    onLengthRatioChanged: {
        updatePSD()

        lengthRatio.histogramReady.connect(function() {
            updatePSD()
        })
    }

    onDataStatisticChanged: {
        updateData()
        dataStatistic.histogramReady.connect(function() {
            updateData()
        })
    }

    function fitData() {
        if(dataStatistic==null || modelStatistic==null) return
        var xMin = 1e10
        var xMax = -1e10
        var yMin = 1e10
        var yMax = -1e10

        for(var i=0; i<dataStatistic.bins; i++) {
            var x = dataStatistic.xValues[i]
            var y = dataStatistic.yValues[i]

            xMin = Math.min(xMin, x)
            xMax = Math.max(xMax, x)
            yMin = Math.min(yMin, y)
            yMax = Math.max(yMax, y)
        }

        for(var i=0; i<modelStatistic.bins; i++) {
            var x = modelStatistic.xValues[i]
            var y = modelStatistic.yValues[i]

            xMin = Math.min(xMin, x)
            xMax = Math.max(xMax, x)
            yMin = Math.min(yMin, y)
            yMax = Math.max(yMax, y)
        }

        if(isNaN(xMin) || isNaN(xMax) || isNaN(yMin) || isNaN(yMax)) return;

        _axisX.min = xMin
        _axisX.max = xMax
        _axisY.min = yMin
        _axisY.max = yMax
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
        id: psdChart
        anchors.left: chart.right
        width: parent.width*0.5
        height: parent.height
        antialiasing: true
        title: currentStatistic.name

        LineSeries {
            id: psd
            name: "Model"
            axisX: __axisX
            axisY: __axisY
        }

        ValueAxis {
            id: __axisX
            min: 0
            max: currentStatistic.max
            tickCount: 5
            titleText: currentStatistic.xLabel
        }
        ValueAxis {
            id: __axisY
            min: 0
            max: 2.0
            tickCount: 5
            titleText: currentStatistic.yLabel
        }
    }

    Button {
        text: "Fit data"
        onClicked: {
            fitData()
        }
    }
}
