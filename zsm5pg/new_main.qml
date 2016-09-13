import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0
import QtCharts 2.1
import Zeolite 1.0

Window {
    property string iniFilename
    id: rootWindow
    visible: simulator.visualize
    width: 1500
    height: 900

    IniFile {
        id: inifile
        filename: iniFilename
        onReadyChanged: {
            adsorption.loadIniFile(iniFile)
            desorption.loadIniFile(iniFile)
        }
    }

    Concentration {
        id: adsorption
    }

    Concentration {
        id: desorption
    }

    PoreSizeStatistic {
        id: poreSizeStatistic
    }

    NoGUI {
        id: simulator
        statistics: [
            adsorption,
            desorption
        ]

        geometry: Geometry {
            id: geometry
            filePath: simulator.filePath
            mode: simulator.mode
        }
        monteCarlo: MonteCarlo {
            geometry: geometry
            filePath: simulator.filePath
            models: [
                adsorption,
                desorption
            ]
            datas: [

            ]
        }
    }
}
