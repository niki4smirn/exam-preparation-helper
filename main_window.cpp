#include "main_window.h"

MainWindow::MainWindow() :
    QMainWindow(nullptr) {
  resize(minimal_size_);
  setMinimumSize(minimal_size_);

  SetupMainLayout();
  SetupQuestionWidgetLayout();

  setCentralWidget(widget_);
  ConnectWidgets();
}

void MainWindow::ConnectWidgets() {
  connect(count_,
          QOverload<int>::of(&QSpinBox::valueChanged), [&](int new_count) {
        questions_list_->clear();
        for (int i = 1; i <= new_count; ++i) {
          auto* new_item =
              new QListWidgetItem(QString::number(i), questions_list_);
          new_item->setTextAlignment(Qt::AlignCenter);
          questions_list_->addItem(new_item);
          question_view_->setVisible(false);
          question_name_edit_->setText("");
        }
      });

  connect(questions_list_,
          &QListWidget::itemClicked, [&](QListWidgetItem*) {
        ChooseNewItem(questions_list_->currentRow());
      });

  connect(questions_list_,
          &QListWidget::itemDoubleClicked, [&](QListWidgetItem* item) {
        QuestionStatus status{QuestionStatus::kDefault};
        if (item->background() == QBrush() ||
            item->background() == Qt::yellow) {
          status = QuestionStatus::kReady;
        } else {
          status = QuestionStatus::kNeedToRepeat;
        }
        UpdateQuestionStatus(item, status);
      });

  connect(question_status_,
          QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int index) {
        QuestionStatus new_status;
        switch (index) {
          case 1: {
            new_status = QuestionStatus::kNeedToRepeat;
            break;
          }
          case 2: {
            new_status = QuestionStatus::kReady;
            break;
          }
          default: {
            new_status = QuestionStatus::kDefault;
            break;
          }
        }
        UpdateQuestionStatus(questions_list_->currentItem(), new_status);
      });

  connect(question_name_edit_,
          &QLineEdit::editingFinished, [&]() {
        QString new_text = question_name_edit_->text();
        if (!new_text.isEmpty()) {
          UpdateQuestionName(questions_list_->currentItem(),
                             new_text);
        }
      });

}

void MainWindow::SetupMainLayout() {
  widget_ = new QWidget(this);
  layout_ = new QGridLayout();

  count_ = new QSpinBox(widget_);
  questions_list_ = new QListWidget(widget_);
  question_view_ = new QGroupBox(widget_);

  question_view_->setVisible(false);

  layout_->addWidget(count_, 0, 0);
  layout_->addWidget(questions_list_, 1, 1);
  layout_->addWidget(question_view_, 2, 1);

  layout_->setColumnStretch(0, 1);
  layout_->setColumnStretch(1, 5);

  widget_->setLayout(layout_);
}

static const QStringList kStatuses = {"-", "Нужно ещё повторить", "Выучен"};

void MainWindow::SetupQuestionWidgetLayout() {
  question_view_layout_ = new QGridLayout();

  question_number_ = new QLabel(question_view_);
  question_name_ = new QLabel(question_view_);
  question_name_edit_ = new QLineEdit(question_view_);
  question_status_ = new QComboBox(question_view_);
  question_status_->addItems(kStatuses);

  question_view_layout_->addWidget(question_number_, 0, 0);
  question_view_layout_->addWidget(question_name_, 1, 0);
  question_view_layout_->addWidget(question_name_edit_, 1, 1);
  question_view_layout_->addWidget(question_status_, 2, 0);

  question_view_layout_->setColumnStretch(0, 1);
  question_view_layout_->setColumnStretch(1, 1);

  question_view_->setLayout(question_view_layout_);
}

void MainWindow::FillQuestionNumberAndName(QListWidgetItem* item) {
  int cur_question_number = questions_list_->currentRow() + 1;
  question_number_->setText(QString::number(cur_question_number));
  bool is_number;
  int got_question_number = item->text().toInt(&is_number);
  QString question_name;
  if (is_number && got_question_number == cur_question_number) {
    question_name += "Билет ";
  }
  question_name += item->text();
  question_name_->setText(question_name);
}

void MainWindow::FillQuestionStatus(QListWidgetItem* item) {
  auto color = item->background();
  if (color == Qt::green) {
    question_status_->setCurrentIndex(
        static_cast<int>(QuestionStatus::kReady));
  } else if (color == Qt::yellow) {
    question_status_->setCurrentIndex(
        static_cast<int>(QuestionStatus::kNeedToRepeat));
  } else {
    question_status_->setCurrentIndex(
        static_cast<int>(QuestionStatus::kDefault));
  }
}

void MainWindow::UpdateQuestionStatus(
    QListWidgetItem* item,
    MainWindow::QuestionStatus status) {
  switch (status) {
    case QuestionStatus::kDefault: {
      item->setBackground(QBrush());
      break;
    }
    case QuestionStatus::kNeedToRepeat: {
      item->setBackground(Qt::yellow);
      break;
    }
    case QuestionStatus::kReady: {
      item->setBackground(Qt::green);
      break;
    }
  }
  FillQuestionStatus(item);
}

void MainWindow::UpdateQuestionName(
    QListWidgetItem* item,
    const QString& new_name) {
  item->setText(new_name);
  FillQuestionNumberAndName(item);
}

void MainWindow::ChooseNewItem(int index) {
  auto* item = questions_list_->item(index);
  question_view_->setVisible(true);
  FillQuestionNumberAndName(item);
  FillQuestionStatus(item);
}
