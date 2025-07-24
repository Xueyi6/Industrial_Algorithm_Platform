#include "litimgitemwidget.h"
#include <QPainter>
#include <QPixmap>

Litimgitemwidget::Litimgitemwidget(QWidget *parent)
    : QWidget(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
    if(false == InitWidget())
    {
        throw std::bad_alloc();
    }
}

Litimgitemwidget::~Litimgitemwidget()
{
}

bool Litimgitemwidget::InitWidget()
{
    return true;
}

void Litimgitemwidget::setImage(QImage& qImage)
{
    m_qImg = qImage;
}

void Litimgitemwidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int nDevceWidth = this->width();
    int nDeviceHeight = this->height();
    //绘制图像
    if(!m_qImg.isNull())
    {
        QPixmap tempPixmap = QPixmap::fromImage(m_qImg);
        painter.drawPixmap(2,2,nDevceWidth-4,nDeviceHeight-4,tempPixmap);
    }
    //绘制图像外围矩形
    QPen pen;
    pen.setColor(QColor(this->hasFocus() ? Qt::red : Qt::black));//设置笔颜色
    pen.setWidth(4);//设置笔宽度
    painter.setPen(pen);//设置为要绘制的笔
    painter.drawRect(QRect(2,2,nDevceWidth-4,nDeviceHeight-4));
}
