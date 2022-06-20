import QtQuick 2.0
import "../common/time.js" as Time

Item {
    id: root
    property var streamInfo: null
    property var itemName: ["name", "profile", "level", "chroma_format",
        "width", "height", "fram_count", "frame_rate", "duration"]

    Column {
        anchors.fill: root
        Repeater {
            id: streamInfoRepeater
            model:root.itemName
            delegate: Rectangle {
                id: streamInfoItem
                height: 24
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
