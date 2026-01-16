# ShirohaChat

[![Qt](https://img.shields.io/badge/Qt-6.8+-41CD52?logo=qt)](https://www.qt.io/)
[![C++](https://img.shields.io/badge/C++-20-00599C?logo=cplusplus)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.21+-064F8C?logo=cmake)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

一款面向大众的普适桌面端即时通讯工具，基于 Qt 6 / QML 构建，聚焦日常社交场景，提供私聊/群聊与多媒体表达能力。

## 项目概述

ShirohaChat 是一个现代化的桌面即时通讯客户端，旨在提供：

- **低延迟通信**：端到端延迟 ≤ 200ms
- **高可靠性**：消息送达率 ≥ 99.99%，零丢失
- **完整聊天体验**：支持私聊、群聊、文本、图片、语音、表情
- **离线消息**：断线重连与离线消息自动拉取
- **本地存储**：SQLite 数据库持久化聊天记录
- **WebSocket 实时通信**：基于自定义协议的消息传递

### 已实现功能

- WebSocket 连接与断线重连
- 私聊会话管理
- 文本消息发送与接收
- Server-ACK 确认机制
- 消息状态追踪（发送中/已送达/失败）
- 本地数据库持久化（SQLite）
- 会话列表与消息列表模型
- 待发送消息恢复机制

## 技术栈

### 核心技术

- **C++20**：现代 C++ 标准，使用面向对象设计
- **Qt 6.8+**：跨平台 GUI 框架
  - Qt Quick / QML：声明式 UI 开发
  - Qt WebSockets：实时通信
  - Qt SQL：数据库访问
- **SQLite3**：本地数据持久化
- **CMake 3.21+**：构建系统

### 架构特点

- **QML_SINGLETON 模式**：服务层单例管理
- **MVC 架构**：清晰的业务逻辑分层
- **Doxygen 文档**：完整的代码注释规范
- **代码质量工具**：集成 clang-format 和 clang-tidy

## 项目结构

```
ShirohaChat/
├── src/
│   ├── app/                    # 应用层
│   │   ├── main.cpp           # 程序入口
│   │   ├── chat_controller.*  # 核心业务控制器（单例）
│   │   ├── network_service.*  # WebSocket 网络服务（单例）
│   │   ├── local_db.*         # 本地数据库服务（单例）
│   │   └── config.h           # 配置常量
│   ├── domain/                 # 领域层
│   │   ├── message.*          # 消息实体
│   │   ├── session.*          # 会话实体
│   │   └── user.*             # 用户实体
│   └── models/                 # 模型层
│       ├── message_list_model.*   # 消息列表模型
│       └── session_list_model.*   # 会话列表模型
├── qml/
│   ├── Main.qml               # 主窗口
│   ├── components/            # UI 组件
│   │   ├── Avatar.qml         # 头像组件
│   │   ├── InputBar.qml       # 输入框组件
│   │   └── MessageBubble.qml  # 消息气泡组件
│   └── views/                 # 视图
│       ├── ChatView.qml       # 聊天视图
│       └── SessionSidebar.qml # 会话侧边栏
├── docs/                       # 项目文档
├── test_server.py             # Python WebSocket 测试服务器
├── CMakeLists.txt             # 根 CMake 配置
├── .clang-format              # 代码格式化配置
└── .clang-tidy                # 静态检查配置
```

## 快速开始

### 前置依赖

- **CMake** >= 3.21
- **C++20 编译器**：
  - GCC 11+
  - Clang 13+
  - MSVC 2019+
- **Qt 6.8+** 模块：
  - Core
  - Gui
  - Qml
  - Quick
  - QuickControls2
  - WebSockets
  - Sql
- **SQLite** 驱动插件：QSQLITE

### 安装 Qt

确保 Qt 安装包含以下组件：

```bash
# 使用 Qt Online Installer 或包管理器安装
# Debian/Ubuntu
sudo apt install qt6-base-dev qt6-declarative-dev qt6-websockets-dev

# Manjaro/Arch Linux
sudo pacman -S qt6-base qt6-declarative qt6-websockets qt6-svg

# macOS (Homebrew)
brew install qt@6

# Windows
# 下载 Qt Online Installer: https://www.qt.io/download-qt-installer
```

### 构建步骤

```bash
# 1. 克隆仓库
git clone https://github.com/yourusername/ShirohaChat.git
cd ShirohaChat

# 2. 配置构建
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# 3. 编译
cmake --build build -j$(nproc)

# 4. 运行客户端
./build/src/app/shirohachat
```

### 启动测试服务器

项目提供了一个 Python WebSocket 测试服务器用于开发：

```bash
# 安装依赖
pip install websockets

# 启动服务器（默认端口 8765）
python test_server.py
```

服务器支持的功能：
- `send_message` + `send_message_ack`
- `heartbeat` + `heartbeat_ack`
- `receive_message` 广播
- msgId 幂等性

## 开发指南

### 代码质量工具

项目集成了 `clang-format` 和 `clang-tidy`，确保代码质量和一致性。

#### 格式化代码

```bash
# 格式化所有 C++ 源码（遵循 .clang-format）
cmake --build build --target format

# 手动格式化单个文件
clang-format -i src/app/chat_controller.cpp
```

#### 静态检查

```bash
# 运行 clang-tidy（基于 compile_commands.json）
cmake --build build --target tidy

# 同时运行 tidy 和 format
cmake --build build --target ci-lint
```

#### 配置选项

```bash
# 关闭编译期 clang-tidy（如果未安装）
cmake -S . -B build -DENABLE_CLANG_TIDY=OFF
```

### 架构说明

#### 单例服务

项目使用 `QML_SINGLETON` 宏定义三个核心单例：

1. **ChatController**：业务控制器，协调网络与数据库
2. **NetworkService**：WebSocket 通信服务
3. **LocalDB**：SQLite 数据库服务

```cpp
class ChatController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    static ChatController& instance();
    static ChatController* create(QQmlEngine*, QJSEngine*);
    // ...
};
```

#### 数据模型

使用 `QAbstractListModel` 实现响应式数据绑定：

- **SessionListModel**：会话列表（左侧边栏）
- **MessageListModel**：消息列表（聊天窗口）

#### 协议设计

WebSocket 消息使用 JSON 格式：

```json
{
  "cmd": "send_message",
  "msgId": "uuid-1234",
  "from": "user123",
  "to": "user456",
  "content": "Hello!",
  "timestamp": "2026-01-16T12:00:00Z"
}
```

## 测试

```bash
# 运行单元测试（待实现）
cmake --build build --target test

# 启动测试服务器
python test_server.py
```

## 文档

- [ShirohaChat 项目文档](docs/ShirohaChat项目文档.md)：完整的需求分析与架构设计
- [贡献指南](CONTRIBUTING.md)：如何参与开发

## 贡献

欢迎贡献！请遵循以下步骤：

1. Fork 本仓库
2. 创建特性分支（`git checkout -b feature/amazing-feature`）
3. 提交更改（`git commit -m 'Add amazing feature'`）
4. 推送到分支（`git push origin feature/amazing-feature`）
5. 提交 Pull Request

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 相关链接

- [Qt 官方文档](https://doc.qt.io/)
- [Qt Quick 指南](https://doc.qt.io/qt-6/qtquick-index.html)
- [WebSocket 协议](https://datatracker.ietf.org/doc/html/rfc6455)

---
