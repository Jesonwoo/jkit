import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.5

ApplicationWindow {
    id:window
    visible: true
    width: 640
    height: 480
    title: qsTr("jkit")

    Loader {
        anchors.fill: parent
        id: loader
    }

    Component.onCompleted: {
        loader.source = "MediaParsing/StreamParsingPage.qml"
    }

}
