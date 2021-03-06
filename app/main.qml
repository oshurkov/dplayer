import QtQuick 2.5
import QtQuick.Window 2.2

Window {
    visible: true
    title: qsTr("DATACAM Player ")
    width: 640
    height: 480

    maximumHeight : height
    maximumWidth : width
    minimumHeight : height
    minimumWidth : width

    Rectangle {
        id: rectangle_main
        width: parent.width
        height: parent.height
        color: "#ffff00"
        border.color: "#000000"

        property alias txtDrive: txtDrive
        property alias txtMemo: txtMemo

        ListView {
            id: listview
            width: 250; height: 100
            anchors.right: parent.right
            y: 100
            model: myModel
            delegate: Component {
                Item {
                    width: parent.width
                    height: 25
                    Column {
                        Text {
                            text: name
                            anchors.right: parent.right
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            listview.currentIndex = index
                            _chekUSB.msg(name)
                        }
                    }
                }
            }
            highlight: Rectangle {
                color: "lightsteelblue"
                x: listview.currentItem.x
            }
            focus: true
        }

        Text {
            id: txtDrive
            objectName: "txtDrive"
            anchors.left: parent.left
            anchors.top: parent.top
            text: "no usb disk"
        }

        Text {
            id: txtMemo
            width: 560
            height: 21
            objectName: "txtMemo"
            text: "Memo..."
            anchors.left: parent.left
            anchors.bottom: parent.bottom
        }

    }

}
