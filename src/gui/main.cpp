#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QMainWindow window;
  window.setWindowTitle("Silver Linings Clinic");
  window.resize(1280, 720);

  // Widget central
  QWidget *centralWidget = new QWidget(&window);
  QVBoxLayout *layout = new QVBoxLayout(centralWidget);

  // Button
  QPushButton *button = new QPushButton("Click Me", centralWidget);
  layout->addWidget(button);

  QObject::connect(button, &QPushButton::clicked, [&]() {
      QMessageBox::information(&window, "Hello", "Welcome to Silver Linings Clinic!");
  });

  window.setCentralWidget(centralWidget);
  window.show();

  return app.exec();
}