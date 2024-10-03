#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CommandHandler.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cerr << "Wrong amount of arguments.\n"
                     "Please provide:\n"
                     "    -User name\n"
                     "    -Computer model\n"
                     "    -Path to file system image\n"
                     "    -Path to Log file\n"
                     "   [-Home folder path]\n";
        return 1;
    }
    QApplication a(argc, argv);
    CommandHandler cmd(argc, argv);
    MainWindow w;
    auto ui = w.GetUi();

    QObject::connect(&cmd, &CommandHandler::OnCurrentPathChanged, ui->txtCurrentFolder, &QLineEdit::setText);
    QObject::connect(&cmd, &CommandHandler::OnOutput, ui->txtOutput, &QTextEdit::setText);

    QObject::connect(ui->btnCd, &QPushButton::clicked, &w, &MainWindow::HandleCdButtonPressed);
    QObject::connect(&w, &MainWindow::OnCdButtonPressed, &cmd, &CommandHandler::HandleCdButtonPressed);

    QObject::connect(ui->btnLs, &QPushButton::clicked, &w, &MainWindow::HandleLsButtonPressed);
    QObject::connect(&w, &MainWindow::OnLsButtonPressed, &cmd, &CommandHandler::HandleLsButtonPressed);

    QObject::connect(ui->btnHistory, &QPushButton::clicked, &w, &MainWindow::HandleHistoryButtonPressed);
    QObject::connect(&w, &MainWindow::OnHistoryButtonPressed, &cmd, &CommandHandler::HandleHistoryButtonPressed);

    QObject::connect(ui->btnCat, &QPushButton::clicked, &w, &MainWindow::HandleCatButtonPressed);
    QObject::connect(&w, &MainWindow::OnCatButtonPressed, &cmd, &CommandHandler::HandleCatButtonPressed);

    QObject::connect(ui->btnRev, &QPushButton::clicked, &w, &MainWindow::HandleRevButtonPressed);
    QObject::connect(&w, &MainWindow::OnRevButtonPressed, &cmd, &CommandHandler::HandleRevButtonPressed);

    QObject::connect(ui->btnExit, &QPushButton::clicked, &cmd, &CommandHandler::exit);

    w.show();
    cmd.InitUi();
    return a.exec();
}
