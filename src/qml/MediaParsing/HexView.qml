import QtQuick 2.12
import QtQuick.Controls 2.5
import "../common/strutils.js" as StrUtils
Item {
    id: root
    readonly property int rowHeight: 18

    ListView {
        id: hexListView
        anchors.fill: parent
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        onDragStarted:  interactive = false
        onDragEnded:  interactive = true


        model: hexListModel
        delegate: Row {
            id: hexListViewItem
            spacing: 4
            height: rowHeight

            // 偏移地址
            Rectangle {
                height: rowHeight
                width: offsetTxt.width
                color: "#dddde1"
                Text {
                    id: offsetTxt
                    text: "0x"+StrUtils.prefixZero(offset.toString(16), 8)
                }
            }// 偏移地址

            // 二进制区域
            Rectangle {
                id: hexRect
                width: hexRow.width
                height: hexRow.height
                color: "#f4f6fa"
                Row {
                    id: hexRow
                    spacing: 8
                    Repeater {
                        id: hexRepeater
                        model: 16
                        delegate: Rectangle {
                            width: rowHeight
                            height: rowHeight
                            color: "transparent"
                            Text{
                                id: hexTxt
                                anchors.centerIn: parent
                                text: size <= index?"":JSON.parse(hex_list)[index]
                            }
                        }
                    }
                } //hexRow
            } //二进制区域

            // ASCII区域
            Rectangle {
                id: asciiRect
                width: hexListView.width - asciiRect.x
                height: hexRow.height
                color: "#f4f6fa"

                Row {
                    id: asciiRow
                    spacing: 1
                    Repeater {
                        model: 16
                        delegate: Rectangle {
                            width: rowHeight
                            height: rowHeight
                            color: "transparent"
                            Text{
                                id:asciiTxt
                                anchors.centerIn: parent
                                text: size <= index?"":JSON.parse(ascii_list)[index]
                            }
                        }
                    }
                }
            } // ASCII区域
        } // delegate

        // 进度条
        ScrollBar.vertical: ScrollBar {
            anchors.top: hexListView.top
            anchors.topMargin: -1
            anchors.right: hexListView.right
            width: 15
            height: hexListView.height
        }
    }

    ListModel {
        id: hexListModel

    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onWheel: {
            if(hexListView.height>hexListView.contentHeight) {
                return
            }
            var upperLimit = 0 // 上限位置
            var lowerLimit = hexListView.contentHeight-hexListView.height // 下限位置
            var y = hexListView.contentY
            if(wheel.angleDelta.y < 0) {
                y += 20
            } else {
                y -= 20
            }
            if(y<0) { y = 0 }
            if(y>lowerLimit) { y = lowerLimit }

            hexListView.contentY = y;
        }
    }

    // arr:二进制结果
    /*[{"ascii_list":["o"],"hex_list":["6F"],"offset":672,"size":1}]*/
    function updateHexList(arr) {
        hexListModel.clear();
        hexListView.contentHeight = 0
        for(var i = 0; i < arr.length; ++i) {
            arr[i]["hex_list"] = JSON.stringify(arr[i]["hex_list"])
            arr[i]["ascii_list"] = JSON.stringify(arr[i]["ascii_list"])

            hexListModel.append(arr[i])
        }
    }
}
