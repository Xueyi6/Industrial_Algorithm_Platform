#include "CustomGraphicsView.h"
#include <QMutexLocker>
#include <QLayout>
#include <QWheelEvent>
#include "CustomImageItem.h"

#define ZOOMMAX 50   //最大放大倍数
#define ZOOMMIN 0.1 //最小缩小倍数

CustomGraphicsView::CustomGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , m_pScene(Q_NULLPTR)
    , m_pImageItem(Q_NULLPTR)
    , m_pPosInfoWidget(Q_NULLPTR)
    , m_pPosInfoLabel(Q_NULLPTR)
{
    //禁用滑动条
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //反锯齿
    this->setRenderHint(QPainter::Antialiasing);
    this->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    //解决拖动时背景图片残影
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setDragMode(QGraphicsView::ScrollHandDrag);
    this->setSceneRect(INT_MIN/2, INT_MIN/2, INT_MAX, INT_MAX);
    setBackground();
    centerOn(0, 0);

    if(false == InitWidget())
    {
        throw std::bad_alloc();
    }
}

CustomGraphicsView::~CustomGraphicsView()
{
}

bool CustomGraphicsView::InitWidget()
{
    //创建变量对象
    m_pScene = new QGraphicsScene(this);
    m_pImageItem = new CustomImageItem(this);
//    m_pImageItem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    this->setScene(m_pScene);
    m_pScene->addItem(m_pImageItem);
    m_pPosInfoLabel = new QLabel(this);
    m_pPosInfoWidget = new QWidget(this);

    //在视觉窗口下方显示鼠标坐标以及图像的灰度值
    m_pPosInfoLabel->setStyleSheet("color:rgb(200,255,200); "
                              "background-color:rgba(50,50,50,160); "
                              "font: Microsoft YaHei;"
                              "font-size: 15px;");
    m_pPosInfoLabel->setText(" W:0,H:0 | X:0,Y:0 | R:0,G:0,B:0");
    //显示区域窗口
    m_pPosInfoWidget->setFixedHeight(25);
    m_pPosInfoWidget->setGeometry(0, this->height() - 25, this->width(), 25);
    m_pPosInfoWidget->setStyleSheet("background-color:rgba(0,0,0,0);");
    QHBoxLayout* pInfoLayout = new QHBoxLayout();
    pInfoLayout->setSpacing(0);
    pInfoLayout->setContentsMargins(0,0,0,0);
    pInfoLayout->addWidget(m_pPosInfoLabel);
    m_pPosInfoWidget->setLayout(pInfoLayout);

    //初始化信号槽
    connect(m_pImageItem, &CustomImageItem::RGBValue, this, [&](QString InfoVal) {
        m_pPosInfoLabel->setText(InfoVal);
        });

    return true;
}

//为视觉窗口设置图像，是一个公共对外接口
void CustomGraphicsView::SetImage(const QImage &image)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    m_qImage = image.copy();
    m_Image = QPixmap::fromImage(image);
    m_pImageItem->w = m_Image.width();
    m_pImageItem->h = m_Image.height();
    m_pImageItem->setPixmap(m_Image);

    fitFrame();
    onCenter();
    show();
}

//重写鼠标滚轮滚动的事件函数
//主要依赖于Zoom()方法
void CustomGraphicsView::wheelEvent(QWheelEvent *event)
{
    //滚轮的滚动量
    QPoint scrollAmount = event->angleDelta();
    if ((scrollAmount.y() > 0) && (m_dZoomValue >= ZOOMMAX)) //最大放大到原始图像的50倍
    {
       return;
    }
    else if ((scrollAmount.y() < 0) && (m_dZoomValue <= ZOOMMIN))//最小缩小到原始图像的50倍
    {
      return;
    }

    // 正值表示滚轮远离使用者,为放大;负值表示朝向使用者,为缩小
    scrollAmount.y() > 0 ? onZoom(1.1) : onZoom(0.9);
}

//在视觉窗口上双击鼠标左键，会有图像居中效果，主要依赖于onCenter()方法。
void CustomGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        //自适应图像大小至视觉窗口的大小
        fitFrame();
        //居中显示
        onCenter();
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

//绘制函数，用于视觉窗口背景绘制
void CustomGraphicsView::paintEvent(QPaintEvent* event)
{
    QPainter paint(this->viewport());
    //绘制背景
    paint.drawTiledPixmap(QRect(QPoint(0, 0), QPoint(this->width(), this->height())), m_tilePixmap);
    QGraphicsView::paintEvent(event);
}

//当窗口尺寸发生变化时，实时更新视觉窗口位置
void CustomGraphicsView::resizeEvent(QResizeEvent *event)
{
    fitFrame();
    onCenter();
    m_pPosInfoWidget->setGeometry(0, this->height() - 25, this->width(), 25);
    QGraphicsView::resizeEvent(event);
}

//视图居中
void CustomGraphicsView::onCenter()
{
    //调用QGraphicsView自带的方法centerOn，使视觉窗口的中心位于图像元素的中心点
    //并设置m_pImageItem的坐标，使其位于零点
    this->centerOn(m_pImageItem->pixmap().width()/2, m_pImageItem->pixmap().height()/2);
    m_pImageItem->setPos(0, 0);
}

void CustomGraphicsView::onZoom(float scaleFactor)
{
    //记录下当前相对于图像原图的缩放比例，可以记录下当前图像真实放大缩小了多少倍
    //可以借此来限制图像的最大或最小缩放比例
    m_dZoomValue *= scaleFactor;
    //调用视图类QGraphicsView自带的scale缩放方法，来对视图进行缩放，实现放大缩小的功能
    //缩放的同时，视图里的所有元素也会进行缩放，也就达到了视觉窗口放大缩小的效果
    this->scale(scaleFactor, scaleFactor);
}

//图片自适应方法，根据图像原始尺寸和当前视觉窗口的大小计算出应缩放的尺寸，再根据已经缩放的比例计算还差的缩放比例，
//补齐应缩放的比例，使得图像和视觉窗口大小相适配
void CustomGraphicsView::fitFrame()
{
    if (this->width() < 1 || m_Image.width() < 1)
        return;

    //计算缩放比例
    double winWidth = this->width();
    double winHeight = this->height();
    double ScaleWidth = (m_Image.width() + 1) / winWidth;
    double ScaleHeight = (m_Image.height() + 1) / winHeight;
    double s_temp = ScaleWidth >= ScaleHeight ? 1 / ScaleWidth : 1 / ScaleHeight;
    double scale = s_temp / m_dZoomValue;

    onZoom(scale);
    m_dZoomValue = s_temp;
}

//设置视觉窗口背景为棋盘格样式
void CustomGraphicsView::setBackground(bool enabled, bool invertColor)
{
    if (enabled)
    {
        m_tilePixmap.fill(invertColor ? QColor(220, 220, 220) : QColor(35, 35, 35));
        QPainter tilePainter(&m_tilePixmap);
        const QColor color(50, 50, 50, 255);
        const QColor invertedColor(210, 210, 210, 255);
        tilePainter.fillRect(0, 0, 18, 18, invertColor ? invertedColor : color);
        tilePainter.fillRect(18, 18, 18, 18, invertColor ? invertedColor : color);
        tilePainter.end();

        //当取消注释时，视觉窗口背景格会跟随图像一起缩放
        //setBackgroundBrush(m_tilePixmap);
    }
    else
    {
        //setBackgroundBrush(Qt::transparent);
    }
}

