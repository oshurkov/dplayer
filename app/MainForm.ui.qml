import QtQuick 2.5

Rectangle {
    id: rectangle_main
    width: 800; height: 600

    property alias txtDrive: txtDrive
    property alias txtMemo: txtMemo

    ListView {
        width: 100; height: 100
        anchors.right: parent.right
        anchors.top: parent.top
        model: myModel
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        focus: true
        delegate: Rectangle {
            height: 25
            width: 100
            Text { text: modelData }
        }
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

