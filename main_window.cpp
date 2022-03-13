#include <random>
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

template<typename Iter>
Iter SelectRandomly(Iter start, Iter end) {
  if (start == end) {
    return end;
  }
  static std::mt19937 gen{std::random_device()()};
  std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
  std::advance(start, dis(gen));
  return start;
}

void MainWindow::ConnectWidgets() {
  connect(count_,
          QOverload<int>::of(&QSpinBox::valueChanged), [&](int new_count) {
        questions_list_->clear();
        prev_question_index.reset();
        cur_question_index.reset();
        total_progress_->setMaximum(new_count);
        green_progress_->setMaximum(new_count);
        default_count_ = new_count;
        green_count_ = 0;
        UpdateProgressBars();
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

  connect(next_question_,
          &QPushButton::clicked, [&]() {
        std::vector<int> unready_indexes;
        for (int i = 0; i < questions_list_->count(); ++i) {
          if (GetStatus(questions_list_->item(i)) != QuestionStatus::kReady) {
            unready_indexes.push_back(i);
          }
        }
        auto it = SelectRandomly(unready_indexes.begin(),
                                 unready_indexes.end());
        if (it != unready_indexes.end()) {
          ChooseNewItem(*it);
        }
      });

  connect(prev_question_,
          &QPushButton::clicked, [&]() {
        if (prev_question_index.has_value()) {
          ChooseNewItem(prev_question_index.value());
        }
      });
}

void MainWindow::SetupMainLayout() {
  widget_ = new QWidget(this);
  layout_ = new QGridLayout();

  count_ = new QSpinBox(widget_);
  questions_list_ = new QListWidget(widget_);
  question_view_ = new QGroupBox(widget_);

  next_question_ = new QPushButton(widget_);
  next_question_->setText("Следующий билет");
  prev_question_ = new QPushButton(widget_);
  prev_question_->setText("Предыдущий билет");

  total_progress_ = new QProgressBar(widget_);
  green_progress_ = new QProgressBar(widget_);

  question_view_->setVisible(false);

  layout_->addWidget(count_, 0, 0, 1, 1);
  layout_->addWidget(next_question_, 1, 0, Qt::AlignTop);
  layout_->addWidget(prev_question_, 2, 0, Qt::AlignTop);
  layout_->addWidget(questions_list_, 1, 1, 2, 2);
  layout_->addWidget(question_view_, 3, 1, 1, 2);
  layout_->addWidget(total_progress_, 0, 1);
  layout_->addWidget(green_progress_, 0, 2);

  layout_->setRowStretch(0, 1);
  layout_->setRowStretch(2, 1);
  layout_->setColumnStretch(0, 1);
  layout_->setColumnStretch(1, 3);
  layout_->setColumnStretch(2, 3);

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
  question_status_->setCurrentIndex(static_cast<int>(GetStatus(item)));
}

void MainWindow::UpdateQuestionStatus(
    QListWidgetItem* item,
    MainWindow::QuestionStatus new_status) {
  UpdateQuestionTypeCounts(item, new_status);
  UpdateProgressBars();
  switch (new_status) {
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
  prev_question_index = cur_question_index;
  cur_question_index = index;
  auto* item = questions_list_->item(index);
  questions_list_->setCurrentItem(item);
  question_view_->setVisible(true);
  FillQuestionNumberAndName(item);
  FillQuestionStatus(item);
}

MainWindow::QuestionStatus MainWindow::GetStatus(QListWidgetItem* item) {
  QuestionStatus result;
  auto color = item->background();
  if (color == Qt::green) {
    result = QuestionStatus::kReady;
  } else if (color == Qt::yellow) {
    result = QuestionStatus::kNeedToRepeat;
  } else {
    result = QuestionStatus::kDefault;
  }
  return result;
}

void MainWindow::UpdateQuestionTypeCounts(
    QListWidgetItem* item,
    MainWindow::QuestionStatus new_status) {
  if (new_status == QuestionStatus::kReady &&
      GetStatus(item) != QuestionStatus::kReady) {
    ++green_count_;
  }
  if (new_status != QuestionStatus::kReady &&
      GetStatus(item) == QuestionStatus::kReady) {
    --green_count_;
  }
  if (new_status == QuestionStatus::kDefault &&
      GetStatus(item) != QuestionStatus::kDefault) {
    ++default_count_;
  }
  if (new_status != QuestionStatus::kDefault &&
      GetStatus(item) == QuestionStatus::kDefault) {
    --default_count_;
  }
}

void MainWindow::UpdateProgressBars() {
  total_progress_->setValue(questions_list_->count() - default_count_);
  green_progress_->setValue(green_count_);
}
