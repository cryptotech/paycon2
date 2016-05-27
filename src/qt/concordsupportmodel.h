#ifndef CONCORDSUPPORTMODEL_H
#define CONCORDSUPPORTMODEL_H

#include <QWidget>
#include <QAbstractTableModel>
#include <QStringList>

class ConcordSupportModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ConcordSupportModel(QWidget *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void clear();
    void setSupport(std::vector<std::pair<std::string, int> > newSupport);

signals:

public slots:

private:
    QStringList columns;
    std::vector<std::pair<std::string, int> > support;
};

#endif // CONCORDSUPPORTMODEL_H
