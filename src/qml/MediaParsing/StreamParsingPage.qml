import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import com.jkit.app 1.0
import "../common/strutils.js" as StrUtils

Page {
    id: root

    Component.onCompleted: {

    }

    StreamParser {
        id: streamParser
    }

    BinaryStream {
        id: binaryStream
    }

    Connections  {
        target: streamParser
        function onOpenFinished(info) {
            streamInfoView.updateStreamInfo(info)
            for(var i =0; i < streamParser.naluCount(); ++i) {
                var nal = streamParser.getNaluInfo(i);
                naluListModel.append(nal);
            }
        }
    }

//    Rectangle{
//        id: bg
//        anchors.fill: parent
//        color: "#f0f0f0"
//    }

    // h264流信息窗口
    StreamInfoView {
        id: streamInfoView
        x: naluListView.width
        y: menuBar.height
        headerHeight: 24
        height: root.height-menuBar.height-hexView.height
        width: root.width-naluListView.width
    }

    ListModel {
        id: naluListModel
        /*
            {
                "frame_num":249,
                "slice_num":0,
                "frame_type":"P",
                "length":384,
                "offset":0,
                "slice_type":5,
                "start_code":4
            }
        */
    }

    MenuBar {
        id: menuBar
        width: parent.width

        Menu {
            title: qsTr("&File")
            Action {
                text: qsTr("&Open...")
                onTriggered: {
                    fileDialog.open()
                }
            }
            MenuSeparator { }
            Action {
                text: qsTr("&Quit")
                onTriggered: {
                    Qt.quit()
                }
            }
        }
    }

    ListView {
        id: naluListView
        x: 0
        clip: true
        interactive: true
        anchors.top: menuBar.bottom
        height: parent.height-menuBar.height-hexView.height
        width: parent.width * 0.7
        model: naluListModel
        delegate: naluDelegate
        ScrollBar.vertical: ScrollBar  {
            width: 10
            height: 50
        }
        // 收起或展开item
        // index: 激活的item index
        function foldOrUnfoldItem(index) {
            // 收起上一个打开
            var curIdx = naluListView.currentIndex
            var curItem = naluListView.currentItem
            if(curIdx!== -1 && curIdx !== index) {
                curItem.fold()
            }
            naluListView.currentIndex = index
            naluListView.currentItem.switchFoldAndUnfold()
        }
        // 高亮选中
        function highlight(index) {
            var curIdx = naluListView.currentIndex
            var curItem = naluListView.currentItem
            if(curIdx!== -1 && curIdx !== index) {
                curItem.setHighlight(false)
            }
            naluListView.currentIndex = index
            naluListView.currentItem.setHighlight(true)
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Please choose an H264/AVC file")
        nameFilters: ["H264/AVC Files (*.h264)", "*.*"]
        onAccepted: {
            console.log("filepath:", fileUrl)
            streamParser.openStream(fileUrl)
            binaryStream.openStream(fileUrl)
        }
    }

    Component {
        id: naluDelegate
        Rectangle {
            id: naluItem
            width: naluListView.width
            height: naluInfoRect.height + (naluItem.isUnfolded ? syntaxView.height:0)
            property bool isUnfolded: false
            property bool highlight: false
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
            // nulu信息
            Rectangle {
                id: naluInfoRect
                width: naluItem.width
                height: 28
                color: naluItem.highlight  ? "#0079d8":"#ffffff"

                property int spacing: 8
                TextMetrics {
                    id: textMetrics
                    text: "0x00000000"
                }

                Text {
                    id: offsetTxt
                    x: 0
                    height: naluInfoRect.height
                    width: textMetrics.width+naluInfoRect.spacing
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: Qt.application.font.pixelSize * 1.2
                    color: "gray"
                    text: "0x"+StrUtils.prefixZero(offset.toString(16), 8)
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
                    objectName: "triangle"
                    text: naluItem.isUnfolded ? "\u02c5":"\u02c3"
                    font.pixelSize: Qt.application.font.pixelSize * 1.6
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
                            str = String("%1 Frame #%2 slice #%3").arg(frame_type).arg(frame_num).arg(slice_num)
                        } else {
                            str = frame_type
                        }
                        return str
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
                    width: syntaxView.width - syntaxTxt.x
                    lineHeight: 1.2
                    elide: Text.ElideRight
                }
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onDoubleClicked: {
                    naluListView.foldOrUnfoldItem(index)
                    syntaxTxt.text = streamParser.getNaluSyntax(index)
                }
                onClicked: {
                    if(naluListView.currentIndex === index) {
                        return
                    }

                    var arr = binaryStream.getHexList(offset, length, 16)
                    hexView.updateHexList(arr);
                    naluListView.highlight(index)
                }
                onEntered: {
                    naluInfoRect.border.color = "#dddde1"
                }
                onExited: {
                    naluInfoRect.border.color = "#ffffff"
                }
            }
            // 收起item
            function fold() {
                naluItem.isUnfolded = false
            }
            // 展开item
            function unfold() {
                naluItem.isUnfolded = true
            }
            // 是否展开
            function isUnFlod() {
                return naluItem.isUnfolded
            }
            // 切换展开和收起的状态
            function switchFoldAndUnfold() {
                naluItem.isUnfolded = !naluItem.isUnfolded
            }
            // 设置是否高亮
            function setHighlight(enable) {
                naluItem.highlight = enable
            }
        }
    }

    HexView {
        id: hexView
        y: root.height*0.7

        width: root.width
        height: root.height*0.3
    }
}
