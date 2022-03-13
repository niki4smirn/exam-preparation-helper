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
  connect(count_,
          QOverload<int>::of(&QSpinBox::valueChanged), [&](int new_count) {
    questions_list_->clear();
    for (int i = 1; i <= new_count; ++i) {
      auto* new_item = new QListWidgetItem(QString::number(i), questions_list_);
      new_item->setTextAlignment(Qt::AlignCenter);
      questions_list_->addItem(new_item);
    }
  });

  connect(questions_list_,
          &QListWidget::itemDoubleClicked, [&](QListWidgetItem *item) {
    if (item->background() == QBrush() ||
        item->background() == Qt::yellow) {
      item->setBackground(Qt::green);
    } else {
      item->setBackground(Qt::yellow);
    }
  });
}
