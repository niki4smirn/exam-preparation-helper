#pragma once

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QMainWindow>
#include <QSpinBox>
#include <QPushButton>
#include <optional>

class MainWindow : public QMainWindow {
 Q_OBJECT
 public:
  MainWindow();

 private:
  void ConnectWidgets();
  void SetupMainLayout();
  void SetupQuestionWidgetLayout();
  void FillQuestionNumberAndName(QListWidgetItem* item);
  void FillQuestionStatus(QListWidgetItem* item);

  enum class QuestionStatus {
    kDefault,
    kNeedToRepeat,
    kReady
  };

  void UpdateQuestionStatus(QListWidgetItem* item, QuestionStatus status);
  void UpdateQuestionName(QListWidgetItem* item, const QString& new_name);

  static QuestionStatus GetStatus(QListWidgetItem* item);

  void ChooseNewItem(int index);

  QWidget* widget_;
  QGridLayout* layout_;
  QSpinBox* count_;
  QListWidget* questions_list_;

  QGroupBox* question_view_;
  QGridLayout* question_view_layout_;
  QLabel* question_number_;
  QLabel* question_name_;
  QLineEdit* question_name_edit_;
  QComboBox* question_status_;

  QPushButton* next_question_;
  QPushButton* prev_question_;
  std::optional<int> prev_question_index;
  std::optional<int> cur_question_index;

  QSize minimal_size_{500, 500};
};
