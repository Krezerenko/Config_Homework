#include "mainwindow.h"
#include "CommandHandler.h"
#include <iostream>

#include <QApplication>
#include "ui_mainwindow.h"

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
    MainWindow w(nullptr, &cmd);
    auto ui = w.GetUi();
    QObject::connect(ui->btnCd, &QPushButton::clicked, &w, &MainWindow::HandleCdButtonPressed);
    QObject::connect(&cmd, &CommandHandler::OnCurrentPathChanged, ui->txtCurrentFolder, &QLineEdit::setText);
    QObject::connect(ui->btnLs, &QPushButton::clicked, &w, &MainWindow::HandleLsButtonPressed);
    w.show();
    cmd.InitUi();
    return a.exec();
}
