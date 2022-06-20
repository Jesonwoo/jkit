import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import com.jkit.app 1.0
Page {
    id: root

    Component.onCompleted: {
    }

    StreamParser {
        id: streamParser
    }

    Connections  {
        target: streamParser
        function onOpenFinished(info) {
            streamInfoView.updateStreamInfo(info)
            for(var i =0; i < streamParser.naluCount(); ++i) {
                var nal = streamParser.getNaluInfo(i);
                console.log(JSON.stringify(nal))
                naluListModel.append(nal);
            }
        }
    }

    Rectangle{
        anchors.fill: parent
        color: "#f0f0f0"
    }

    // h264流信息窗口
    StreamInfoView {
        id: streamInfoView
        x: naluListView.width
        y: menuBar.height
        height: root.height-menuBar.height
        width: root.width-naluListView.width
    }

    ListModel {
        id: naluListModel
        /*
            {
                "frame_num":249,
                "frame_type":"P",
                "length":384,
                "offset":"0x0005b930",
                "slice_type":5,
                "start_code":4
            }
        */
    }

    MenuBar {
        id: menuBar
        height: parent.height*0.08
        width: parent.width

        Menu {
            title: qsTr("&File")
            Action {
                text: qsTr("&Open...")
                onTriggered: {
                    fileDialog.folder = "file:///D:/develop/tool/ffmpeg/ffmpeg-static/bin/"
                    fileDialog.open()
                }
            }
            MenuSeparator { }
            Action { text: qsTr("&Quit") }
        }

    }

    ListView {
        id: naluListView
        x: 0
        clip: true
        interactive: true
        anchors.top: menuBar.bottom
        height: parent.height-menuBar.height
        width: parent.width * 0.7
        model: naluListModel
        delegate: naluDelegate
        ScrollBar.vertical: ScrollBar  {
            width: 10
            height: 50
        }

        onCurrentIndexChanged: {
        }
        // 折叠上一个选中item
        function foldLastItem() {
            if(naluListView.currentIndex !== -1) {
                naluListView.currentItem.fold()
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose an H264/AVC file")
        nameFilters: ["H264/AVC Files (*.h264)", "*.*"]
        onAccepted: {
            var filepath = new String(fileUrl)
            console.log("filepath:", filepath)
            streamParser.openStream(filepath.replace("file:///", ""));
        }
    }

    Component {
        id: naluDelegate
        Rectangle {
            id: naluItem
            width: naluListView.width
            height: naluInfoRect.height + (naluItem.isUnfolded ? syntaxView.height:0)
            property bool isUnfolded: false
            property color naluItemColor: {
                var color = "#000000"
                if(frame_type === "I") {
                    color = "#ff0000"
                } else if(frame_type === "P") {
                    color = "#0000ff"
                } else if(frame_type === "B") {
                    color = "#00c800"
                } else if(frame_type === "SPS") {
                    color = "#428456"
                } else if(frame_type === "PPS") {
                    color = "#ff0000"
                } else if(frame_type === "SEI") {
                    color = "#b18700"
                }
                return color
            }
            Rectangle {
                id: naluInfoRect
                width: naluItem.width
                height: 28
                property int spacing: 8
                Text {
                    id: offsetTxt
                    x: 0
                    height: naluInfoRect.height
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Qt.application.font.pixelSize * 1.2
                    color: "gray"
                    text: offset
                }
                // 色块
                Rectangle {
                    id: colorRect
                    x: offsetTxt.width + naluInfoRect.spacing
                    y: (naluInfoRect.height - height)/2
                    width: 4
                    height: naluInfoRect.height-4
                    color: naluItem.naluItemColor
                }
                // 箭头
                Text {
                    id: triangleTxt
                    x: colorRect.x+colorRect.width+naluInfoRect.spacing
                    height: naluInfoRect.height
                    verticalAlignment: Text.AlignVCenter
                    text: naluItem.isUnfolded ? "\u02c5":"\u02c3"
                    font.pixelSize: Qt.application.font.pixelSize * 1.2
                }
                // nalu内容
                Text {
                    id: contentTxt
                    x: triangleTxt.x+triangleTxt.width+naluInfoRect.spacing
                    height: naluInfoRect.height
                    verticalAlignment: Text.AlignVCenter
                    color: naluItem.naluItemColor
                    text: {
                        var str = ""
                        if(frame_type === "I"
                                ||frame_type === "P"
                                || frame_type === "B") {
                            str = String("%1 Frame #%2").arg(frame_type).arg(frame_num)
                        } else {
                            str = frame_type
                        }
                        return str
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if(naluListView.currentIndex !== index) {
                            naluListView.foldLastItem()
                        }
                        naluListView.currentIndex = index
                        naluItem.isUnfolded = !naluItem.isUnfolded

                        syntaxTxt.text = streamParser.getNaluSyntax(index)
                    }
                    onEntered: {
                        naluInfoRect.border.color = "#dddde1"
                    }
                    onExited: {
                        naluInfoRect.border.color = "#ffffff"
                    }
                }

            }
            // nalu下标
            Text {
                id: naludxTxt
                anchors.right: naluInfoRect.right
                anchors.rightMargin: 8
                verticalAlignment: Text.AlignVCenter
                height: naluInfoRect.height
                color: "gray"
                text: index
            }
            // 句法显示区域
            Rectangle {
                id: syntaxView
                anchors.top: naluInfoRect.bottom
                visible: naluItem.isUnfolded
                width: naluItem.width
                height: syntaxTxt.contentHeight
                Rectangle {
                    id: syntaxColorRect
                    x: colorRect.x + 4
                    y: (syntaxView.height - height)/2
                    width: 4
                    height: syntaxView.height-4
                    color: naluItem.naluItemColor
                }
                Text {
                    id: syntaxTxt
                    anchors.left: syntaxColorRect.right
                    anchors.leftMargin: 4
                    lineHeight: 1.2

                }
            }

            function fold() {
                naluItem.isUnfolded = false
            }

            function unfold() {
                naluItem.isUnfolded = true
            }
        }
    }
}
