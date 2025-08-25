#ifndef ASSESSOR_DIALOG_H
#define ASSESSOR_DIALOG_H

#include <QDialog>
#include <memory>
#include "../../include/core/Assessor.h"

using namespace SilverClinic;

class QLineEdit;
class QComboBox;
class QDialogButtonBox;
class QPushButton;
class QLabel;

class AssessorDialog : public QDialog {
    Q_OBJECT
public:
    explicit AssessorDialog(QWidget* parent = nullptr);
    void setAssessor(const Assessor& a);
    Assessor getAssessor() const;
    void setModeAdd(bool add);
    void setViewMode(bool view);

private:
    QLineEdit* m_first;
    QLineEdit* m_last;
    QLineEdit* m_email;
    QLineEdit* m_phone;
    QLineEdit* m_street;
    QLineEdit* m_city;
    QComboBox* m_province;
    QLineEdit* m_postal;
    Assessor m_assessor;
    // buttons and controls for view/edit behavior
    QDialogButtonBox* m_buttons;
    QPushButton* m_okBtn;
    QPushButton* m_cancelBtn;
    // dialog header
    QLabel* m_dialogTitle;
};

#endif // ASSESSOR_DIALOG_H
