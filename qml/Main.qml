import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import ShirohaChat

ApplicationWindow {
    id: window
    width: 960
    height: 640
    visible: true
    title: qsTr("ShirohaChat")

    // 启动初始化
    Component.onCompleted: {
        // 打开本地数据库
        LocalDB.open("shirohachat.db")

        // 连接到 WebSocket 服务器
        ChatController.connect("ws://localhost:8080", "user_1")

        // 恢复待发送消息
        ChatController.recoverPendingMessages()
    }

    // 经典分栏布局
    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        // 左侧：会话列表侧边栏
        SessionSidebar {
            SplitView.preferredWidth: 250
            SplitView.minimumWidth: 200
            SplitView.maximumWidth: 400
        }

        // 右侧：聊天主视图
        ChatView {
            SplitView.fillWidth: true
        }
    }
}
