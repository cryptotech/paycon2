#include "concordpetitionmodel.h"
#include "main.h"
#include "guiutil.h"

ConcordPetitionModel::ConcordPetitionModel(QWidget *parent) :
    QAbstractTableModel(parent), petition(0)
{
    rowHeaders << tr("Height") << tr("TxID") << tr("Petition Hash") << tr("URL");
}

int ConcordPetitionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

int ConcordPetitionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ConcordPetitionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (!petition)
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.row())
        {
        case Height:
            return QString::number(petition->nHeight);
        case TxID:
            return QString::fromStdString(petition->txid.GetHex());
        case PetitionHash:
            return QString::fromStdString(petition->strHash);
        case URL:
            return QString::fromStdString(petition->strURL);
        }
        break;
    }
    return QVariant();
}

QVariant ConcordPetitionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Vertical)
    {
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return tr("Concord Petition");
            break;
        default:
            return QVariant();
            break;
        }
    }

    switch (role)
    {
    case Qt::DisplayRole:
        return rowHeaders[section];
        break;
    case Qt::ToolTipRole:
        switch (section)
        {
        case Height:
            return tr("Block height the petition was created at.");
        case TxID:
            return tr("Transaction the petition was created with.");
        case PetitionHash:
            return tr("Hash of the petition.");
        case URL:
            return tr("URL where the petition text may be found.");
        }
        break;
    }
    return QVariant();
}

Qt::ItemFlags ConcordPetitionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void ConcordPetitionModel::clear()
{
    beginResetModel();
    petition = 0;
    endResetModel();
}

void ConcordPetitionModel::setPetition(CConcord *newPetition)
{
    beginResetModel();
    petition = newPetition;
    endResetModel();
}
