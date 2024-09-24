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
                     "    -Path to log file\n"
                     "   [-Home folder path]\n";
        return 1;
    }
    QApplication a(argc, argv);
    CommandHandler cmd(argc, argv);
    MainWindow w(nullptr, &cmd);
    QObject::connect(w.GetUi()->btnGoToFolder, &QPushButton::clicked, &w, &MainWindow::HandleCdButtonPressed);
    QObject::connect(&cmd, &CommandHandler::OnCurrentPathChanged, w.GetUi()->txtCurrentFolder, &QLineEdit::setText);
    w.show();
    return a.exec();
}
