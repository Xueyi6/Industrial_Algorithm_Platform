#ifndef PHOTOMETRIC_UI_H
#define PHOTOMETRIC_UI_H

#include <QWidget>
#include <CustomGraphicsView.h>

namespace Ui {
class Photometric_ui;
}

class Photometric_ui : public QWidget
{
    Q_OBJECT

public:
    explicit Photometric_ui(QWidget *parent = nullptr);
    bool InitPhotometric();
    CustomGraphicsView* getCustomGraphicsView();
    QList<float> getSlant();
    QList<float> getTilt();
    ~Photometric_ui();

private:
    Ui::Photometric_ui *ui;
    CustomGraphicsView* customview;
    QList<float> Slants, Tilts;
};

#endif // PHOTOMETRIC_UI_H
