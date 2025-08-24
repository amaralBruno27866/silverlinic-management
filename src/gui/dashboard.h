#pragma once
#include <QWidget>
#include <QToolBar>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QList>
#include <QString>

namespace SilverClinic { class AssessorManager; }

class Dashboard : public QWidget {
  Q_OBJECT
public:
  explicit Dashboard(SilverClinic::AssessorManager* manager = nullptr, QWidget *parent = nullptr);
};
