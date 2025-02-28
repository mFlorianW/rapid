// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef RAPID_WORKFLOW_QT_LOCALSESSIONMANAGEMENT_HPP
#define RAPID_WORKFLOW_QT_LOCALSESSIONMANAGEMENT_HPP

#include <QObject>
#include <QVariant>
#include <common/qt/SessionMetaDataListModel.hpp>
#include <storage/ISessionDatabase.hpp>
#include <workflow/LocalSessionManagement.hpp>

namespace Rapid::Workflow::Qt
{

/**
 * Extends the local session management for Qt usage
 */
class LocalSessionManagement : public QObject, public Workflow::LocalSessionManagement
{
    Q_OBJECT

    /**
     * @property Rapid::Common::Qt::SessionMetaDataListModel*
     *
     * @brief Gives a list model with the local stored session meta data.
     *
     * @details The model is automatically updated when the ISessionDatabase announce changes.
     */
    Q_PROPERTY(Rapid::Common::Qt::SessionMetaDataListModel* listModel READ getSessionMetaDataListModel CONSTANT)
public:
    Q_DISABLE_COPY_MOVE(LocalSessionManagement)

    /**
     * @brief Creates an instance @ref Rapid::Workflow::Qt::LocalSessionManagement instance
     *
     * @details The @ref Rapid::Workflow::Qt::LocalSessionManagement doesn't take the ownership of the database instance pointer.
     *          So the database instance must live as long as the @ref Rapid::Workflow::Qt::LocalSessionManagement.
     *
     * @param db The session database that shall be used for querying the local sessions.
     */
    [[nodiscard]] LocalSessionManagement(Storage::ISessionDatabase* db);

    /** @cond Doxygen_Suppress */
    ~LocalSessionManagement() override;
    /** @endcond Doxygen_Suppress */

    /**
     * @brief Gives the @ref Rapid::Common::Qt::SessionMetaDataListModel
     *
     * @details The caller doesn't take the ownership of the pointer the instance is automatically freed when the lifetime of @ref Rapid::Workflow::Qt::LocalSessionManagement ends.
     *          The model is updated by the @ref Rapid::Common::Qt::LocalSessionManagement when the database announce changes.
     *
     * @return A list model with the local sesion meta data.
     */
    [[nodiscard]] Rapid::Common::Qt::SessionMetaDataListModel* getSessionMetaDataListModel() const noexcept;

private:
    void handleSessionMetaDataResult(System::AsyncResult* result);

    struct PendingResult
    {
        std::size_t index = 0U;
        std::shared_ptr<Storage::GetSessionMetaDataResult> result;
        KDBindings::ScopedConnection doneConnection;
    };

    std::unique_ptr<Common::Qt::SessionMetaDataListModel> mMetaDataListModel{nullptr};
    std::unordered_map<System::AsyncResult*, PendingResult> mPendingResults;
};

} // namespace Rapid::Workflow::Qt

#endif // !RAPID_WORKFLOW_QT_LOCALSESSIONMANAGEMENT_HPP
