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
    void onCurrentSelectionChanged(const QModelIndex & , const QModelIndex & );
    void onWalletSelected(const QItemSelection & /*selected*/, const QItemSelection & /*deselected*/);

    void on_action_NewWallet_triggered();
    void on_action_RunWallet_triggered();
    void on_action_EditWallet_triggered();
    void on_action_DeleteWallet_triggered();
    void on_action_Settings_triggered();
    void on_action_Exit_triggered();
    void on_tabWidget_tabCloseRequested(int index);
    //void on_action_SSOrderId_triggered();
    void on_sendButton_clicked();
    void on_txSendButton_clicked();
    void on_jsonButton_clicked();
    void on_xpubButton_clicked();
    void on_ethButton_clicked();
    void on_cmdButton_clicked();
    void on_pushButton_clicked();
};

