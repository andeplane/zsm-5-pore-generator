import QtQuick 2.5
import QtQuick.Window 2.2
import SimVis 1.0
import MySimulator 1.0

Window {
    visible: true
    width: 1024
    height: 768
    MySimulator {
        id: simulator

    }

    Visualizer {
        anchors.fill: parent
        simulator: simulator
        camera: camera
        backgroundColor: "black"

        SkyBox {
            id: skybox
            camera: camera
            texture: ":/cubemap.jpg"
        }

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
                attenuation: 0.001
                position: camera.position
            }

            SkyBoxReflection {
                id: reflection
                skybox: skybox
                reflectivity: 0.2
            }

            SimplexBump {
                id: simplexBump
                enabled: false
                intensity: 0.01
                scale: 10.0
            }
        }
    }

    Camera {
        id: camera
        farPlane: 1000000
        nearPlane: 0.01
    }


}

