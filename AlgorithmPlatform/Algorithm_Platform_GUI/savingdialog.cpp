#include "savingdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include<QDebug>

SavingDialog::SavingDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("保存中...");
    setModal(true);
    setFixedSize(300, 100);

    auto layout = new QVBoxLayout(this);
    auto label = new QLabel("正在导出，请稍等...", this);
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    layout->addWidget(label);
    layout->addWidget(progressBar);
}

void SavingDialog::setProgress(int value) {
    qDebug() << "value:" << value;
    progressBar->setValue(value);
}
