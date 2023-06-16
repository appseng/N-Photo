#include "aboutdialogimpl.h"

AboutDialogImpl::AboutDialogImpl()
        :QDialog()
{
    setupUi(this);
    connect(pushButton, SIGNAL(clicked()), this, SLOT(close()));
}
