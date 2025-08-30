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
    // Table styling is provided by central theme QSS (theme.qss). Keep defaults here.

    m_placeholder = new QLabel("Nenhum assessor encontrado.", this);
    m_placeholder->setAlignment(Qt::AlignCenter);
    m_placeholder->setStyleSheet("color: #777; font-size: 14px; padding:20px");

    m_add = new QPushButton(tr("Add"));
    m_edit = new QPushButton(tr("Edit"));
    m_delete = new QPushButton(tr("Delete"));
    m_import = new QPushButton(tr("Import CSV"));
    m_edit->setEnabled(false);
    // semantic object names for QSS theming (use declared member names)
    m_add->setObjectName("primaryButton");
    m_edit->setObjectName("secondaryButton");
    m_delete->setObjectName("dangerButton");
    m_import->setObjectName("secondaryButton");

    // Add filter and search controls
    m_filterCombo = new QComboBox(this);
    m_filterCombo->addItem("Sort by ID");
    m_filterCombo->addItem("Sort by First Name");
    m_filterCombo->addItem("Sort by Last Name");

    m_searchField = new QLineEdit(this);
    m_searchField->setPlaceholderText("Search by id, first, last, email or phone...");
        m_searchField->setObjectName("searchField");

    QHBoxLayout* toolbarLayout = new QHBoxLayout;
    toolbarLayout->setContentsMargins(12, 0, 12, 0); // align with table padding
    toolbarLayout->setSpacing(8);
    toolbarLayout->addWidget(m_add);
    toolbarLayout->addWidget(m_edit);
    toolbarLayout->addWidget(m_delete);
    toolbarLayout->addWidget(m_import);
    toolbarLayout->addStretch();
    toolbarLayout->addWidget(m_filterCombo);
    toolbarLayout->addWidget(m_searchField);

    // Place toolbar layout inside a named widget so QSS can target its buttons
    QWidget* toolbarWidget = new QWidget(this);
    toolbarWidget->setObjectName("assessorToolbar");
    toolbarWidget->setContentsMargins(0,0,0,0);
    toolbarWidget->setLayout(toolbarLayout);

    QVBoxLayout* main = new QVBoxLayout(this);
    main->addWidget(toolbarWidget);
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
