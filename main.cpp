#include <QApplication>

#include "main_window.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QCoreApplication::setApplicationName("ExamPreparationHelper");

  MainWindow window;
  window.show();

  return QApplication::exec();
}