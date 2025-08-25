#include "dashboard.h"
#include "managers/AssessorManager.h"
#include "assessor/assessor_view.h"
#include "managers/AssessorManager.h"
#include <QList>
#include <QString>

Dashboard::Dashboard(SilverClinic::AssessorManager* manager, QWidget *parent) : QWidget(parent) {
	// Paleta de cores (mais sóbria / profissional)
	QString colorTitleBar = "#203040";    // deep slate
	QString colorButton = "#3a4a57";      // muted slate for active nav buttons
	QString colorButtonHover = "#2f3b45"; // slightly darker on hover
	QString colorButtonText = "#ffffff";
	QString colorBackground = "#f6f7f9";  // neutral light background
	QString colorContentBg = "#ffffff";
	QString colorAccent = "#4f6d86";      // subtle accent

	// Apply light background and dark text to improve readability across widgets
	this->setStyleSheet(QString("QWidget { background: %1; color: #222222; }").arg(colorBackground));
	// Barra lateral
	QFrame *sideBar = new QFrame;
	sideBar->setFixedWidth(200);
	sideBar->setStyleSheet(QString("background:%1;").arg(colorTitleBar));
	QVBoxLayout *sideLayout = new QVBoxLayout(sideBar);
	sideLayout->setAlignment(Qt::AlignTop);

	QLabel *title = new QLabel("Silver Linings");
	title->setStyleSheet("color:white;font-size:20px;font-weight:600;margin:16px 8px 16px 8px;");
	title->setAlignment(Qt::AlignCenter);
	sideLayout->addWidget(title);

	QPushButton *btnAssessor = new QPushButton("Assessor");
	QPushButton *btnClient = new QPushButton("Client");
	QPushButton *btnCaseProfile = new QPushButton("Case Profile");
	QPushButton *btnForms = new QPushButton("Forms");
	QList<QPushButton*> navBtns = {btnAssessor, btnClient, btnCaseProfile, btnForms};
	for (auto btn : navBtns) {
		// Standardize button size and appearance
		btn->setFixedHeight(44);
		btn->setCursor(Qt::PointingHandCursor);
		btn->setStyleSheet(QString(
			"QPushButton { background: %1; color: %2; font-size:15px; border:none; padding:0 12px; margin-bottom:10px; border-radius:6px; text-align:left; }"
			"QPushButton:hover { background: %3; }"
		).arg(colorButton, colorButtonText, colorButtonHover));
		btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		sideLayout->addWidget(btn);
	}
	sideLayout->addStretch();

	// Área principal (com cabeçalho que mostra a seção atual)
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

	// Header for content area
	QFrame *contentHeader = new QFrame;
	contentHeader->setObjectName("contentHeader");
	contentHeader->setFixedHeight(56);
	QLabel *sectionTitle = new QLabel("Assessor");
	// title styling is handled by theme.qss for QFrame#contentHeader QLabel
	QHBoxLayout *headerLayout = new QHBoxLayout(contentHeader);
	headerLayout->addWidget(sectionTitle);
	headerLayout->addStretch();

	// Content container (header + stacked pages)
	QWidget *content = new QWidget;
	QVBoxLayout *contentLayout = new QVBoxLayout(content);
	contentLayout->setContentsMargins(12,12,12,12);
	contentLayout->setSpacing(8);
	content->setStyleSheet(QString("background:%1;").arg(colorContentBg));
	contentLayout->addWidget(contentHeader);
	contentLayout->addWidget(stack);

	// Navegação e atualização do título da seção
	connect(btnAssessor, &QPushButton::clicked, [=]() { stack->setCurrentIndex(0); sectionTitle->setText("Assessor"); });
	connect(btnClient, &QPushButton::clicked, [=]() { stack->setCurrentIndex(1); sectionTitle->setText("Client"); });
	connect(btnCaseProfile, &QPushButton::clicked, [=]() { stack->setCurrentIndex(2); sectionTitle->setText("Case Profile"); });
	connect(btnForms, &QPushButton::clicked, [=]() { stack->setCurrentIndex(3); sectionTitle->setText("Forms"); });

	// Layout principal
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->addWidget(sideBar);
	mainLayout->addWidget(content);
	setLayout(mainLayout);
}