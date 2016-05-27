#ifndef CONCORDPAGE_H
#define CONCORDPAGE_H

#include <map>

#include <QWidget>

namespace Ui {
class ConcordPage;
}
class WalletModel;
class CConcord;
class ConcordPetitionModel;
class ConcordSupportModel;

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

class ConcordPage : public QWidget
{
    Q_OBJECT

public:
    explicit ConcordPage(QWidget *parent = 0);
    ~ConcordPage();

    void setModel(WalletModel *model);

public slots:
    void showConcordTxResult(std::string txID, std::string txError);
    void setConcordSearchResults(CConcord *pResult);
    void showPetitionSupport(std::map<std::string, int> mapSupport);

private slots:
    void on_createPetitionEdit_textChanged();

    void on_createPetitionButton_clicked();

    void on_setVotesButton_clicked();

    void on_searchConcordButton_clicked();

    void on_getPetitionSupportButton_clicked();

    void on_petitionSupportView_customContextMenuRequested(const QPoint &pos);

    void searchHighlightedPetition();

private:
    Ui::ConcordPage *ui;
    WalletModel *model;
    ConcordPetitionModel *petitionModel;
    ConcordSupportModel *supportModel;
    QMenu *petitionViewContextMenu;

    void loadVotes();
    void saveVotes();
};

#endif // CONCORDPAGE_H
