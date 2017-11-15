#include "MainWindow.h"
#include "SettingsForm.h"
#include "WalletForm.h"
#include "WalletModel.h"

#include <QtSql>
#include <QtWidgets>

#include <algorithm>


MainWindow::MainWindow(QWidget *pnt)
    : QMainWindow(pnt)
{
    setupUi( this );

    QSettings const settings;
    restoreGeometry( settings.value( "geometry", saveGeometry() ).toByteArray() );
    restoreState   ( settings.value( "state"   , saveState()    ).toByteArray() );

    WalletModel* the_model( new WalletModel(this) );
    queryTableView->setModel( the_model );
    queryTableView->show();

    refresh();

    auto selection_model( queryTableView->selectionModel() );
    connect( selection_model, &QItemSelectionModel::currentChanged  , this, &MainWindow::onCurrentSelectionChanged );
    connect( selection_model, &QItemSelectionModel::selectionChanged, this, &MainWindow::onWalletSelected          );
    connect( qApp,            &QApplication::aboutToQuit,             this, &MainWindow::onAboutToQuit             );

    upperDockWidget->hide();
    lowerDockWidget->hide();

    setWindowTitle( tr("Multiple Exoduses %1").arg(MULTIEXO_VERSION) );
}


MainWindow::~MainWindow()
{
}


void MainWindow::onAboutToQuit()
{
    QSettings settings;
    settings.setValue( "geometry", saveGeometry() );
    settings.setValue( "state",    saveState()    );
}


void MainWindow::onCurrentSelectionChanged(const QModelIndex & , const QModelIndex & )
{
    handleSelection();
}


void MainWindow::onWalletSelected(const QItemSelection & /*selected*/, const QItemSelection & /*deselected*/)
{
    handleSelection();
}


void MainWindow::handleSelection()
{
    auto selection_model( queryTableView->selectionModel() );
    auto rows( selection_model->selectedRows() );
    if( rows.empty() )
    {
        action_RunWallet->setEnabled(false);
        action_EditWallet->setEnabled(false);
        action_DeleteWallet->setEnabled(false);
    }
    else
    {
        action_RunWallet->setEnabled(rows.size() == 1);
        action_EditWallet->setEnabled(rows.size() == 1);
        action_DeleteWallet->setEnabled(true);
    }
}



bool MainWindow::setupDatabase()
{
    QDir const app_data_path( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) );
    app_data_path.mkpath( app_data_path.absolutePath() );
    QString const sql_file( QString("%1/data.sql").arg(app_data_path.absolutePath()) );

    auto db( QSqlDatabase::addDatabase("QSQLITE") );
    db.setDatabaseName( sql_file );
    if( !db.open() )
    {
        QMessageBox::critical( 0, "Database Error", QString("Cannot open database file %1!").arg(sql_file) );
        return false;
    }

    QSqlQuery q;
    return q.exec(
        "CREATE TABLE IF NOT EXISTS wallets "
        "( id INTEGER PRIMARY KEY ASC, name TEXT, type INTEGER, data_dir TEXT, description TEXT, notes TEXT )"
    );
}


void MainWindow::closeEvent( QCloseEvent* event )
{
    if( f_processList.empty() )
    {
        event->accept();
    }
    else
    {
        QMessageBox::critical( this, tr("Processes running!"), tr("You cannot close multiexo because there are Exodus processes running!") );
        event->ignore();
    }
}


void MainWindow::refresh()
{
    queryTableView->selectionModel()->clearSelection();

    auto* model( dynamic_cast<WalletModel*>(queryTableView->model()) );
    Q_ASSERT(model);
    model->doQuery();

    queryTableView->hideColumn( WalletModel::field_id       );    // hide id column
    queryTableView->hideColumn( WalletModel::field_data_dir );    // hide directory column
    queryTableView->hideColumn( WalletModel::field_notes    );    // hide notes column

    for( int idx = 0; idx < model->columnCount(); ++idx )
    {
        if( !queryTableView->isColumnHidden(idx) )
        {
            queryTableView->resizeColumnToContents( idx );
        }
    }
}


void MainWindow::on_action_NewWallet_triggered()
{
    WalletForm dlg( this );
    if( dlg.exec() == QDialog::Accepted )
    {
        refresh();
    }
}


void MainWindow::on_action_RunWallet_triggered()
{
    auto selected( queryTableView->selectionModel()->selectedIndexes() );
    auto type( selected.at(WalletModel::field_type).data().toString() );
    auto dir( selected.at(WalletModel::field_data_dir).data().toString()  );
    auto const tabText( selected.at(WalletModel::field_name).data().toString() );

    QTextEdit* edit = nullptr;
    for( int idx = 0; idx < tabWidget->count(); ++idx )
    {
        if( tabWidget->tabText(idx) == QString("%1 (finished)").arg(tabText) )
        {
            // Reuse the editor if the tab has the same name...
            edit = dynamic_cast<QTextEdit*>(tabWidget->widget(idx));
            edit->clear();
            tabWidget->setTabText( idx, tabText ); // Just in case we are reusing, we take the "(finished)" text away.
            break;
        }
    }

    if( edit == nullptr )
    {
        // If no entry, make a new one.
        edit = new QTextEdit(tabWidget);
        edit->setReadOnly(true);
        tabWidget->addTab( edit, tabText );
    }
    edit->setHtml( "<b>Process Starting...</b>" );

    tabWidget->setCurrentWidget( edit );
    lowerDockWidget->show();

    auto* the_model( dynamic_cast<WalletModel*>(queryTableView->model()) );
    Q_ASSERT(the_model);
    int const row( selected.at(WalletModel::field_id).row() );
    the_model->setItemUnavailable( row, true );

    // Just in case, so the user can't reselect the disabled item.
    queryTableView->selectionModel()->clearSelection();

    QProcess* process( new QProcess(this) ); // This process is now owned by the window, which will delete everything once done...

    // Listen for finished signal.
    // Set text message at end and change the tab name,
    // Remove process pointer from list.
    connect( process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
             [process,row,edit,this](int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/ )
    {
        auto* inner_model( dynamic_cast<WalletModel*>(queryTableView->model()) );
        Q_ASSERT(inner_model);
        inner_model->setItemUnavailable( row, false );

        edit->append( "<br/><br/><b>Process Finished.</b>" );
        int const tabid = tabWidget->indexOf(edit);
        tabWidget->setTabText( tabid, QString("%1 (finished)").arg(tabWidget->tabText(tabid)) );

        // Find our process and remove it from the list, since it's terminated...
        //
        auto iter = std::find_if( std::begin(f_processList), std::end(f_processList), [&]( QProcess* proc )
        {
            return proc == process;
        });
        if( iter != std::end(f_processList) )
        {
            f_processList.erase(iter);
        }
    });

    // Tell the user if there is an error launch the application
    //
    connect( process, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
             [&]( QProcess::ProcessError /*error*/ )
    {
        QMessageBox::critical( this, tr("Error launch application!"), tr("Could not run Exodus or Eden!") );
    });

    // Now listen for stdout/err and put it into the editor
    //
    connect( process, static_cast<void(QProcess::*)()>(&QProcess::readyRead), [process,edit]()
    {
        edit->append( process->readAllStandardOutput().data() );
        edit->append( QString("<font color=\"red\">%1</font>").arg(process->readAllStandardError().data()) );
    });

    QStringList args;
    args << "--datadir" << dir;
    process->start( SettingsForm::getExodusPath(type), args );

    f_processList.push_back( process );
}


void MainWindow::on_action_EditWallet_triggered()
{
    auto selected( queryTableView->selectionModel()->selectedRows() );
    auto item( selected.at(0) );
    WalletForm dlg( this );
    dlg.readDataFromDb(item.data().toInt());
    if( dlg.exec() == QDialog::Accepted )
    {
        refresh();
    }
}


void MainWindow::on_action_DeleteWallet_triggered()
{
    auto selected( queryTableView->selectionModel()->selectedRows() );
    auto result = QMessageBox::question
        ( this
        , tr("About to delete")
        , tr("Are you sure you want to delete the wallet%1? This cannot be undone!")
            .arg(selected.size()==1? "": "s")
        );
    if( result == QMessageBox::No )
    {
        return;
    }

    QSqlDatabase::database().transaction();

    for( auto item : selected )
    {
        QSqlQuery q( "DELETE FROM wallets WHERE id = ?" );
        q.bindValue( 0, item.data().toInt() );
        if( q.exec() )
        {
            refresh();
        }
        else
        {
            qDebug() << "Cannot drop record id=" << item.data().toInt();
            auto err( q.lastError() );
            QMessageBox::critical
                    ( this
                      , tr("Cannot delete row!")
                      , tr("Failed to write to database. Error='%1'")
                      .arg(err.text())
                      );
            QSqlDatabase::database().rollback();
        }
    }

    QSqlDatabase::database().commit();
}


void MainWindow::on_action_Settings_triggered()
{
    SettingsForm dlg(this);
    dlg.exec();
}


void MainWindow::on_action_Exit_triggered()
{
    close();
    QApplication::quit();
}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if( tabWidget->tabText(index).contains("(finished)") )
    {
        tabWidget->removeTab( index );
        if( tabWidget->count() == 0 )
        {
            lowerDockWidget->hide();
        }
    }
    else
    {
        QMessageBox::critical( this, tr("Process still active!"), tr("You cannot remove this while the process is still running!") );
    }
}

void MainWindow::on_sendButton_clicked()
{
    if( orderIdEdit->text().isEmpty() )
    {
        QMessageBox::critical( this, tr("No SS order id!"), tr("You must enter an order ID first!") );
        return;
    }

    QUrl url( QString("https://shapeshift.io/#/status/%1").arg(orderIdEdit->text()) );
    QDesktopServices::openUrl( url );
}

void MainWindow::on_txSendButton_clicked()
{
    if( receiveAddrEdit->text().isEmpty() )
    {
        QMessageBox::critical( this, tr("No receive address specified!"), tr("You must enter a receive address first!") );
        return;
    }

    QUrl url( QString("https://shapeshift.io/txstat/%1").arg(receiveAddrEdit->text()) );
    QDesktopServices::openUrl( url );
}

void MainWindow::on_jsonButton_clicked()
{
    if( orderIdEdit->text().isEmpty() )
    {
        QMessageBox::critical( this, tr("No SS order id!"), tr("You must enter an order ID first!") );
        return;
    }

    QUrl url( QString("https://shapeshift.io/orderInfo/%1").arg(orderIdEdit->text()) );
    QDesktopServices::openUrl( url );
}

void MainWindow::on_xpubButton_clicked()
{
    if( xpubEdit->text().isEmpty() )
    {
        QMessageBox::critical( this, tr("No XPub address!"), tr("You must enter an XPub key first!") );
        return;
    }

    qApp->clipboard()->setText(
                QString("flux.actions.accounts.setHDKeyFromXPub('%1', '%2')")
                    .arg(coinTypeCB->currentText())
                    .arg(xpubEdit->text())
                );
}

void MainWindow::on_ethButton_clicked()
{
    if( ethEdit->text().isEmpty() )
    {
        QMessageBox::critical( this, tr("No XPub address!"), tr("You must enter an XPub key first!") );
        return;
    }

    qApp->clipboard()->setText(
                QString("Exodus.debug.setEthAddress('%1')")
                    .arg(ethEdit->text())
                );
}

void MainWindow::on_cmdButton_clicked()
{
    if( orderIdEdit->text().isEmpty() )
    {
        QMessageBox::critical( this, tr("No SS order id!"), tr("You must enter an order ID first!") );
        return;
    }

    qApp->clipboard()->setText(
                QString("/exchange ticket %1 failed")
                    .arg(orderIdEdit->text())
                );
}

void MainWindow::on_pushButton_clicked()
{
    orderIdEdit->clear();
    receiveAddrEdit->clear();
    xpubEdit->clear();
    ethEdit->clear();
}
