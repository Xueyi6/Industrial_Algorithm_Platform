#ifndef CUSTOMIMAGEITEM_H
#define CUSTOMIMAGEITEM_H
/**************************************************
 * 图像元素
 * 其实和roi元素是同一类东西，都是在视觉窗口内显示的元素item
 *
 **************************************************/

#include <QGraphicsPixmapItem>


//通过鼠标点选获取当前灰度值
class CustomImageItem :public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    CustomImageItem(QWidget *parent = nullptr);

protected:
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

public:
    int w;
    int h;

signals:
    void RGBValue(QString InfoVal);
};

#endif // CUSTOMIMAGEITEM_H
