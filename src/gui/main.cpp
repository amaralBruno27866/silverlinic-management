
#include <QApplication>
#include <QMainWindow>
#include <sqlite3.h>
#include "dashboard.h"
#include "core/DatabaseConfig.h"
#include "managers/AssessorManager.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // Global styling: ensure dialogs and inputs use a readable dark-gray font
  app.setStyleSheet(
    "QDialog { color: #333333; }"
    "QLabel { color: #333333; }"
    "QLineEdit { color: #333333; }"
    "QComboBox { color: #333333; }"
    "QPushButton { color: #333333; }"
  );

  // Open the main database and apply pragmas
  sqlite3* db = nullptr;
  int rc = sqlite3_open(DatabaseConfig::MAIN_DATABASE_PATH.c_str(), &db);
  if (rc != SQLITE_OK || !DatabaseConfig::applyStandardPragmas(db)) {
      // If DB can't be opened, log to stderr and still launch app in degraded mode
      fprintf(stderr, "Failed to open or configure database: %s\n", sqlite3_errmsg(db));
      if (db) sqlite3_close(db);
      db = nullptr;
  }

  // Create manager if db is available
  SilverClinic::AssessorManager* manager = nullptr;
  if (db) {
      manager = new SilverClinic::AssessorManager(db);
  }

  QMainWindow window;
  window.setWindowTitle("SilverClinic");
  window.resize(1280, 720);

  Dashboard *dashboard = new Dashboard(manager, &window);
  window.setCentralWidget(dashboard);
  window.show();

  int ret = app.exec();

  // Cleanup
  delete dashboard;
  delete manager;
  if (db) sqlite3_close(db);

  return ret;
}