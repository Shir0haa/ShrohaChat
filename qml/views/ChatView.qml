pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ShirohaChat

// ChatView：聊天主视图
Rectangle {
    id: root
    color: "white"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 顶部：当前会话名称
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#f9f9f9"

            Text {
                anchors.centerIn: parent
                text: ChatController.currentSessionName || "选择一个会话"
                font.pixelSize: 18
                font.bold: true
            }

            // 底部分隔线
            Rectangle {
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: 1
                color: "#e0e0e0"
            }
        }

        // 中部：消息列表
        ListView {
            id: messageListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            model: ChatController.currentMessageModel

            delegate: MessageBubble {
                width: messageListView.width
            }

            // 自动滚动逻辑
            function scrollToBottom() {
                positionViewAtEnd()
            }

            onCountChanged: {
                Qt.callLater(scrollToBottom)
            }

            Component.onCompleted: {
                scrollToBottom()
            }

            // 占位符
            Text {
                anchors.centerIn: parent
                text: "暂无消息"
                font.pixelSize: 16
                color: "#999"
                visible: messageListView.count === 0
            }
        }

        // 底部：输入栏
        InputBar {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
        }
    }
}
