#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "dialog.h"

 Dialog::Dialog(QWidget *parent)
     : QDialog(parent)
 {
     statusLabel = new QLabel;
     quitButton = new QPushButton(tr("Выход"));

     if (!server.listen(QHostAddress::Any, 5500)) {
         QMessageBox::critical(this, tr("A multi-threaded client of a game n-Puzzle"),
                               tr("It's impossible to start server: %1.")
                               .arg(server.errorString()));
         close();
         return;
     }

     statusLabel->setText(tr("A server has started and available on port %1.\n"
                             "Now, time to launch client nPuzzle")
                          .arg(server.serverPort()));

     connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

     QHBoxLayout *buttonLayout = new QHBoxLayout;
     buttonLayout->addStretch(1);
     buttonLayout->addWidget(quitButton);
     buttonLayout->addStretch(1);

     QVBoxLayout *mainLayout = new QVBoxLayout;
     mainLayout->addWidget(statusLabel);
     mainLayout->addLayout(buttonLayout);
     setLayout(mainLayout);
     setWindowTitle(tr("A multiuser server of a game n-Puzzle"));
 }
