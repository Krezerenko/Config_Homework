

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

    ~MainWindow() override;

    [[nodiscard]] Ui::MainWindow * GetUi() const;

public slots:
    void HandleCdButtonPressed();
    void HandleLsButtonPressed();
    void HandleHistoryButtonPressed();
    void HandleRevButtonPressed();

signals:
    void OnCdButtonPressed(const QString &inputPath);
    void OnLsButtonPressed(const QString &inputPath);
    void OnHistoryButtonPressed();
    void OnRevButtonPressed(const QString &inputPath);

private:
    Ui::MainWindow *m_ui;
//    CommandHandler *m_cmd;
};


#endif //HOMEWORK1_MAINWINDOW_H
