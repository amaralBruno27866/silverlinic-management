#include "assessor_dialog.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>

// Populate provinces list when dialog is constructed
// We'll add items to the combobox after creating it
namespace {
    const std::vector<QString> CANADIAN_PROVINCES = {
        "AB - Alberta", "BC - British Columbia", "MB - Manitoba", "NB - New Brunswick",
        "NL - Newfoundland and Labrador", "NS - Nova Scotia", "NT - Northwest Territories",
        "NU - Nunavut", "ON - Ontario", "PE - Prince Edward Island", "QC - Quebec",
        "SK - Saskatchewan", "YT - Yukon"
    };
}

AssessorDialog::AssessorDialog(QWidget* parent) : QDialog(parent) {
    // layout and fields
    auto *layout = new QFormLayout(this);
    layout->setLabelAlignment(Qt::AlignRight);
    layout->setFormAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    // Slightly larger dialog for readability
    setMinimumSize(480, 260);

    m_first = new QLineEdit(this);
    m_last = new QLineEdit(this);
    m_email = new QLineEdit(this);
    m_phone = new QLineEdit(this);
    m_street = new QLineEdit(this);
    m_city = new QLineEdit(this);
    m_province = new QComboBox(this);
    m_postal = new QLineEdit(this);

    // nicer input sizes
    m_first->setMinimumWidth(260);
    m_last->setMinimumWidth(260);
    m_email->setMinimumWidth(260);
    m_phone->setMinimumWidth(260);

    // populate provinces
    for (const auto &p : CANADIAN_PROVINCES) {
        m_province->addItem(p);
    }

    layout->addRow(tr("First name:"), m_first);
    layout->addRow(tr("Last name:"), m_last);
    layout->addRow(tr("Email:"), m_email);
    layout->addRow(tr("Phone:"), m_phone);
    // Address fields
    layout->addRow(tr("Street:"), m_street);
    layout->addRow(tr("City:"), m_city);
    layout->addRow(tr("Province:"), m_province);
    layout->addRow(tr("Postal code:"), m_postal);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_okBtn = m_buttons->button(QDialogButtonBox::Ok);
    m_cancelBtn = m_buttons->button(QDialogButtonBox::Cancel);
    connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(m_buttons);

    // Styling to ensure labels/input are readable regardless of parent styles
    this->setStyleSheet(
    "QDialog { background: #ffffff; }"
    "QLabel { color: #222222; font-size: 14px; }"
    "QLineEdit { color: #333333; font-size: 13px; padding:6px; border:1px solid #d0d0d0; border-radius:4px; background:#ffffff; }"
    "QLineEdit:disabled { color: #777777; }"
    "QLineEdit[placeholderText] { color: #888888; }"
    "QComboBox { color: #333333; font-size: 13px; padding:6px; }"
    "QComboBox QAbstractItemView { color: #333333; }")
    ;

    // Style the dialog buttons (Ok green, Cancel red)
    QPushButton* okBtn = m_buttons->button(QDialogButtonBox::Ok);
    QPushButton* cancelBtn = m_buttons->button(QDialogButtonBox::Cancel);
    if (okBtn) {
        okBtn->setStyleSheet("background: #2a77fc; color: white; padding:6px 12px; border-radius:6px;");
    }
    if (cancelBtn) {
        cancelBtn->setStyleSheet("background: #fc4949; color: white; padding:6px 12px; border-radius:6px;");
    }
}

void AssessorDialog::setModeAdd(bool add) {
    if (add) {
        this->setWindowTitle(tr("Add a new Assessor"));
    } else {
        this->setWindowTitle(tr("Edit Assessor"));
    }
}

void AssessorDialog::setViewMode(bool view) {
    // Make inputs read-only when in view mode
    m_first->setReadOnly(view);
    m_last->setReadOnly(view);
    m_email->setReadOnly(view);
    m_phone->setReadOnly(view);
    m_street->setReadOnly(view);
    m_city->setReadOnly(view);
    m_postal->setReadOnly(view);
    m_province->setEnabled(!view);

    if (view) {
        this->setWindowTitle(tr("View Assessor"));
        if (m_okBtn) m_okBtn->setEnabled(false);
        if (m_cancelBtn) m_cancelBtn->setText(tr("Close"));
    } else {
        if (m_okBtn) m_okBtn->setEnabled(true);
        if (m_cancelBtn) m_cancelBtn->setText(tr("Cancel"));
    }
}



void AssessorDialog::setAssessor(const Assessor& a) {
    m_assessor = a;
    m_first->setText(QString::fromStdString(a.getFirstName()));
    m_last->setText(QString::fromStdString(a.getLastName()));
    m_email->setText(QString::fromStdString(a.getEmail()));
    m_phone->setText(QString::fromStdString(a.getPhone()));
    m_street->setText(QString::fromStdString(a.getAddress().getStreet()));
    m_city->setText(QString::fromStdString(a.getAddress().getCity()));
    // match province text to combobox entry if possible
    QString prov = QString::fromStdString(a.getAddress().getProvince());
    int idx = m_province->findText(prov, Qt::MatchContains);
    if (idx >= 0) m_province->setCurrentIndex(idx);
    m_postal->setText(QString::fromStdString(a.getAddress().getPostalCode()));
}

Assessor AssessorDialog::getAssessor() const {
    Assessor a = m_assessor;
    a.setFirstName(m_first->text().toStdString());
    a.setLastName(m_last->text().toStdString());
    a.setEmail(m_email->text().toStdString());
    a.setPhone(m_phone->text().toStdString());
    // build address
    Address addr;
    addr.setStreet(m_street->text().toStdString());
    addr.setCity(m_city->text().toStdString());
    addr.setProvince(m_province->currentText().toStdString());
    addr.setPostalCode(m_postal->text().toStdString());
    a.setAddress(addr);
    return a;
}
