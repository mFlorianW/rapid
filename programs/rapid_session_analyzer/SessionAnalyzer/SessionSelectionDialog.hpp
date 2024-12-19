// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SESSIONSELECTIONDIALOG_HPP
#define SESSIONSELECTIONDIALOG_HPP

#include <GenericTableModel.hpp>
#include <ISessionDatabase.hpp>
#include <QDialog>
#include <QHeaderView>
#include <SessionMetaDataProvider.hpp>
#include <memory>

namespace Ui
{
class SessionSelectionDialog;
}

namespace Rapid::SessionAnalyzer
{

class SessionSelectionDialog : public QDialog
{
    Q_OBJECT
public:
    SessionSelectionDialog(Storage::ISessionDatabase& sessionDatabase);
    Q_DISABLE_COPY_MOVE(SessionSelectionDialog)
    ~SessionSelectionDialog() override;

Q_SIGNALS:
    void sessionSelected(Rapid::Common::SessionMetaData const& session);

private Q_SLOTS:
    void onSessionSelected(QModelIndex const& index);
    void onSessionSelected();
    void selectSession(qsizetype index);

private:
    std::unique_ptr<Ui::SessionSelectionDialog> mSelectionDialogUi;
    Storage::ISessionDatabase& mSessionDatabase;
    std::unique_ptr<Storage::Qt::SessionMetaDataProvider> mSessionMetaDataProvider;
    std::unique_ptr<Common::Qt::GenericTableModel<Storage::Qt::SessionMetaDataProvider>> mSessionSelectionModel;
    QHeaderView mSessionSelectionModelHeaderView{Qt::Horizontal};
};

} // namespace Rapid::SessionAnalyzer

#endif // !SESSIONSELECTIONDIALOG_HPP
