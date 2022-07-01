import QtQuick 2.0
import "../common/time.js" as Time

Item {
    id: root
    property int lineHeight: (root.height-headerHeight)/itemName.length
    property int headerHeight: 24
    property var streamInfo: null
    property var itemName: ["name", "profile", "level", "chroma_format",
        "width", "height", "fram_count", "frame_rate", "duration"]

    Rectangle {
        id: header
        width: root.width
        height: root.headerHeight
        color: "#f0f0f0"
        Text {
            anchors.fill: header
            verticalAlignment: Text.AlignVCenter
            text: qsTr("Stream Info")
        }
    }

    Column {
        anchors.top: header.bottom
        width: root.width
        height: root.height-header.height
        Repeater {
            id: streamInfoRepeater
            model:root.itemName
            delegate: Rectangle {
                id: streamInfoItem
                height: root.lineHeight
                width: root.width
                color: index % 2 == 1 ? "#f4f6fa":"#ffffff"
                Text {
                    height: streamInfoItem.height
                    width: streamInfoItem.width/2
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    text: modelData
                }
                Text {
                    x: streamInfoItem.width/2
                    height: streamInfoItem.height
                    width: streamInfoItem.width/2
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    text: {
                        if(modelData === "name") {
                            return "value"
                        }
                        var txt = "- -"
                        if(root.streamInfo !== null) {
                            txt = root.streamInfo[modelData]
                        }
                        return txt

                    }
                }
            }
        }
    }

    function updateStreamInfo(info) {
        info["duration"] = Time.msec2string(info["duration"])
        streamInfo = info
//        console.log("updateStreamInfo:", JSON.stringify(info))
    }
}
