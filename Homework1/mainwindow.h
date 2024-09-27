

#ifndef HOMEWORK1_MAINWINDOW_H
#define HOMEWORK1_MAINWINDOW_H

#include <QMainWindow>
#include "CommandHandler.h"


QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    explicit MainWindow(QWidget *parent, CommandHandler *cmd);

    ~MainWindow() override;

    [[nodiscard]] Ui::MainWindow * GetUi() const;

public slots:
    void HandleCdButtonPressed();
    void HandleLsButtonPressed();

private:
    Ui::MainWindow *m_ui;
    CommandHandler *m_cmd;
};


#endif //HOMEWORK1_MAINWINDOW_H
