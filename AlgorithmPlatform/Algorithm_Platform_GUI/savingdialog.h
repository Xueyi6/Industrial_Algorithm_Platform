#ifndef SAVINGDIALOG_H
#define SAVINGDIALOG_H

#include <QObject>
#include <QWidget>
#include <QProgressBar>
#include <QDialog>

class SavingDialog : public QDialog {
    Q_OBJECT
public:
    explicit SavingDialog(QWidget *parent = nullptr);

public slots:
    void setProgress(int value);

private:
    QProgressBar *progressBar;
};

#endif // SAVINGDIALOG_H
