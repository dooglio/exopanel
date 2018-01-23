#pragma once

#include "ui_MainWindow.h"

#include <QMainWindow>
#include <QProcess>

#include <vector>

class MainWindow
        : public QMainWindow
        , private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *pnt = 0);
    ~MainWindow();

    static bool setupDatabase();

protected:
    void closeEvent( QCloseEvent* event ) override;

private:
    std::vector<QProcess*>   f_processList;

    void handleSelection();
    void refresh();

private slots:
    void onAboutToQuit();
    void on_action_Exit_triggered();
    void on_sendButton_clicked();
    void on_txSendButton_clicked();
    void on_jsonButton_clicked();
    void on_xpubButton_clicked();
    void on_ethButton_clicked();
    void on_cmdButton_clicked();
    void on_pushButton_clicked();
};

