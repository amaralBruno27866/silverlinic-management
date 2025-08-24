#include "assessor_view.h"
#include "managers/AssessorManager.h"
#include <QMessageBox>
#include <QFileDialog>
#include "assessor_model.h"
#include "assessor_dialog.h"
#include <QTableView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QMessageBox>
#include <QHeaderView>
#include <QLabel>
#include <QStyle>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include "../../include/core/Address.h"
#include <QComboBox>
#include <QLineEdit>

AssessorView::AssessorView(SilverClinic::AssessorManager* manager, QWidget* parent) : QWidget(parent), m_manager(manager) {
    m_model = new AssessorModel(this);
    // If a manager is available, populate model from DB
    if (m_manager) {
        auto rows = m_manager->readAll();
        m_model->setAssessors(rows);
    }
    m_table = new QTableView(this);
    m_table->setModel(m_model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setAlternatingRowColors(true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Improve readability: ensure table text uses dark-gray and clean grid/background
    // Table styling: white rows, black text; selected rows use translucent blue; hover uses translucent gray
    m_table->setStyleSheet(
        "QTableView { color: #000000; background: #ffffff; gridline-color: #e6e6e6; font-size: 13px; }"
        "QTableView::item { padding: 6px; background: #ffffff; color: #000000; }"
        "QHeaderView::section { color: #333333; background: #f5f5f5; font-weight: bold; }"
        "QTableView::indicator { border: none; }"
        // Selected row: import-blue (#2a77fc) with 75% transparency => rgba(42,119,252,0.25)
        "QTableView::item:selected { background: rgba(42,119,252,0.25); color: #000000; }"
        "QTableView::item:selected:active { background: rgba(42,119,252,0.25); }"
        "QTableView::item:selected:!active { background: rgba(42,119,252,0.25); }"
        "QTableView { selection-background-color: rgba(42,119,252,0.25); }"
        // Hover: #333333 at 65% transparency => rgba(51,51,51,0.35)
        "QTableView::item:hover { background: rgba(51,51,51,0.35); }"
    );

    m_placeholder = new QLabel("Nenhum assessor encontrado.", this);
    m_placeholder->setAlignment(Qt::AlignCenter);
    m_placeholder->setStyleSheet("color: #777; font-size: 14px; padding:20px");

    m_add = new QPushButton("Add");
    m_edit = new QPushButton("Edit");
    m_delete = new QPushButton("Delete");
    m_import = new QPushButton("Import CSV");
    m_edit->setEnabled(false);
    m_delete->setEnabled(false);
    // Add button: purple with hover (30% darker)
    m_add->setStyleSheet(
        "QPushButton { background:#aa1bf7; color:white; padding:6px 12px; border-radius:6px; }"
        "QPushButton:hover { background:#7713ad; }"
    );
    // Edit button: confirm green with hover (30% darker)
    m_edit->setStyleSheet(
        "QPushButton { background:#3bf73b; color:white; padding:6px 12px; border-radius:6px; }"
        "QPushButton:hover { background:#29ad29; }"
    );
    // Delete button: red with hover (30% darker)
    m_delete->setStyleSheet(
        "QPushButton { background:#ff5555; color:white; padding:6px 12px; border-radius:6px; }"
        "QPushButton:hover { background:#b23c3c; }"
    );
    // Import CSV: success blue with hover (30% darker)
    m_import->setStyleSheet(
        "QPushButton { background:#2a77fc; color:white; padding:6px 12px; border-radius:6px; }"
        "QPushButton:hover { background:#1d53b0; }"
    );

    // Add filter and search controls
    m_filterCombo = new QComboBox(this);
    m_filterCombo->addItem("Sort by ID");
    m_filterCombo->addItem("Sort by First Name");
    m_filterCombo->addItem("Sort by Last Name");

    m_searchField = new QLineEdit(this);
    m_searchField->setPlaceholderText("Search by id, first, last, email or phone...");

    QHBoxLayout* toolbarLayout = new QHBoxLayout;
    toolbarLayout->addWidget(m_add);
    toolbarLayout->addWidget(m_edit);
    toolbarLayout->addWidget(m_delete);
    toolbarLayout->addWidget(m_import);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_filterCombo);
    toolbarLayout->addWidget(m_searchField);

    QVBoxLayout* main = new QVBoxLayout(this);
    main->addLayout(toolbarLayout);
    main->addWidget(m_placeholder);
    main->addWidget(m_table);

    auto updateButtons = [this]() {
        bool has = m_model->rowCount() > 0;
        m_placeholder->setVisible(!has);
        m_table->setVisible(has);
        m_edit->setEnabled(has);
        m_delete->setEnabled(has);
    };

    connect(m_add, &QPushButton::clicked, this, &AssessorView::onAdd);
    connect(m_edit, &QPushButton::clicked, this, &AssessorView::onEdit);
    connect(m_delete, &QPushButton::clicked, this, &AssessorView::onDelete);
    connect(m_import, &QPushButton::clicked, this, &AssessorView::onImport);
    connect(m_table->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection&, const QItemSelection&) {
        bool sel = !m_table->selectionModel()->selectedRows().isEmpty();
        m_edit->setEnabled(sel);
        m_delete->setEnabled(sel);
    });

    // Connect filter and search
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AssessorView::onFilterChanged);
    connect(m_searchField, &QLineEdit::textChanged, this, &AssessorView::onSearchTextChanged);

    // Open view dialog on double click
    connect(m_table, &QTableView::doubleClicked, this, &AssessorView::onRowDoubleClicked);

    updateButtons();
}

void AssessorView::onFilterChanged(int index) {
    // 0: id, 1: first, 2: last
    auto list = m_manager ? m_manager->readAll() : std::vector<Assessor>();
    if (!m_manager) list = m_model->rowCount() > 0 ? std::vector<Assessor>() : list; // fallback

    switch (index) {
        case 0:
            std::sort(list.begin(), list.end(), [](const Assessor &a, const Assessor &b){ return a.getAssessorId() < b.getAssessorId(); });
            break;
        case 1:
            std::sort(list.begin(), list.end(), [](const Assessor &a, const Assessor &b){ return a.getFirstName() < b.getFirstName(); });
            break;
        case 2:
            std::sort(list.begin(), list.end(), [](const Assessor &a, const Assessor &b){ return a.getLastName() < b.getLastName(); });
            break;
        default: break;
    }
    if (!list.empty()) m_model->setAssessors(list);
}

void AssessorView::onSearchTextChanged(const QString &text) {
    QString t = text.trimmed();
    if (t.isEmpty()) {
        // reset list
        if (m_manager) m_model->setAssessors(m_manager->readAll());
        return;
    }
    // perform search across id, first, last, email, phone
    std::vector<Assessor> results;
    auto source = m_manager ? m_manager->readAll() : std::vector<Assessor>();
    for (const auto &a : source) {
        if (QString::fromStdString(std::to_string(a.getAssessorId())).contains(t, Qt::CaseInsensitive) ||
            QString::fromStdString(a.getFirstName()).contains(t, Qt::CaseInsensitive) ||
            QString::fromStdString(a.getLastName()).contains(t, Qt::CaseInsensitive) ||
            QString::fromStdString(a.getEmail()).contains(t, Qt::CaseInsensitive) ||
            QString::fromStdString(a.getPhone()).contains(t, Qt::CaseInsensitive)) {
            results.push_back(a);
        }
    }
    m_model->setAssessors(results);
}

void AssessorView::onAdd() {
    AssessorDialog dlg(this);
    dlg.setModeAdd(true);
    if (dlg.exec() == QDialog::Accepted) {
        Assessor a = dlg.getAssessor();
        // Ensure valid ID
        if (a.getAssessorId() < Assessor::ID_PREFIX) {
            a.setAssessorId(Assessor::getNextAssessorId());
            a.setCreationTimestamp();
        }
        if (m_manager) {
            if (!m_manager->create(a)) {
                QMessageBox::warning(this, "Create", "Failed to create assessor in database");
            } else {
                // refresh from DB
                auto rows = m_manager->readAll();
                m_model->setAssessors(rows);
            }
        } else {
            m_model->addAssessor(a);
        }
    }
}

void AssessorView::onEdit() {
    auto sel = m_table->selectionModel()->selectedRows();
    if (sel.empty()) return;
    int row = sel.first().row();
    auto opt = m_model->assessorAt(row);
    if (!opt) return;
    AssessorDialog dlg(this);
    dlg.setAssessor(*opt);
    dlg.setModeAdd(false);
    if (dlg.exec() == QDialog::Accepted) {
        Assessor a = dlg.getAssessor();
        a.updateTimestamp();
        if (m_manager) {
            if (!m_manager->update(a)) {
                QMessageBox::warning(this, "Update", "Failed to update assessor in database");
            } else {
                auto rows = m_manager->readAll();
                m_model->setAssessors(rows);
            }
        } else {
            m_model->updateAssessor(row, a);
        }
    }
}

void AssessorView::onDelete() {
    auto sel = m_table->selectionModel()->selectedRows();
    if (sel.empty()) return;
    int row = sel.first().row();
    auto res = QMessageBox::question(this, "Delete", "Delete selected assessor?");
    if (res == QMessageBox::Yes) {
        auto opt = m_model->assessorAt(row);
        if (!opt) return;
        int id = opt->getAssessorId();
        if (m_manager) {
            if (!m_manager->deleteById(id)) {
                QMessageBox::warning(this, "Delete", "Failed to delete assessor from database");
            } else {
                auto rows = m_manager->readAll();
                m_model->setAssessors(rows);
            }
        } else {
            m_model->removeAssessor(row);
        }
    }
}

void AssessorView::onRowDoubleClicked(const QModelIndex &index) {
    if (!index.isValid()) return;
    int row = index.row();
    auto opt = m_model->assessorAt(row);
    if (!opt) return;
    AssessorDialog dlg(this);
    dlg.setAssessor(*opt);
    dlg.setModeAdd(false);
    dlg.setViewMode(true);
    dlg.exec();
}

void AssessorView::onImport() {
    if (!m_manager) {
        QMessageBox::warning(this, "Import", "Database manager not available");
        return;
    }
    QString path = QFileDialog::getOpenFileName(this, "Import CSV", QString(), "CSV Files (*.csv);;All Files (*)");
    if (path.isEmpty()) return;
    int inserted = m_manager->importFromCSV(path.toStdString());
    QMessageBox::information(this, "Import", QString("Imported %1 rows").arg(inserted));
    // Refresh model
    auto rows = m_manager->readAll();
    m_model->setAssessors(rows);
}
