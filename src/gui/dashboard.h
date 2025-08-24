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

class Dashboard : public QWidget {
  Q_OBJECT
public:
  Dashboard(QWidget *parent = nullptr);
};
