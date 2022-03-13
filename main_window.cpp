#include "main_window.h"

MainWindow::MainWindow() :
    QMainWindow(nullptr),
    widget_(new QWidget(this)),
    layout_(new QGridLayout()) {
  resize(minimal_size_);
  setMinimumSize(minimal_size_);

  count_ = new QSpinBox(widget_);
  questions_list_ = new QListWidget(widget_);
  layout_->addWidget(count_, 0, 0);
  layout_->addWidget(questions_list_, 1, 1);
  widget_->setLayout(layout_);
  setCentralWidget(widget_);
  ConnectWidgets();
}

void MainWindow::ConnectWidgets() {
  connect(count_, QOverload<int>::of(&QSpinBox::valueChanged), [&](int i) {
    while (i < questions_list_->count()) {
      questions_list_->takeItem(questions_list_->count() - 1);
    }
    while (i > questions_list_->count()) {
      questions_list_->addItem(new QListWidgetItem(QString::number(i), questions_list_));
    }
  });
}
