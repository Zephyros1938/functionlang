#include "main_window.hpp"
#include "qcontainerfwd.h"
#include "qobject.h"
#include "qtablewidget.h"
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSplitter>
#include <QVBoxLayout>
#include <format>
#include <functionlang.hpp>
#include <iostream>
#include <limits>

MyWindow::MyWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi();
  setupConnections();

  setWindowTitle(std::format("FunctionLang Gui Editor {} - {}",
                             functionlang::VERSION, functionlang::VERSIONTEXT)
                     .c_str());
  resize(1000, 600);
  statusBar()->showMessage("Ready");
  functionlangArgs.resize(functionlang::INTERNAL_VARIABLE_START,
                          -std::numeric_limits<double>::max());
}

void MyWindow::setupUi() {
  auto *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  auto *mainLayout = new QVBoxLayout(centralWidget);

  auto *splitter = new QSplitter(Qt::Horizontal);

  // ---  Editor Area ---
  auto *leftWidget = new QWidget();
  auto *leftLayout = new QVBoxLayout(leftWidget);

  equationInput = new QLineEdit();
  equationInput->setPlaceholderText(
      "Enter equation (e.g., +1,1 or -(1,1) or A1,3,0,@0 or "
      "+A1,5,-1,^@0,@0,_/*20,21,2,2)");
  equationInput->setFixedHeight(40);

  equationInput->setStyleSheet("font-family: 'Consolas', 'Monaco', monospace; "
                               "font-size: 14px; padding: 5px;");

  auto *buttonLayout = new QHBoxLayout();
  calcButton = new QPushButton("Calculate (Enter)");
  liveButton = new QPushButton("Live Mode");
  liveButton->setCheckable(true);
  buttonLayout->addWidget(calcButton);
  buttonLayout->addWidget(liveButton);

  leftLayout->addWidget(new QLabel("<b>Equation Editor</b>"));
  leftLayout->addWidget(equationInput);
  leftLayout->addLayout(buttonLayout);
  leftLayout->addStretch(); // Pushes elements to the top

  // ---  Variables Table ---
  varTable = new QTableWidget(functionlang::INTERNAL_VARIABLE_START, 1);
  varTable->setHorizontalHeaderLabels({"Value"});
  varTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  splitter->addWidget(leftWidget);
  splitter->addWidget(varTable);
  splitter->setStretchFactor(0, 2); // Editor gets more space than table

  mainLayout->addWidget(splitter);
}

void MyWindow::setupConnections() {
  connect(calcButton, &QPushButton::clicked, this, &MyWindow::handleCalculate);
  connect(equationInput, &QLineEdit::returnPressed, this,
          &MyWindow::handleCalculate);

  connect(equationInput, &QLineEdit::textChanged, this,
          &MyWindow::onTextChanged);
  connect(liveButton, &QPushButton::toggled, this, &MyWindow::toggleLiveMode);
  connect(varTable, &QTableWidget::itemChanged, this,
          &MyWindow::handleVarTableChange);
}

void MyWindow::onTextChanged(const QString &text) {
  if (0) {

  } else {
    equationInput->setStyleSheet(
        "background-color: white; color: black; border: 1px solid gray;");
    statusBar()->clearMessage();
  }

  if (liveButton->isChecked()) {
    handleCalculate();
  }
}

void MyWindow::handleCalculate() {
  QString expression = equationInput->text();
  if (expression.isEmpty())
    return;

  auto expr = expression.toStdString();
  const char *expr_cstr = expr.c_str();
  const double evaluated =
      functionlang::parseExpression(expr_cstr)(functionlangArgs);
  statusBar()->showMessage(QString::fromStdString(std::format("{}", evaluated)),
                           -1);
}

void MyWindow::toggleLiveMode(bool checked) {
  isLiveMode = checked;
  if (isLiveMode) {
    statusBar()->showMessage("Live Mode: ON", -1);
    handleCalculate();
  } else {
    statusBar()->showMessage("Live Mode: OFF", -1);
  }
}

void MyWindow::handleVarTableChange(QTableWidgetItem *item) {
  int row = item->row();
  int col = item->column();
  QString text = item->text();

  std::cout << row << ":" << col << " " << text.toStdString() << std::endl;
  functionlangArgs[row] = text.toDouble();
  auto expr = equationInput->text().toStdString();
  auto expr_cstr = expr.c_str();
  const double evaluated =
      functionlang::parseExpression(expr_cstr)(functionlangArgs);
  statusBar()->showMessage(QString::fromStdString(std::format("{}", evaluated)),
                           -1);
}
