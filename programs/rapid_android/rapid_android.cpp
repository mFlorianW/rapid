// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "GlobalState.hpp"
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <spdlog/sinks/android_sink.h>
#include <spdlog/spdlog.h>
#include <system/qt/RapidApplication.hpp>

void setupLogger()
{
#ifdef ENABLE_ANDROID
    auto android_sink = std::make_shared<spdlog::sinks::android_sink_mt>("rapid_android");
    auto logger = std::make_shared<spdlog::logger>("rapid_android", android_sink);
    logger->set_level(spdlog::level::debug);
    spdlog::set_default_logger(logger);
#endif // ENABLE_ANDROID
}

int main(int argc, char** argv)
{
    setupLogger();
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
    // engine.loadFromModule("Rapid.Android", "RapidAndroid");
    engine.singletonInstance<Rapid::Android::GlobalState*>("Rapid.Android", "GlobalState");
    engine.load(QUrl{"qrc:/Rapid/Android/qml/RapidAndroid.qml"});

    return app.exec();
}
