#include "litimgwidget.h"
#include "QListWidget"
#include <QFileDialog>
#include "litimgitemwidget.h"
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QTime>

LitimgWidget::LitimgWidget(QWidget *parent) :
    QWidget(parent)
  , m_pImgListWidget(Q_NULLPTR)
  , m_imagePaths(Q_NULLPTR)
  , mainLayout(Q_NULLPTR)
  , qimge(Q_NULLPTR)
{
    if(false == InitWidget())
    {
        throw std::bad_alloc();
    }
}

bool LitimgWidget::InitWidget(){
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

    connect(m_pImgListWidget, &QListWidget::currentRowChanged, this, &LitimgWidget::OncurrentItemChanged);
    connect(m_pImgListWidget, &QListWidget::itemClicked, this, &LitimgWidget::OnitemClicked);
    return true;
}

void LitimgWidget::onOpenPushbuttonClicked()
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("打开图片"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    fileDialog->setNameFilter(tr("Images(*.png *.jpg *.jpeg *.bmp)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);

    if(fileDialog->exec())
    {
        m_imagePaths = fileDialog->selectedFiles();
        updateLitimages();
    }
    //打印所有选择的文件的路径
    for(auto tmp : m_imagePaths)
        qDebug()<<tmp;
}

void LitimgWidget::OncurrentItemChanged(int currentrow)
{
    if(currentrow < 0 || currentrow > m_imagePaths.size())
        return;
    current_row = currentrow;
    QImage image(m_imagePaths[currentrow]);  // 确保构造有效
    emit sendImg(image);  // 传递已构造的对象
}

//该函数用于刷新缩略图窗口内显示的缩略图，在初次填充缩略图窗口或者
//拖动缩略图窗口改变大小时调用，可更新窗口内缩略图的内容与缩略图的大小
void LitimgWidget::updateLitimages()
{
    //先清空QListWidget
    m_pImgListWidget->clear();
    m_loadImages.clear();
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
    for(auto path : m_imagePaths)
    {
        //初始化缩略图元素窗口
        Litimgitemwidget *litimgItemWidget = new Litimgitemwidget();
        QImage qImage = QImage(path);
        litimgItemWidget->setImage(qImage);
        //定义QListWidgetItem对象
        QListWidgetItem *imageItem = new QListWidgetItem();
        imageItem->setSizeHint(QSize(m_pImgListWidget->height() + 20,m_pImgListWidget->height() - 10));
        m_pImgListWidget->addItem(imageItem);
        m_pImgListWidget->setItemWidget(imageItem,litimgItemWidget);
        m_loadImages.push_back(qImage);
    }
}

//重写了缩略图窗口的resizeEvent事件，当大小变动时，窗口内的缩略图item也要刷新，
//否则显示不美观，具体效果可以注释掉对updateLitimages()函数的调用查看效果。
void LitimgWidget::resizeEvent(QResizeEvent *event)
{

    if (m_imagePaths[0]!= "") {
        updateLitimages();
    }
}


//对外接口，提供给图像处理对象（也就是生产者线程，即该类的类成员m_pImageProducer）使用，
//生产者线程通过该接口获取图像的绝对路径，方便生产者根据路径读取图像并传递给消费者进行图像处理
QString LitimgWidget::getImagePath(int index)
{
    if(index < 0 || index > m_imagePaths.size()-1)
    {
        return "";
    }
    return m_imagePaths[index];
}

//获取缩略图窗口当前的序号，当没有选择任何一个item时，会返回-1
//该接口同样是提供给生产者使用，与getImagePath()接口配合可以获取当前选中图像的绝对路径
int LitimgWidget::getCurrentIndex()
{
    return m_pImgListWidget->currentRow();
}

//槽函数，与processlist的updataProcess信号绑定，当算法列表选中的算法更改时，则会通知缩略图
//来更新绑定的算法对象指针
void LitimgWidget::on_updataProcess(MainInterface* process)
{
    m_pProcess = process;
}

void LitimgWidget::OnitemClicked(QListWidgetItem *item)
{
    OncurrentItemChanged(current_row);
}

//单次运行算法，单次运行或者循环运行算法时，都调用该函数方法
void LitimgWidget::runOnce(bool is_continue)
{
    auto imageIndex = -1;
    //获取图片
    if(!is_continue){
        // 默认是列表元素索引是-1,在这里还判断一次是因为说明是没勾选连续执行
        imageIndex = getCurrentIndex();
        if(imageIndex == -1){
            // 说明没选测定图片
            imageIndex = -999;
        }
    }
    QString currentImagePath = getImagePath(imageIndex);
    QImage QShowImage = QImage(currentImagePath);
    QShowImage = QShowImage.convertToFormat(QImage::Format_RGB888);

    // QTime startTime = QTime::currentTime();
    //算法运行
    MainInterface *tool = m_pProcess;
    algoworker->setPlugin(tool);
    algoworker->setCurrentrow(imageIndex);
    algoworker->setInputImgs(m_loadImages);
    QMetaObject::invokeMethod(algoworker, "run", Qt::QueuedConnection);
    // QTime stopTime = QTime::currentTime();
    // double elapsed = startTime.msecsTo(stopTime);
    // emit updataProcessTime(elapsed);
}
// 导出运行结果
void LitimgWidget::export_result(QString dirpath)
{
    exportworker->setPlugin(m_pProcess);
    exportworker->setFilePath(dirpath);
    QMetaObject::invokeMethod(exportworker, "run", Qt::QueuedConnection);
}
void LitimgWidget::setAlgorithmWorker(AlgorithmWorker *Worker)
{
    algoworker = Worker;
}

void LitimgWidget::setExportWorker(ExportWorker *Worker)
{
    exportworker = Worker;
}
