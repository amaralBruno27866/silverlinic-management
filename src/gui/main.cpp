
#include <QApplication>
#include <QMainWindow>
#include "dashboard.h"


int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QMainWindow window;
  window.setWindowTitle("SilverClinic");
  window.resize(1280, 720);

  Dashboard *dashboard = new Dashboard(&window);
  window.setCentralWidget(dashboard);
  window.show();

  return app.exec();
}