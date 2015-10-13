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
            width: 250; height: 100
            anchors.right: parent.right
            y: 100
            model: myModel
            delegate: Text {
                    text: name
                    anchors.right: parent.right
            }
            highlight: Rectangle {
                color: "lightsteelblue"
                radius: 3
                border.width: 1
                border.color: "grey"
                anchors.right: parent.right
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
