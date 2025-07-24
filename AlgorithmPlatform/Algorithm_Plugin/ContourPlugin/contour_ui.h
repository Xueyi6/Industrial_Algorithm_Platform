#ifndef CONTOUR_UI_H
#define CONTOUR_UI_H

#include <QWidget>
#include "CustomGraphicsView.h"

namespace Ui {
class Contour_ui;
}

class Contour_ui : public QWidget
{
    Q_OBJECT
public:
    explicit Contour_ui(QWidget *parent = nullptr);
    ~Contour_ui();
    bool InitContour();
    CustomGraphicsView *getCustomGraphicsView();
    QString get_contour_display_model();
    QString get_contour_detect_model();
    int get_gauss_size();
    int get_max_threshold();
    int get_min_threshold();
    int get_segment_threshold();
    double get_min_area_threshold();
    bool is_canny();
    bool get_is_highlight();
    bool get_is_gauss();
    bool drawRotatedRect();
    bool drawArea();
    bool drawBoundingRect();
private slots:

    void on_is_segment_clicked(bool checked);

    void on_is_gauss_clicked(bool checked);

    void on_is_canny_clicked(bool checked);

private:
    Ui::Contour_ui *ui;
    CustomGraphicsView *customview;
//    QString contour_display_model, contour_detect_model, gauss_size;
//    int max_threshold, min_threshold;
//    bool is_gauss, is_highlight;
};

#endif // CONTOUR_UI_H
