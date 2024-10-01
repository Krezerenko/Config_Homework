#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CommandHandler.h"
#include "ZipHandler.h"
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
//    ZipHandler zip(argv[3]);
//    std::cout << zip.WriteToFile("some/stupid/file", "lol\n") << '\n';
    CommandHandler cmd(argc, argv);
    MainWindow w(nullptr, &cmd);
    auto ui = w.GetUi();
    QObject::connect(ui->btnCd, &QPushButton::clicked, &w, &MainWindow::HandleCdButtonPressed);
    QObject::connect(&cmd, &CommandHandler::OnCurrentPathChanged, ui->txtCurrentFolder, &QLineEdit::setText);
    QObject::connect(ui->btnLs, &QPushButton::clicked, &w, &MainWindow::HandleLsButtonPressed);
//    QObject::connect(&a, &QApplication::aboutToQuit, &cmd, &CommandHandler::CleanUp);
    w.show();
    cmd.InitUi();
    return a.exec();
}
