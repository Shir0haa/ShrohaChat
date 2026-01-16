pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ShirohaChat

// SessionSidebar：会话列表侧边栏
Rectangle {
    id: root
    color: "#f5f5f5"

    // 状态管理属性
    property bool isLoading: false
    property bool isError: false
    property string errorMessage: ""

    // 加载状态指示器
    BusyIndicator {
        anchors.centerIn: parent
        running: root.isLoading
        visible: root.isLoading
    }

    ListView {
        id: sessionListView
        anchors.fill: parent
        model: ChatController.sessionModel
        clip: true
        visible: !root.isLoading

        delegate: Item {
            required property string displayName
            required property string lastMessage
            required property string timeString
            required property int unreadCount
            required property string sessionId
            required property string avatarUrl

            width: sessionListView.width
            height: 72

            // 基础布局：使用 RowLayout
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 12

                // 1. 头像占位符（暂时用 Rectangle 代替 Avatar）
                Rectangle {
                    Layout.preferredWidth: 48
                    Layout.preferredHeight: 48
                    Layout.alignment: Qt.AlignVCenter
                    radius: 24
                    color: "#cccccc"

                    Text {
                        anchors.centerIn: parent
                        text: displayName.length > 0 ? displayName.charAt(0).toUpperCase() : "?"
                        font.pixelSize: 20
                        font.bold: true
                        color: "white"
                    }
                }

                // 2. 中间信息区 (标题 + 预览)
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter
                    spacing: 4

                    Text {
                        text: displayName
                        font.pixelSize: 16
                        font.bold: true
                        color: "#333333"
                        Layout.fillWidth: true
                    }

                    Text {
                        text: lastMessage
                        font.pixelSize: 14
                        color: "#888888"
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                }

                // 3. 右侧信息区 (时间 + 未读)
                ColumnLayout {
                    Layout.alignment: Qt.AlignTop | Qt.AlignRight
                    spacing: 8

                    Text {
                        text: timeString
                        font.pixelSize: 12
                        color: "#AAAAAA"
                    }

                    Rectangle {
                        width: 20
                        height: 20
                        radius: 10
                        color: "#FF3B30"
                        visible: unreadCount > 0

                        Text {
                            anchors.centerIn: parent
                            text: unreadCount > 99 ? "99+" : unreadCount
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }
                }
            }

            // 交互区域
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    ChatController.switchToSession(sessionId)
                }
            }

            // 背景与分隔线
            Rectangle {
                anchors.fill: parent
                color: mouseArea.containsMouse ? "#E5E5E5" : "transparent"
                z: -1
            }

            Rectangle {
                width: parent.width
                height: 1
                anchors.bottom: parent.bottom
                color: "#F0F0F0"
            }
        }

        // 空状态与错误状态
        Item {
            anchors.fill: parent
            visible: sessionListView.count === 0 && !root.isLoading

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 16
                visible: !root.isError

                Text {
                    text: "💬"
                    font.pixelSize: 48
                    Layout.alignment: Qt.AlignHCenter
                }
                Text {
                    text: qsTr("暂无会话")
                    color: "#999999"
                    font.pixelSize: 16
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 16
                visible: root.isError

                Text {
                    text: "⚠️"
                    font.pixelSize: 48
                    Layout.alignment: Qt.AlignHCenter
                }
                Text {
                    text: root.errorMessage || qsTr("加载失败")
                    color: "#D32F2F"
                    font.pixelSize: 16
                    Layout.alignment: Qt.AlignHCenter
                }
                Button {
                    text: qsTr("重试")
                    Layout.alignment: Qt.AlignHCenter
                    onClicked: {
                        ChatController.loadSessions()
                    }
                }
            }
        }
    }
}
