#include "assessor_model.h"
#include <QVariant>

AssessorModel::AssessorModel(QObject* parent) : QAbstractTableModel(parent) {}

int AssessorModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return static_cast<int>(m_assessors.size());
}

int AssessorModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 5; // id, first, last, email, phone
}

QVariant AssessorModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) return {};
    const Assessor &a = m_assessors.at(index.row());
    switch (index.column()) {
        case 0: return a.getAssessorId();
        case 1: return QString::fromStdString(a.getFirstName());
        case 2: return QString::fromStdString(a.getLastName());
        case 3: return QString::fromStdString(a.getEmail());
        case 4: return QString::fromStdString(a.getPhone());
        default: return {};
    }
}

QVariant AssessorModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return {};
    switch (section) {
        case 0: return QString("ID");
        case 1: return QString("First");
        case 2: return QString("Last");
        case 3: return QString("Email");
        case 4: return QString("Phone");
        default: return {};
    }
}

Qt::ItemFlags AssessorModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void AssessorModel::setAssessors(const std::vector<Assessor>& list) {
    beginResetModel();
    m_assessors = list;
    endResetModel();
}

std::optional<Assessor> AssessorModel::assessorAt(int row) const {
    if (row < 0 || row >= static_cast<int>(m_assessors.size())) return std::nullopt;
    return m_assessors.at(row);
}

void AssessorModel::addAssessor(const Assessor& a) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_assessors.push_back(a);
    endInsertRows();
}

void AssessorModel::updateAssessor(int row, const Assessor& a) {
    if (row < 0 || row >= rowCount()) return;
    m_assessors[row] = a;
    emit dataChanged(index(row,0), index(row, columnCount()-1));
}

void AssessorModel::removeAssessor(int row) {
    if (row < 0 || row >= rowCount()) return;
    beginRemoveRows(QModelIndex(), row, row);
    m_assessors.erase(m_assessors.begin() + row);
    endRemoveRows();
}
