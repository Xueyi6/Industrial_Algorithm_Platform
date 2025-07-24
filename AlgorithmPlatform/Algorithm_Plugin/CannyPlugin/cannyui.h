#ifndef CANNYUI_H
#define CANNYUI_H

#include <QWidget>
#include "CustomGraphicsView.h"
#include <QString>

namespace Ui {
class CannyUi;
}

class CannyUi : public QWidget
{
    Q_OBJECT

public:
    explicit CannyUi(QWidget *parent = nullptr);
    ~CannyUi();
    bool InitCanny();
    CustomGraphicsView* getCustomGraphicsView();
    int get_max_threshold();
    int get_min_threshold();
    int get_kernel_size();
    bool get_paradigm();
    bool get_api_checked();

private:
    Ui::CannyUi *ui;
    CustomGraphicsView* customview;

    int max_threshold, min_threshold;
    QString kernel_size, paradigm;
};

#endif // CANNYUI_H
