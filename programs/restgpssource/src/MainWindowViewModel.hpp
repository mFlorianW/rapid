// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef MAINWINDOWVIEWMODEL_HPP
#define MAINWINDOWVIEWMODEL_HPP

#include <QAbstractListModel>
#include <QGeoCoordinate>
#include <QObject>
#include <memory>

struct MainWindowViewModelPrivate;
class MainWindowViewModel : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MainWindowViewModel)

    Q_PROPERTY(QAbstractItemModel* positionModel READ getPositionModel CONSTANT)

    Q_PROPERTY(QGeoCoordinate currentPosition READ getCurrentPosition NOTIFY currentPositionChanged)

    Q_PROPERTY(bool gpsSourceActive READ isGpsSourceActive NOTIFY gpsSourceActiveChanged)

    Q_PROPERTY(QString hostAddress READ getHostAddress WRITE setHostAddress NOTIFY hostAddressChanged)

    Q_PROPERTY(QString hostPort READ getHostPort WRITE setHostPort NOTIFY hostPortChanged)
public:
    MainWindowViewModel();
    ~MainWindowViewModel() override;

    QAbstractItemModel* getPositionModel() const noexcept;

    Q_INVOKABLE void startGpsSource();
    Q_INVOKABLE void stopGpsSource();
    Q_INVOKABLE void loadGpsFile(QUrl const& fileName);

    [[nodiscard]] QGeoCoordinate getCurrentPosition() const noexcept;

    [[nodiscard]] bool isGpsSourceActive() const noexcept;

    [[nodiscard]] QString getHostAddress() const noexcept;
    void setHostAddress(QString const& hostAddress);

    [[nodiscard]] QString getHostPort() const noexcept;
    void setHostPort(QString const& hostPort);

Q_SIGNALS:
    void currentPositionChanged();
    void gpsSourceActiveChanged();
    void hostAddressChanged();
    void hostPortChanged();

private:
    void handlePositionUpdate();
    void updateUrl();

private:
    std::unique_ptr<MainWindowViewModelPrivate> d;
};
#endif // MAINWINDOWVIEWMODEL_HPP
