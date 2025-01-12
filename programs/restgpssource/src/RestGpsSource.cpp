// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "RestGpsSource.hpp"
#include "MainWindowViewModel.hpp"
#include "system/EventLoop.hpp"
#include <QQmlContext>
#include <QQuickWindow>
#include <QTimer>
#include <system/Timer.hpp>

struct RestGpsSourcePrivate
{
    QQmlApplicationEngine mEngine;
    MainWindowViewModel mMainWindowViewModel;
    QTimer mRapidTimer;
};

RestGpsSource::RestGpsSource()
    : d{std::make_unique<RestGpsSourcePrivate>()}
{
    auto qmlContext = d->mEngine.rootContext();
    qmlContext->setContextProperty("g_MainWindowViewModel", &d->mMainWindowViewModel);

    d->mEngine.load(QUrl{"qrc:/qml/MainWindow.qml"});

    // TODO: move this in somehow in the mainloop
    d->mRapidTimer.setInterval(std::chrono::milliseconds(1));
    QObject::connect(&d->mRapidTimer, &QTimer::timeout, &d->mRapidTimer, []() {
        Rapid::System::EventLoop::instance().processEvents();
    });
    d->mRapidTimer.start();
}

RestGpsSource::~RestGpsSource() = default;

void RestGpsSource::show()
{
    auto* topLevel = d->mEngine.rootObjects().value(0);
    auto* window = qobject_cast<QQuickWindow*>(topLevel);
    window->show();
}
