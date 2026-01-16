import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ShirohaChat

// InputBar：输入栏组件
Rectangle {
    id: root
    color: "#f9f9f9"

    // 顶部分隔线
    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: 1
        color: "#e0e0e0"
    }

    RowLayout {
        anchors {
            fill: parent
            margins: 12
        }
        spacing: 12

        TextField {
            id: inputField
            Layout.fillWidth: true
            placeholderText: qsTr("输入消息...")
            font.pixelSize: 15

            // 回车发送
            Keys.onReturnPressed: {
                sendButton.clicked()
            }
        }

        Button {
            id: sendButton
            text: qsTr("发送")
            highlighted: true
            enabled: inputField.text.trim().length > 0

            onClicked: {
                if (inputField.text.trim().length > 0) {
                    ChatController.sendMessage(inputField.text)
                    inputField.clear()
                }
            }
        }
    }
}
