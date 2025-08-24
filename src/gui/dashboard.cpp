#include "dashboard.h"
#include "managers/AssessorManager.h"
#include "assessor/assessor_view.h"
#include "managers/AssessorManager.h"
#include <QList>
#include <QString>

Dashboard::Dashboard(SilverClinic::AssessorManager* manager, QWidget *parent) : QWidget(parent) {
	// Paleta de cores
	QString colorTitleBar = "#6e00ff";
	QString colorButton = "#aa1bf7";
	QString colorButtonHover = "#bf5df4";
	QString colorError = "#fc4949";
	QString colorConfirm = "#3bf73b";
	QString colorAlert = "#eef42c";
	QString colorSuccess = "#2a77fc";

	// Apply light background and dark text to improve readability across widgets
	this->setStyleSheet("QWidget { background: #fafafa; color: #333333; }");
	// Barra lateral
	QFrame *sideBar = new QFrame;
	sideBar->setFixedWidth(180);
	sideBar->setStyleSheet(QString("background:%1;").arg(colorTitleBar));
	QVBoxLayout *sideLayout = new QVBoxLayout(sideBar);
	sideLayout->setAlignment(Qt::AlignTop);

	QLabel *title = new QLabel("Silver Linings");
	title->setStyleSheet("color:white;font-size:22px;font-weight:bold;margin:16px 0 24px 0;");
	sideLayout->addWidget(title);

	QPushButton *btnAssessor = new QPushButton("Assessor");
	QPushButton *btnClient = new QPushButton("Client");
	QPushButton *btnCaseProfile = new QPushButton("Case Profile");
	QPushButton *btnForms = new QPushButton("Forms");
	QList<QPushButton*> navBtns = {btnAssessor, btnClient, btnCaseProfile, btnForms};
	for (auto btn : navBtns) {
		btn->setStyleSheet(QString(
			"QPushButton {background:%1;color:white;font-size:16px;border:none;padding:12px;margin-bottom:8px;border-radius:6px;}"
			"QPushButton:hover {background:%2;}"
		).arg(colorButton, colorButtonHover));
		sideLayout->addWidget(btn);
	}
	sideLayout->addStretch();

	// Área principal
	QStackedWidget *stack = new QStackedWidget;
	QWidget *assessorPage = new QWidget;
	assessorPage->setLayout(new QVBoxLayout);
	// use the AssessorView widget for the assessor page
	AssessorView* assessorView = new AssessorView(manager, assessorPage);
	assessorPage->layout()->addWidget(assessorView);
	QWidget *clientPage = new QWidget;
	clientPage->setLayout(new QVBoxLayout);
	clientPage->layout()->addWidget(new QLabel("Client Management"));
	QWidget *caseProfilePage = new QWidget;
	caseProfilePage->setLayout(new QVBoxLayout);
	caseProfilePage->layout()->addWidget(new QLabel("Case Profile Management"));
	QWidget *formsPage = new QWidget;
	formsPage->setLayout(new QVBoxLayout);
	formsPage->layout()->addWidget(new QLabel("Forms Management"));
	stack->addWidget(assessorPage);
	stack->addWidget(clientPage);
	stack->addWidget(caseProfilePage);
	stack->addWidget(formsPage);

	// Navegação
	connect(btnAssessor, &QPushButton::clicked, [=]() { stack->setCurrentIndex(0); });
	connect(btnClient, &QPushButton::clicked, [=]() { stack->setCurrentIndex(1); });
	connect(btnCaseProfile, &QPushButton::clicked, [=]() { stack->setCurrentIndex(2); });
	connect(btnForms, &QPushButton::clicked, [=]() { stack->setCurrentIndex(3); });

	// Layout principal
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->addWidget(sideBar);
	mainLayout->addWidget(stack);
	setLayout(mainLayout);
	setStyleSheet("background:#f5f5fa;");
}