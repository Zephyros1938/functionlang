#pragma once

#include "qtablewidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QStatusBar>
#include <QTableWidget>
#include <qpushbutton.h>
#include <vector>

class MyWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MyWindow(QWidget *parent = nullptr);
  ~MyWindow() override = default;

private slots:
  // Manual triggers
  void handleCalculate();
  void handleVarTableChange(QTableWidgetItem *item);

  // Automatic triggers
  void onTextChanged(const QString &text);
  void toggleLiveMode(bool checked);

private:
  // --- UI Setup Methods ---
  void setupUi();
  void setupConnections();

  // --- Editor Widgets ---
  QLineEdit *equationInput;
  QPushButton *calcButton;
  QPushButton *liveButton;

  // --- Data Widgets ---
  QTableWidget *varTable;

  // --- Logic State ---
  bool isLiveMode = false;

  // Helper to update the status bar easily
  void updateStatus(const QString &message, int timeout = 0);

  // functionlang data
  std::vector<double> functionlangArgs;
};
