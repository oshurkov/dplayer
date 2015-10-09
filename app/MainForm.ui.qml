import QtQuick 2.5

Rectangle {
    id: rectangle1
    property alias mouseArea: mouseArea
    property alias txtDrive: txtDrive
    property alias txtMemo: txtMemo

    width: 560
    height: 360

    MouseArea {
        id: mouseArea
        anchors.fill: parent
    }

    Text {
        id: txtDrive
        objectName: "txtDrive"
        anchors.centerIn: parent
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

