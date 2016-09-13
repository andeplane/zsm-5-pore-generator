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
            simulator.loadIniFile(iniFile)
            geometry.loadIniFile(iniFile)
            monteCarlo.loadIniFile(iniFile)
            adsorption.loadIniFile(iniFile)
            desorption.loadIniFile(iniFile)
            adsorptionData.loadIniFile(iniFile)
            desorptionData.loadIniFile(iniFile)
        }
    }

    Concentration {
        id: adsorption
        name: "adsorption"
    }

    Concentration {
        id: desorption
        name: "desorption"
    }

    Statistic {
        id: adsorptionData
        name: "adsorptionData"
        sourceKey: "adsorptionData"
    }

    Statistic {
        id: desorptionData
        name: "desorptionData"
        sourceKey: "desorptionData"
    }

    PoreSizeStatistic {
        id: poreSizeStatistic
        name: "poresizestatistic"
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
            id: monteCarlo
            geometry: geometry
            filePath: simulator.filePath
            models: [
                adsorption,
                desorption
            ]
            datas: [
                adsorptionData,
                desorptionData
            ]
        }
    }
}
