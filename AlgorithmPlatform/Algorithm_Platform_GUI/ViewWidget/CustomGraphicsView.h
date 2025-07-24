#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H
/**************************************************
 *
 *重写视图类，该类为视觉窗口的核心代码
 *
 **************************************************/

#include <QWidget>
#include <QGraphicsView>
#include <QEvent>
#include <QLabel>

class CustomImageItem;
class CustomGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    CustomGraphicsView(QWidget *parent = 0);
    ~CustomGraphicsView();
    //界面初始化
    bool InitWidget();
    //设置视觉窗口的图像
    void SetImage(const QImage & qImage);

protected:
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

public slots:
    //视图居中显示
    void onCenter();
    //视图缩放
    void onZoom(float fScaleFactor);
private:
    //辅助函数:自适应大小
    void fitFrame();
    void setBackground(bool enabled = true,bool invertColor = false);

private:
    double m_dZoomValue = 1;

    QGraphicsScene* m_pScene;//场景
    CustomImageItem* m_pImageItem;//图像元素
    QWidget* m_pPosInfoWidget;//视觉窗口左下方，用于显示鼠标位置以及对应位置像素灰度值
    QLabel* m_pPosInfoLabel; //显示灰度值的标签

    QPixmap m_Image;//视觉窗口所显示的图像
    QImage m_qImage;
    QPixmap m_tilePixmap = QPixmap(36, 36);//背景图片方格
};

#endif // CUSTOMGRAPHICSVIEW_H
