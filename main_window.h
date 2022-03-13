#pragma once

#include <QMainWindow>
#include <QSpinBox>
#include <QListWidget>
#include <QGridLayout>

class MainWindow : public QMainWindow {
 Q_OBJECT
 public:
  MainWindow();

 private:
  void ConnectWidgets();

  QWidget* widget_;
  QGridLayout* layout_;
  QSpinBox* count_;
  QListWidget* questions_list_;

  QSize minimal_size_{300, 300};
};
