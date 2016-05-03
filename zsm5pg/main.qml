import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import SimVis 1.0
import MySimulator 1.0
import QtCharts 2.0
import Zeolite 1.0

Window {
    id: rootWindow
    visible: true
    width: 1500
    height: 900

    PoreSizeStatistic {
        id: poreSizeStatistic
        bins: 50
        min: 0
        max: 10
        onHistogramReady: {
            lineSeries.clear()
            lineSeries2.clear()
            var xMin = 1e10
            var xMax = 0
            var yMin = 1e10
            var yMax = 0
            var lambda = 0.76078
            for(var i in xValues) {
                var x = xValues[i]
                var y = yValues[i]
                var yTarget = lambda*Math.exp(-lambda*(x-2.0))
                if(x < 2.0) yTarget = 0
                lineSeries.append(x,y)
                lineSeries2.append(x, yTarget)
                xMin = Math.min(xMin, x)
                xMax = Math.max(xMax, x)
                yMin = Math.min(yMin, y)
                yMax = Math.max(yMax, y)
                yMin = Math.min(yMin, yTarget)
                yMax = Math.max(yMax, yTarget)
            }
            // xAxis.min = xMin*0.9
            // xAxis.max = xMax*1.1
            xAxis.max = xMax
            // yAxis.min = yMin*0.9
            yAxis.max = yMax*1.1
        }
    }

    PoreVolumeStatistic {
        id: poreVolumeStatistic
        bins: 100
        min: 10
        max: 1000
        onHistogramReady: {
            lineSeries.clear()
            lineSeries2.clear()
            var xMin = 1e10
            var xMax = 0
            var yMin = 1e10
            var yMax = 0
            var lambda = 0.76078
            for(var i in xValues) {
                var x = xValues[i]
                var y = yValues[i]
                var yTarget = lambda*Math.exp(-lambda*(x-2.0))
                if(x < 2.0) yTarget = 0
                lineSeries.append(x,y)
                lineSeries2.append(x, yTarget)
                xMin = Math.min(xMin, x)
                xMax = Math.max(xMax, x)
                yMin = Math.min(yMin, y)
                yMax = Math.max(yMax, y)
                yMin = Math.min(yMin, yTarget)
                yMax = Math.max(yMax, yTarget)
            }
            xAxis.min = xMin*0.9
            xAxis.max = xMax*1.1
            yAxis.min = yMin*0.9
            yAxis.max = yMax*1.1
        }
    }

    MySimulator {
        id: simulator

        monteCarlo: MonteCarlo {
            id: monteCarlo
            geometry: myGeometry
            statistic: simulator.statistic
            standardDeviation: stdDevSlider.value
            temperature: tempSlider.value
            running: true
        }

        geometry: Zsm5geometry {
            id: myGeometry
            planesPerDimension: 50
            lengthScale: 2
        }

        statistic: poreSizeStatistic
    }

    Visualizer {
        anchors.fill: parent
        simulator: simulator
        camera: camera
        backgroundColor: "white"

        TrackballNavigator {
            id: navigator
            anchors.fill: parent
            camera: camera
        }

        TriangleCollection {
            id: triangles

            Light {
                id: light
                ambientColor: "blue"
                specularColor: "white"
                diffuseColor: "green"
                ambientIntensity: 0.025
                diffuseIntensity: 0.5
                specularIntensity: 0.01
                shininess: 100.0
                attenuation: 0.0
                position: camera.position
            }
        }
    }

    ChartView {
        id: chart
        width: 600
        height: 400
        antialiasing: true
        legend.visible: true
        title: "Pore size distribution"

        ValueAxis {
            id: xAxis
            min: 0
            max: 2
            tickCount: 5
            titleText: "d [nm]"
        }
        ValueAxis {
            id: yAxis
            min: 0
            max: 0.2
            tickCount: 5
            titleText: "P(d)"
        }
        LineSeries {
            id: lineSeries
            name: "3d model"
            axisX: xAxis
            axisY: yAxis
            style: Qt.DotLine
            width: 3
        }

        LineSeries {
            id: lineSeries2
            name: "Target"
            axisX: xAxis
            axisY: yAxis
        }

        MouseArea {
            anchors.fill: parent
            drag.target: parent
        }
    }

    Rectangle {
        radius: 10
        width: 400
        height: 200
        anchors.top: parent.top
        anchors.right: parent.right
        color: "white"

        MouseArea {
            anchors.fill: parent
            drag.target: parent
        }

        Column {
            spacing: 10

            Row {
                Text {
                    text: "FPS: "+(1.0/simulator.tickTime).toFixed(2)
                }
            }

            Row {
                Text {
                    text: "Monte carlo: "
                }
                Button {
                    id: startButton
                    text: monteCarlo.running ? "Stop" : "Start"
                    onClicked: {
                        monteCarlo.running = !monteCarlo.running
                    }
                }
            }

            Row {
                Text {
                    text: "MC stddev ("+monteCarlo.standardDeviation.toFixed(3)+"): "
                }
                Slider {
                    id: stdDevSlider
                    minimumValue: 0.01
                    maximumValue: 0.1
                    stepSize: 0.01
                    value: 0.01
                }
            }

            Row {
                Text {
                    text: "MC temp ("+monteCarlo.temperature.toFixed(5)+"): "
                }
                Slider {
                    id: tempSlider
                    minimumValue: 0.0
                    maximumValue: 1.0
                    stepSize: 0.001
                    value: 0.0
                }
            }

            Row {
                Button {
                    text: "Load"
                    onClicked: {
                        myGeometry.load("/projects/geometry.txt")
                    }
                }
                Button {
                    text: "Save"
                    onClicked: {
                        myGeometry.save("/projects/geometry.txt")
                    }
                }
            }
        }
    }

    Camera {
        id: camera
        farPlane: 1000000
        nearPlane: 0.01
    }
}
