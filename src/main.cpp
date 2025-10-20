#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QDebug>

#include "videohandler.h"
#include "connectionmanager.h"
#include "configmanager.h"
#include "messagelogger.h"

int main(int argc, char *argv[])
{
    // 启用高 DPI 支持
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    // 设置应用程序信息
    app.setOrganizationName("ArdKit");
    app.setOrganizationDomain("ardkit.com");
    app.setApplicationName("ArdKit-GUI");
    app.setApplicationVersion("1.0.0");

    // 设置应用程序图标
    app.setWindowIcon(QIcon(":/resources/icons/app-icon.png"));

    qDebug() << "Starting ArdKit-GUI...";

    // 创建后端对象
    VideoHandler videoHandler;
    ConnectionManager connectionManager;
    ConfigManager configManager;
    MessageLogger messageLogger;

    // 设置日志的最大行数从配置读取
    messageLogger.setMaxLines(configManager.maxLogLines());

    // 连接信号：连接状态变化时记录日志
    QObject::connect(&connectionManager, &ConnectionManager::connectionStatusChanged,
                     &messageLogger, &MessageLogger::addInfoMessage);

    // 连接信号：连接管理器接收到数据时记录日志
    QObject::connect(&connectionManager, &ConnectionManager::dataReceived,
                     &messageLogger, &MessageLogger::addMessage);

    // 连接信号：视频处理错误时记录日志
    QObject::connect(&videoHandler, &VideoHandler::errorOccurred,
                     &messageLogger, &MessageLogger::addErrorMessage);

    // 连接信号：连接管理器错误时记录日志
    QObject::connect(&connectionManager, &ConnectionManager::errorOccurred,
                     &messageLogger, &MessageLogger::addErrorMessage);

    // 创建 QML 引擎
    QQmlApplicationEngine engine;

    // 注册 C++ 类型到 QML
    qmlRegisterType<VideoHandler>("ArdKitGUI", 1, 0, "VideoHandler");
    qmlRegisterType<ConnectionManager>("ArdKitGUI", 1, 0, "ConnectionManager");
    qmlRegisterType<ConfigManager>("ArdKitGUI", 1, 0, "ConfigManager");
    qmlRegisterType<MessageLogger>("ArdKitGUI", 1, 0, "MessageLogger");

    // 将后端对象暴露给 QML
    engine.rootContext()->setContextProperty("videoHandler", &videoHandler);
    engine.rootContext()->setContextProperty("connectionManager", &connectionManager);
    engine.rootContext()->setContextProperty("configManager", &configManager);
    engine.rootContext()->setContextProperty("messageLogger", &messageLogger);

    // 加载 QML 主文件
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            qCritical() << "Failed to load QML file:" << url;
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "No root objects created";
        return -1;
    }

    qDebug() << "ArdKit-GUI started successfully";

    // 添加欢迎消息
    messageLogger.addInfoMessage("欢迎使用 ArdKit-GUI");
    messageLogger.addInfoMessage("请点击连接按钮连接到设备");

    return app.exec();
}
