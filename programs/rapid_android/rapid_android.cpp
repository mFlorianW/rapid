// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include <Database.hpp>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <spdlog/sinks/android_sink.h>
#include <spdlog/spdlog.h>
#include <storage/SqliteSessionDatabase.hpp>
#include <system/Logger.hpp>
#include <system/qt/Logger.hpp>
#include <system/qt/RapidApplication.hpp>

namespace Rapid::Android
{
void setupLogger()
{
#ifdef ANDROID
    auto android_sink = std::make_shared<spdlog::sinks::android_sink_mt>("rapid_android");
    auto logger = std::make_shared<spdlog::logger>("rapid_android", android_sink);
    logger->set_level(spdlog::level::debug);
    spdlog::set_default_logger(logger);
    Rapid::System::Logger::setDefaultLogger(logger);
    Rapid::System::Qt::Logger::setDefaultLogger(logger);
    SPDLOG_INFO("Succcesful setup SDPLOG android logger");
#endif // ENABLE_ANDROID
}
} // namespace Rapid::Android

int main(int argc, char** argv)
{
    Rapid::Android::setupLogger();
    Rapid::Android::setupDatabase();
    Rapid::Android::setupDatabaseFilePermissions();

    auto app = Rapid::System::Qt::RapidGuiApplication{argc, argv};

    QQuickStyle::setStyle("Material");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() {
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(QUrl{"qrc:/qt/qml/Rapid/Android/qml/RapidAndroid.qml"});

    return app.exec();
}
