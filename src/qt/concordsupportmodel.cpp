#include "concordsupportmodel.h"

#include "guiutil.h"

ConcordSupportModel::ConcordSupportModel(QWidget *parent) :
    QAbstractTableModel(parent)
{
    columns << tr("Petition ID") << tr("Support");
}

int ConcordSupportModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return support.size();
}

int ConcordSupportModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant ConcordSupportModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    std::pair<std::string, int> petition = support.at(index.row());
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
        case 0:
            return QString::fromStdString(petition.first);
        case 1:
            return petition.second;
        }
    }
    return QVariant();
}

QVariant ConcordSupportModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
        return QVariant();
    if (role == Qt::DisplayRole)
    {
        return columns.at(section);
    }
    return QVariant();
}

Qt::ItemFlags ConcordSupportModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void ConcordSupportModel::clear()
{
    beginResetModel();
    support.clear();
    endResetModel();
}

void ConcordSupportModel::setSupport(std::vector<std::pair<std::string, int> > newSupport)
{
    beginResetModel();
    support = newSupport;
    endResetModel();
}
