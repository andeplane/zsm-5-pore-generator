import QtQuick 2.5
import QtQuick.Window 2.2
import SimVis 1.0
import MySimulator 1.0
import QtCharts 2.0

Window {
    id: rootWindow
    visible: true
    width: 1650
    height: 1080
    MySimulator {
        id: simulator
        planeSize: 100
        planesPerDimension: 100
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

    Camera {
        id: camera
        farPlane: 1000000
        nearPlane: 0.01
    }


}

