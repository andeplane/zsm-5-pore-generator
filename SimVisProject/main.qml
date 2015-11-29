import QtQuick 2.5
import QtQuick.Window 2.2
import SimVis 1.0
import MySimulator 1.0
import QMLPlot 1.0

Window {
    id: rootWindow
    visible: true
    width: 1650
    height: 1080
    MySimulator {
        id: simulator

    }

    Visualizer {
        anchors.fill: parent
        simulator: simulator
        camera: camera
        backgroundColor: "white"

//        SkyBox {
//            id: skybox
//            camera: camera
//            texture: ":/cubemap.jpg"
//        }

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

//            SkyBoxReflection {
//                id: reflection
//                skybox: skybox
//                reflectivity: 0.1
//            }
        }

        Figure {
            id: figure
            anchors.top: parent.top
            anchors.right: parent.right
            width: 500
            height: 500
            xMin: 0
            xMax: 5.0*simulator.planeSize/simulator.planesPerDimension
            yMin: 0
            yMax: 20
            LineGraph {
                id: linegraph
                dataSource: simulator.distribution
            }
            LineGraph {
                id: linegraph2
                dataSource: simulator.wantedDistribution
                color: "red"
            }
        }
    }

    Camera {
        id: camera
        farPlane: 1000000
        nearPlane: 0.01
    }


}

