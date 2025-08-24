#ifndef ASSESSOR_MODEL_H
#define ASSESSOR_MODEL_H

#include <QAbstractTableModel>
#include <vector>
#include "../../include/core/Assessor.h"

using namespace SilverClinic;

class AssessorModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit AssessorModel(QObject* parent = nullptr);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Helpers
    void setAssessors(const std::vector<Assessor>& list);
    std::optional<Assessor> assessorAt(int row) const;
    void addAssessor(const Assessor& a);
    void updateAssessor(int row, const Assessor& a);
    void removeAssessor(int row);

private:
    std::vector<Assessor> m_assessors;
};

#endif // ASSESSOR_MODEL_H
