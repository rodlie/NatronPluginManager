#ifndef ADDREPODIALOG_H
#define ADDREPODIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>

#include "plugins.h"

class AddRepoDialog : public QDialog
{
    Q_OBJECT

public:

    explicit AddRepoDialog(QWidget *parent = nullptr,
                           Plugins *plugins = nullptr);

private:

    Plugins *_plugins;
    QLineEdit *_urlEdit;
    QPushButton *_applyButton;
    QPushButton *_cancelButton;
    bool _validRepo;

private slots:

    void handleApplyButton();
};

#endif // ADDREPODIALOG_H
