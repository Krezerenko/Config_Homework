

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "CommandHandler.h"


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
//    m_cmd = nullptr;
}

//MainWindow::MainWindow(QWidget *parent, CommandHandler *cmd) : MainWindow(parent)
//{
//    m_cmd = cmd;
//}

MainWindow::~MainWindow()
{
    delete m_ui;
}

Ui::MainWindow *MainWindow::GetUi() const
{
    return m_ui;
}

void MainWindow::HandleCdButtonPressed()
{
    emit OnCdButtonPressed(m_ui->txtCdFolderPath->text());
}

void MainWindow::HandleLsButtonPressed()
{
    emit OnLsButtonPressed(m_ui->txtLsFolderPath->text());
}

void MainWindow::HandleHistoryButtonPressed()
{
    emit OnHistoryButtonPressed();
}

void MainWindow::HandleRevButtonPressed()
{
    emit OnRevButtonPressed(m_ui->txtRevFilePath->text());
}
