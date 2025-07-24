#ifndef CUSTOMIMAGEVIEW_H
#define CUSTOMIMAGEVIEW_H

#include <QtCore/qglobal.h>
#include <QWidget>

class CustomGraphicsView;
class CustomImageView : public QWidget
{
    Q_OBJECT
public:
    CustomImageView(QWidget *parent = nullptr);
    ~CustomImageView();
    bool InitWidget();

    //获取当前显示的图片
    const QImage & GetImage(){return m_qImage;}
    //设置要显示的图像
    void SetImage(QImage & qImage);
    //设置图片，传入要展示图片的地址
    void SetImage(QString & strPath);

public slots:
     void OnSendImage(QImage &qImage);  // 声明

private:
    CustomGraphicsView *m_pGraphicsview;//视图窗口
    QImage m_qImage; //当前显示图片
};

#endif // CUSTOMIMAGEVIEW_H
