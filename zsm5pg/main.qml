import QtQuick 2.5
import QtQuick.Window 2.2
import SimVis 1.0
import MySimulator 1.0
import QtCharts 2.0
import Zeolite 1.0

Window {
    id: rootWindow
    visible: true
    width: 1500
    height: 900
    MySimulator {
        id: simulator

        monteCarlo: MonteCarlo {
            geometry: myGeometry
            statistic: statistic
            standardDeviation: 0.01
            temperature: 1e-4
        }

        geometry: Zsm5geometry {
            id: myGeometry
            planesPerDimension: 50
            lengthScale: 2
        }

        statistic: PoreSizeStatistic {
            id: statistic
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
                var lambda = 0.76067
                for(var i in statistic.xValues) {
                    var x = statistic.xValues[i]
                    // console.log("x: "+x)
                    var y = statistic.yValues[i]
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
        width: 400
        height: 300
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
            name: "Data"
            axisX: xAxis
            axisY: yAxis
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

    Camera {
        id: camera
        farPlane: 1000000
        nearPlane: 0.01
    }
}
