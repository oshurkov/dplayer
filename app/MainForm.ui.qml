import QtQuick 2.5

Rectangle {
    property alias mouseArea: mouseArea
    property alias txtDrive: txtDrive

    width: 360
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
}

