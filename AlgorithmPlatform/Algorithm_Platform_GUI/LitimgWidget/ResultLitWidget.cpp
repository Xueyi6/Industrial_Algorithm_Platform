#include "ResultLitWidget.h"
#include "QListWidget"
#include <QFileDialog>
#include "litimgitemwidget.h"
#include <QDebug>
#include <QMouseEvent>
#include <QApplication>

ResultLitWidget::ResultLitWidget(QWidget *parent) :
    QWidget(parent)
  , m_pImgListWidget(Q_NULLPTR)
  , m_imagePaths(Q_NULLPTR)
  , mainLayout(Q_NULLPTR)
{
    if(false == InitWidget())
    {
        throw std::bad_alloc();
    }
}

bool ResultLitWidget::InitWidget(){
    // 1. 创建主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 移除边距
    mainLayout->setSpacing(0); // 移除间距

    // 2. 创建ListWidget
    m_pImgListWidget = new QListWidget(this);

    // 3. 设置尺寸策略 - 关键修改
    m_pImgListWidget->setSizePolicy(
                QSizePolicy::Expanding,  // 水平：尽可能扩展
                QSizePolicy::Expanding   // 垂直：尽可能扩展
                );

    // 4. 应用您的原始设置（调整部分）
    m_pImgListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pImgListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pImgListWidget->setFlow(QListView::LeftToRight);
    m_pImgListWidget->setViewMode(QListView::ViewMode::ListMode);
    m_pImgListWidget->setSelectionMode(QListView::SingleSelection);
    m_pImgListWidget->setFocusPolicy(Qt::NoFocus);
    m_pImgListWidget->setSelectionRectVisible(false);
    m_pImgListWidget->setHorizontalScrollMode(QListView::ScrollPerPixel);
    m_pImgListWidget->setDragEnabled(false);
    m_pImgListWidget->setSpacing(4);

    // 5. 添加到布局
    mainLayout->addWidget(m_pImgListWidget);

    // 6. 设置LitimgWidget的布局
    setLayout(mainLayout);

    connect(m_pImgListWidget, &QListWidget::currentRowChanged, this, &ResultLitWidget::OncurrentItemChanged);
    connect(m_pImgListWidget, &QListWidget::itemClicked, this, &ResultLitWidget::OnitemClicked);

    return true;
}


void ResultLitWidget::OncurrentItemChanged(int currentrow)
{
    if(currentrow < 0 || currentrow > m_dstImages.size())
        return;
    current_row = currentrow;
    QImage image(m_dstImages[currentrow]);  // 确保构造有效
    emit sendImg(image);  // 传递已构造的对象
}

void ResultLitWidget::on_changeResultList(QVector<QImage> m_DstImages)
{
    if (m_dstImages.size() != 0){
        m_dstImages.clear();
    }
    m_dstImages = m_DstImages;
    updateLitimages();
}

void ResultLitWidget::OnitemClicked(QListWidgetItem *item)
{
    OncurrentItemChanged(current_row);
}

//该函数用于刷新缩略图窗口内显示的缩略图，在初次填充缩略图窗口或者
//拖动缩略图窗口改变大小时调用，可更新窗口内缩略图的内容与缩略图的大小
void ResultLitWidget::updateLitimages()
{
    qDebug()<< "updateLitimages size:";
    qDebug()<< m_dstImages.size();
    //先清空QListWidget
    m_pImgListWidget->clear();
    //设置QListWidget的显示模式
    m_pImgListWidget->setViewMode(QListView::ListMode);
    //设置QListWidget中单元项的图片大小
    //imageList->setIconSize(QSize(imageList->height()-100,imageList->height()-100));
    //设置QListWidget中单元项的间距
    m_pImgListWidget->setSpacing(5);
    //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
    m_pImgListWidget->setResizeMode(QListWidget::Fixed);
    m_pImgListWidget->setItemAlignment(Qt::AlignCenter);
    //设置不能移动
    m_pImgListWidget->setMovement(QListWidget::Static);
    for(auto qImage : m_dstImages)
    {
        //初始化缩略图元素窗口
        Litimgitemwidget *litimgItemWidget = new Litimgitemwidget();
        //        QImage qImage = QImage(path);
        litimgItemWidget->setImage(qImage);
        //定义QListWidgetItem对象
        QListWidgetItem *imageItem = new QListWidgetItem();
        imageItem->setSizeHint(QSize(m_pImgListWidget->height() + 20,m_pImgListWidget->height() - 10));
        m_pImgListWidget->addItem(imageItem);
        m_pImgListWidget->setItemWidget(imageItem,litimgItemWidget);
    }
}

//重写了缩略图窗口的resizeEvent事件，当大小变动时，窗口内的缩略图item也要刷新，
//否则显示不美观，具体效果可以注释掉对updateLitimages()函数的调用查看效果。
void ResultLitWidget::resizeEvent(QResizeEvent *event)
{

    if (m_imagePaths[0]!= "") {
        updateLitimages();
    }
}

void ResultLitWidget::handleResult(QVector<QImage> result_imgs)
{
    if (result_imgs.size()>0){
        for(QImage qimg:result_imgs){
            m_dstImages.push_back(qimg);
        }
        last_result_img = result_imgs[0];
    }
    emit load_result_img();
}

