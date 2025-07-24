#ifndef LITIMGITEMWIDGET_H
#define LITIMGITEMWIDGET_H

#include <QWidget>

class Litimgitemwidget : public QWidget
{
    Q_OBJECT
public:
    Litimgitemwidget(QWidget *parent = nullptr);
    ~Litimgitemwidget();
    bool InitWidget();

    //设置图像
    void setImage(QImage& qImage);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
private:
    QImage m_qImg;
    QString m_strName;

};

#endif // LITIMGITEMWIDGET_H
