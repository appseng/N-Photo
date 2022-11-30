#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include "server.h"

class Dialog : public QDialog
 {
     Q_OBJECT
 public:
     Dialog(QWidget * = 0);
 private:
     QLabel *statusLabel;
     QPushButton *quitButton;
     Server server;
 };

 #endif
