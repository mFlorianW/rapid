// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#include <QComboBox>
#include <QMainWindow>
#include <common/qt/DeviceSettingsProvider.hpp>
#include <common/qt/GenericTableModel.hpp>
#include <common/qt/GlobalSettingsReader.hpp>
#include <rest/qt/QRestClient.hpp>
#include <storage/qt/SessionDatabaseIpcClient.hpp>
#include <storage/qt/SessionMetaDataProvider.hpp>
#include <workflow/qt/RestSessionManagementWorkflow.hpp>

namespace Ui
{
class SessionManager;
}

namespace Rapid::SessionManager
{

/**
 * @brief SessionManager application claas.
 * @details Brings up the main window make it possible to show and brings up the whole back-end.
 */
class SessionManager final : public QMainWindow
{
    Q_OBJECT
public:
    Q_DISABLE_COPY_MOVE(SessionManager);

    /**
     * @brief Creates an instance of @ref SessionManager
     *
     * @details Setups the main window and back-end
     *          There is only one instance of that class created in the main function.
     */
    SessionManager();

    /**
     * Default destructor
     */
    ~SessionManager() override;

private Q_SLOTS:
    void onDeleteHostSession();
    void onDeviceActivated(qsizetype index);
    void onDownloadSessionRequest();
    void onDownloadSessionRequestFinished(qsizetype index, Workflow::DownloadResult result);

private:
    void writeToDownloadLog(QString const& output);

private:
    std::unique_ptr<Ui::SessionManager> mSessionManager;
    std::unique_ptr<Storage::Qt::SessionDatabaseIpcClient> mSessionDatabase;

    std::unique_ptr<Common::Qt::GlobalSettingsBackend> mSettingsBackend;
    std::unique_ptr<Common::Qt::GlobalSettingsReader> mSettingsReader;

    // Host SessionModel
    using SessionModel = Common::Qt::GenericTableModel<Storage::Qt::SessionMetaDataProvider>;
    std::unique_ptr<Storage::Qt::SessionMetaDataProvider> mHostSessionMetaDataProvider;
    std::unique_ptr<SessionModel> mHostSessionModel;

    // Rest SessionModel
    std::unique_ptr<Rest::QRestClient> mRestClient;
    std::unique_ptr<Workflow::Qt::RestSessionManagementWorkflow> mRestSessionManagement;
    std::shared_ptr<Common::Qt::SessionMetadataProvider> mRestSessionMetadataProvider;
    using RestSessionModel = Common::Qt::GenericTableModel<Common::Qt::SessionMetadataProvider>;
    std::unique_ptr<RestSessionModel> mRestSessionModel;

    // Toolbar elements
    std::unique_ptr<Common::Qt::DeviceSettingsProvider> mDeviceSettingsProvider;
    using DeviceSettingsModel = Common::Qt::GenericTableModel<Common::Qt::DeviceSettingsProvider>;
    std::unique_ptr<DeviceSettingsModel> mDeviceSettingsModel;
    std::unique_ptr<QComboBox> mToolbarDeviceComboBox = std::make_unique<QComboBox>();
};

} // namespace Rapid::SessionManager

#endif //SESSIONMANAGER_HPP
