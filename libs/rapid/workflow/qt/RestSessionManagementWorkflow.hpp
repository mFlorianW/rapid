// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef WORKFLOW_QT_RESTSESSIONDOWNLOADER_HPP
#define WORKFLOW_QT_RESTSESSIONDOWNLOADER_HPP

#include <common/qt/SessionMetadataProvider.hpp>
#include <workflow/RestSessionManagementWorkflow.hpp>

namespace Rapid::Workflow::Qt
{

class RestSessionManagementWorkflow : public Workflow::RestSessionManagementWorkflow
{
public:
    /**
     * Creates an instance of the @ref RestSessionDownloader
     * @param client The REST client that is used for the device communication.
     */
    explicit RestSessionManagementWorkflow(Rest::IRestClient& client);

    /**
     * Default destructor
     */
    ~RestSessionManagementWorkflow() override;

    /**
     * Disabled copy constructor
     */
    RestSessionManagementWorkflow(RestSessionManagementWorkflow const&) = delete;

    /**
     * Disabled copy operator
     */
    RestSessionManagementWorkflow& operator=(RestSessionManagementWorkflow const&) = delete;

    /**
     * Disabled move constructor
     */
    RestSessionManagementWorkflow(RestSessionManagementWorkflow&&) noexcept = delete;

    /**
     * Disabled move operator
     */
    RestSessionManagementWorkflow& operator=(RestSessionManagementWorkflow&&) noexcept = delete;

    /**
     * @brief Gives the @ref Common::SessionMetadataProvider for the configured device.
     *
     * @details The provivder is automatically updated when a @ref Common::SessionMetadata are requeted.
     *          On creation the return @ref SessionMetadataProvider is empty.
     *
     * @return A provider for usage in a @ref Common::GenericTableModel
     */
    std::shared_ptr<Common::Qt::SessionMetadataProvider> getProvider() const noexcept;

private:
    void onSessionMetadataDownloaded(std::size_t index, DownloadResult result);

private:
    std::shared_ptr<Common::Qt::SessionMetadataProvider> mProvider;
};

} // namespace Rapid::Workflow::Qt

#endif // WORKFLOW_QT_RESTSESSIONDOWNLOADER_HPP
