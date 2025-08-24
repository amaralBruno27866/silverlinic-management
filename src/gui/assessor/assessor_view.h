#ifndef ASSESSOR_VIEW_H
#define ASSESSOR_VIEW_H

#include <QWidget>
#include <memory>

class QTableView;
class QPushButton;
class QLabel;
class AssessorModel;
class QComboBox;
class QLineEdit;
namespace SilverClinic { class AssessorManager; }

class AssessorView : public QWidget {
    Q_OBJECT
public:
    explicit AssessorView(SilverClinic::AssessorManager* manager = nullptr, QWidget* parent = nullptr);

private slots:
    void onAdd();
    void onEdit();
    void onDelete();
    void onImport();
    void onRowDoubleClicked(const QModelIndex &index);
    void onFilterChanged(int index);
    void onSearchTextChanged(const QString &text);

private:
    QTableView* m_table;
    QLabel* m_placeholder;
    QPushButton* m_add;
    QPushButton* m_edit;
    QPushButton* m_delete;
    QPushButton* m_import;
    AssessorModel* m_model;
    SilverClinic::AssessorManager* m_manager;
    // filter/search controls
    QComboBox* m_filterCombo;
    QLineEdit* m_searchField;
};

#endif // ASSESSOR_VIEW_H
