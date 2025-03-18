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
     * @brief Creates an instance of the @ref Rapid::Workflow::Qt::RestSessionManagementWorkflow
     *
     * @details This constructor creates an non functional instance.
     *          Before the class can be used an @ref Rapid::Rest::IRestClient must be configured.
     */
    RestSessionManagementWorkflow();
    /**
     * @brief Creates an instance of the @ref RestSessionDownloader
     *
     * @param client The REST client that is used for the device communication.
     *               The REST client must have the same life time as the this instance.
     */
    explicit RestSessionManagementWorkflow(Rest::IRestClient* client);
    /**
     * Default destructor
     */
    ~RestSessionManagementWorkflow() override;
    /**
     * Disabled copy constructor
     */
    RestSessionManagementWorkflow(RestSessionManagementWorkflow const&) = delete;
    RestSessionManagementWorkflow(RestSessionManagementWorkflow&&) noexcept = delete;
    /**
     * Disabled copy operator
     */
    RestSessionManagementWorkflow& operator=(RestSessionManagementWorkflow const&) = delete;
    RestSessionManagementWorkflow& operator=(RestSessionManagementWorkflow&&) noexcept = delete;
    /**
     * Disabled move constructor
     */
    /**
     * Disabled move operator
     */
    /**
     * @brief Gives the @ref Common::SessionMetadataProvider for the configured device.
     *
     * @details The provivder is automatically updated when a @ref Common::SessionMetadata are requeted.
     *          On creation the return @ref SessionMetadataProvider is empty.
     *
     * @return A provider for usage in a @ref Common::GenericTableModel
     */
    std::shared_ptr<Common::Qt::SessionMetadataProvider> getProvider() const noexcept;
private
:
    void onSessionMetadataDownloaded(std::size_t index, DownloadResult result);
private
:
    std::shared_ptr<Common::Qt::SessionMetadataProvider> mProvider;
    KDBindings::ScopedConnection mSessionMetadataDownloadConnection;
};

} // namespace Rapid::Workflow::Qt

#endif // WORKFLOW_QT_RESTSESSIONDOWNLOADER_HPP
