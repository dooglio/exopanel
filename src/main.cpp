#include <iostream>

#include "MainWindow.h"

#include <QtCore>
#include <QtSql>
#include <QtWidgets>


int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);
    app.setApplicationName    ( "exopanel"             );
    app.setApplicationVersion ( EXOPANEL_VERSION       );
    app.setOrganizationDomain ( "exodus.io"            );
    app.setOrganizationName   ( "exodus"               );
    app.setWindowIcon         ( QIcon(":icons/exodus") );

    if( !MainWindow::setupDatabase() )
    {
        return 1;
    }

    MainWindow win;
    win.show();
    return app.exec();
}


// vim: ts=4 sw=4 et
