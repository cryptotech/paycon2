#include "concordpage.h"
#include "ui_concordpage.h"
#include "openssl/sha.h"
#include "conspeech.h"
#include "main.h"
#include "util.h"
#include "walletmodel.h"
#include "concordpetitionmodel.h"
#include "concordsupportmodel.h"

#include <QDebug>
#include <QMessageBox>
#include <QMenu>

ConcordPage::ConcordPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConcordPage),
    model(0)
{
    ui->setupUi(this);
    ui->createPetitionButton->setEnabled(false);
    ui->setVoteCheckBox->setEnabled(false);

    petitionModel = new ConcordPetitionModel(this);
    ui->searchConcordView->setModel(petitionModel);

    // Context menu for petition support view
    QAction *searchPetitionIDAction = new QAction(tr("Search for petition"), this);
    petitionViewContextMenu = new QMenu();
    petitionViewContextMenu->addAction(searchPetitionIDAction);
    connect(searchPetitionIDAction, SIGNAL(triggered()), this, SLOT(searchHighlightedPetition()));

    supportModel = new ConcordSupportModel(this);
    ui->petitionSupportView->setModel(supportModel);
    connect(ui->petitionSupportView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(searchHighlightedPetition()));
}

ConcordPage::~ConcordPage()
{
    delete ui;
}

// Calculate notary ID when text changes.
void ConcordPage::on_createPetitionEdit_textChanged()
{
    std::string petitionText(ui->createPetitionEdit->toPlainText().toStdString());
    if (petitionText.length() == 0)
    {
        ui->petitionIDEdit->clear();
        ui->createPetitionButton->setEnabled(false);
        ui->setVoteCheckBox->setEnabled(false);
        return;
    }
    ui->createPetitionButton->setEnabled(true);
    ui->setVoteCheckBox->setEnabled(true);
    std::string petitionHash(StrToSHA256(petitionText));
    ui->petitionIDEdit->setText(QString::fromStdString(petitionHash));
}

// Create a tx that creates a petitition
void ConcordPage::on_createPetitionButton_clicked()
{
    std::string petitionHash(ui->petitionIDEdit->text().toStdString());

    WalletModel::UnlockContext ctx(model->requestUnlock());
    if (!ctx.isValid()) {
        return;
    }

    model->sendConcordTx(petitionHash);

    if (ui->setVoteCheckBox->isChecked())
    {
        strDefaultStakeSpeech = "concord " + petitionHash.substr(0, 8);
        concordConSpeech.push_back(strDefaultStakeSpeech);
        qDebug() << "saving concord petitions";
        if ( !SaveConcordConSpeech() )
            qDebug() << "Concord CONSpeech petitions FAILED to save!";
        loadVotes();
    }
}

void ConcordPage::on_setVotesButton_clicked()
{
    saveVotes();
}

void ConcordPage::loadVotes()
{
    QStringList list;
    for (std::vector<std::string>::iterator it = concordConSpeech.begin(); it != concordConSpeech.end(); ++it)
    {
        list.append(QString::fromStdString(*it).mid(8));
    }
    ui->votesEdit->setPlainText(list.join("\n"));
}

void ConcordPage::saveVotes()
{
    QStringList list = ui->votesEdit->toPlainText().replace("\n", ",").replace(" ", ",").split(',', QString::SkipEmptyParts);
    std::vector<std::string> newSpeeches;
    concordConSpeech.clear();

    if (list.length() > 0)
    {
        newSpeeches.push_back("concord");
        foreach ( const QString &strLine, list )
            if ( !strLine.isEmpty() && strLine.length() >= 8 && IsHex(strLine.toStdString()) )
            {
                // Create new string if necessary
                if (newSpeeches.back().length() > MAX_TX_COMMENT_LEN - 9)
                {
                    newSpeeches.push_back("concord");
                }
                std::string &newSpeech = newSpeeches.back();
                newSpeech = newSpeech + " " + strLine.trimmed().left(8).toStdString();
            }


        for (std::vector<std::string>::iterator it = newSpeeches.begin(); it != newSpeeches.end(); ++it)
        {
            concordConSpeech.push_back(*it);
        }
    }

    // save new speech
    qDebug() << "saving concord petitions";
    if ( !SaveConcordConSpeech() )
        qDebug() << "Concord CONSpeech petitions FAILED to save!";

    loadVotes();
}

void ConcordPage::showConcordTxResult(std::string txID, std::string txError)
{
    if (txError == "") {
        std::string txSentMsg = "Concord petition created successfully: " + txID;
        QMessageBox::information(this, tr("Create Concord Petition"),
            tr(txSentMsg.c_str()),
            QMessageBox::Ok, QMessageBox::Ok);
        ui->createPetitionButton->setEnabled(false);
        ui->setVoteCheckBox->setEnabled(false);
    } else {
        QMessageBox::warning(this, tr("Create Concord Petition"),
            tr(txError.c_str()),
            QMessageBox::Ok, QMessageBox::Ok);
    }
}

void ConcordPage::setConcordSearchResults(CConcord *pResult)
{
    if (!pResult)
    {
        LogPrintf("No concord results.\n");
        QMessageBox::warning(this, tr("Concord Search"),
            tr("No concord petition found."),
            QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    petitionModel->setPetition(pResult);
}

bool petitionPairSort(std::pair<std::string, int> i, std::pair<std::string, int> j) { return i.second > j.second; }

void ConcordPage::showPetitionSupport(std::map<string, int> mapSupport)
{
    std::vector<std::pair<std::string, int> > support;
    for (std::map<std::string, int>::iterator it = mapSupport.begin(); it != mapSupport.end(); ++it) {
        std::pair<std::string, int> petitionSupport = std::make_pair(it->first, it->second);
        support.push_back(petitionSupport);
    }
    std::sort(support.begin(), support.end(), petitionPairSort);
    supportModel->setSupport(support);
}

void ConcordPage::setModel(WalletModel *model)
{
    this->model = model;
    connect(this->model, SIGNAL(concordTxSent(std::string, std::string)), this, SLOT(showConcordTxResult(std::string, std::string)));
    connect(this->model, SIGNAL(concordSearchComplete(CConcord*)), this, SLOT(setConcordSearchResults(CConcord*)));
    connect(this->model, SIGNAL(petitionSupportRetrieved(std::map<std::string,int>)), this, SLOT(showPetitionSupport(std::map<std::string,int>)));
    loadVotes();
}

void ConcordPage::on_searchConcordButton_clicked()
{
    std::string pid(ui->searchConcordEdit->text().toStdString());
    if (!(IsHex(pid) && pid.length() == 8)) {
        ui->searchConcordEdit->setValid(false);
        return;
    }
    petitionModel->clear();
    model->searchConcords(pid);
}


void ConcordPage::on_getPetitionSupportButton_clicked()
{
    int nWindow = ui->petitionWindowSpinbox->value();
    model->getPetitionSupport(nWindow);
}

void ConcordPage::on_petitionSupportView_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->petitionSupportView->indexAt(pos);
    if (index.isValid())
        petitionViewContextMenu->exec(QCursor::pos());
}

void ConcordPage::searchHighlightedPetition()
{
    QModelIndexList indexes = ui->petitionSupportView->selectionModel()->selectedIndexes();
    if (indexes.size() >= 1)
    {
        QModelIndex pidIndex = indexes.at(0);
        QString pid = ui->petitionSupportView->model()->data(pidIndex).toString();
        ui->searchConcordEdit->setText(pid);
        ui->tabWidget->setCurrentIndex(1);
        ui->searchConcordButton->animateClick();
    }
}
