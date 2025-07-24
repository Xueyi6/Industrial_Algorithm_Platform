#include "platformgui_widget.h"
#include "ui_platformgui_widget.h"
#include "ViewWidget/CustomGraphicsView.h"
#include<QImage>
#include<QDebug>
#include <QThread>
#include <QMessageBox>
#include <QFileDialog>

PlatformGUI_Widget::PlatformGUI_Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlatformGUI_Widget)
    , litimgWidget(Q_NULLPTR)
    , customview(Q_NULLPTR)
    , processlist(Q_NULLPTR)
    , resultlitwidget(Q_NULLPTR)
    , lastProcessPanel(Q_NULLPTR)
{
    ui->setupUi(this);
    ui->tabWidget->clear();
    ui->tabWidget->setBaseSize(ui->tabWidget->width(), 100);
    ui->is_continuous->setChecked(false);
    ui->Stop_run->setEnabled(false);
    if(InitWidget() == false)
    {
        throw std::bad_alloc();
    }
    if(InitProcessList() == false)
    {
        throw std::bad_alloc();
    }
    if(initAlgorithmThread() == false)
    {
        throw std::bad_alloc();
    }

    if(initExportThread() == false)
    {
        throw std::bad_alloc();
    }
    qRegisterMetaType<QVector<QImage>>("QVector<QImage>");

}

PlatformGUI_Widget::~PlatformGUI_Widget()
{
    delete ui;
}

bool PlatformGUI_Widget::InitWidget()
{
    // 加载右下角组件
    litimgWidget = new LitimgWidget(this);
    resultlitwidget = new ResultLitWidget(this);
    ui->tabWidget->addTab(litimgWidget, tr("load_imgs"));
    ui->tabWidget->addTab(resultlitwidget, tr("result_imgs"));
    connect(ui->load_img_btn, &QPushButton::clicked, litimgWidget, &LitimgWidget::onOpenPushbuttonClicked);

    if (!ui->img_show_widget->layout()) {
        // 创建一个新的布局，例如水平布局
        QHBoxLayout *layout = new QHBoxLayout(ui->img_show_widget);
        layout->setContentsMargins(0, 0, 0, 0); // 去掉边距
        ui->img_show_widget->setLayout(layout);
    }
    customview = new CustomGraphicsView();
    // ui->img_show_widget->layout()->addWidget(customview);

    connect(litimgWidget, &LitimgWidget::sendImg, customview, &CustomGraphicsView::SetImage);
    connect(resultlitwidget, &ResultLitWidget::sendImg, customview, &CustomGraphicsView::SetImage);
    connect(resultlitwidget, &ResultLitWidget::load_result_img, this, &PlatformGUI_Widget::Load_Result_Imgs);
    return true;
}

// 承担加载算法树的职责
bool PlatformGUI_Widget::InitProcessList()
{
    processlist = new ProcessList(ui->ProcessList, this);
    connect(processlist, &ProcessList::changeProcessPanel, this, &PlatformGUI_Widget::on_changeProcessPanel);
    connect(processlist, &ProcessList::changeProcessView, this, &PlatformGUI_Widget::on_changeProcessView);
    connect(processlist, &ProcessList::updataProcess, litimgWidget, &LitimgWidget::on_updataProcess);
    connect(processlist, &ProcessList::changeResultList, resultlitwidget, &ResultLitWidget::on_changeResultList);
    return true;
}


//当算法树切换算法时，该槽函数切换主窗口显示的算法面板
void PlatformGUI_Widget::on_changeProcessPanel(QWidget* panel)
{
    // QDEBUG("change process panel");
    if(panel == nullptr)
    {
        return;
    }

    // 先删除初始化查看图片的控件（未做）
    auto layout = ui->img_show_widget->layout();
    // 删除布局中所有的控件
    if(lastProcessPanel != nullptr)
    {
        lastProcessPanel->setParent(nullptr);
    }
    lastProcessPanel = panel;
    layout->addWidget(panel);
}

void PlatformGUI_Widget::on_changeProcessView(CustomGraphicsView *customView)
{
    if (customview) {
        disconnect(litimgWidget, &LitimgWidget::sendImg, customview, &CustomGraphicsView::SetImage);
        disconnect(resultlitwidget, &ResultLitWidget::sendImg, customview, &CustomGraphicsView::SetImage);
        customview = Q_NULLPTR;
        qDebug() << "PlatformGUI_Widget::on_changeProcessView";
    }
    if(litimgWidget->m_pProcess->name().contains("Photom")){
        ui->is_continuous->setEnabled(false);
    }else {
        ui->is_continuous->setEnabled(true);
    }
    customview = customView;
    connect(litimgWidget, &LitimgWidget::sendImg, customview, &CustomGraphicsView::SetImage);
    connect(resultlitwidget, &ResultLitWidget::sendImg, customview, &CustomGraphicsView::SetImage);
}

void PlatformGUI_Widget::Load_Result_Imgs()
{
    ui->Stop_run->setEnabled(false);
    if (resultlitwidget->last_result_img.isNull()){
        if(litimgWidget->m_pProcess->name().contains("Photom")){
            QMessageBox::warning(litimgWidget, tr("运行错误"),tr("图片数量不足，数量需: %1").arg(4));
        }
        else {
            QMessageBox::warning(litimgWidget, tr("运行错误"), tr("未选择测定图片!!!"));
        }
        return;
    }
    customview->SetImage(resultlitwidget->last_result_img);
    if(is_stop){
        // 来个弹窗，提示算法已停止运行
        is_stop = !is_stop;
        QMessageBox::warning(resultlitwidget,tr("停止运行"),QString("已停止执行该算法"));
    }
    resultlitwidget->last_result_img = QImage();
    resultlitwidget->updateLitimages();
}

bool PlatformGUI_Widget::initAlgorithmThread()
{
    if (algoThread) return true; // 已初始化就不重复做

    algoWorker = new AlgorithmWorker();  // 不带参数
    algoThread = new QThread();
    litimgWidget->setAlgorithmWorker(algoWorker);
    algoWorker->moveToThread(algoThread);
    connect(algoThread, &QThread::finished, algoThread, &QObject::deleteLater);
    connect(algoWorker, &AlgorithmWorker::resultReady, resultlitwidget, &ResultLitWidget::handleResult);
    algoThread->start();
    return true;
}

bool PlatformGUI_Widget::initExportThread()
{
    if (exportThread) return true; // 已初始化就不重复做
    exportWorker = new ExportWorker();  // 不带参数
    exportThread = new QThread();
    litimgWidget->setExportWorker(exportWorker);
    exportWorker->moveToThread(exportThread);
    connect(exportThread, &QThread::finished, exportThread, &QObject::deleteLater);
    connect(exportWorker, &ExportWorker::resultReady, this, &PlatformGUI_Widget::exportHandle);
    exportThread->start();
    return true;
}

void PlatformGUI_Widget::on_run_algorithm_clicked()
{
    initAlgorithmThread();
    bool is_continue = ui->is_continuous->isChecked();
    if (is_continue){
        ui->Stop_run->setEnabled(true);
    }
    litimgWidget->runOnce(is_continue);
}

void PlatformGUI_Widget::on_Export_clicked()
{
    initExportThread();
    if (resultlitwidget->m_dstImages.size()==0){
        QMessageBox::warning(resultlitwidget,tr("导出警告"),tr("当前算法无结果可导出!"));
        return;
    }
    // 1. 弹出文件夹选择对话框
    QString dirPath = QFileDialog::getExistingDirectory(litimgWidget, tr("选择保存目录"), "", QFileDialog::ShowDirsOnly);
    // 如果用户取消了选择，返回
    if (dirPath.isEmpty()) {
        return;
    }
    ui->Export->setEnabled(false);
    litimgWidget->export_result(dirPath);
}

void PlatformGUI_Widget::exportHandle(bool flag)
{
    if (flag) {
        QMessageBox::information(resultlitwidget,
                                 tr("导出成功"),
                                 tr("共导出图像数: %1 ").arg(resultlitwidget->m_dstImages.count()));
    } else {
        QMessageBox::warning(resultlitwidget,
                             tr("导出错误"),
                             tr("请重试!"));
    }
    ui->Export->setEnabled(true);
}

void PlatformGUI_Widget::on_Stop_run_clicked()
{
    litimgWidget->m_pProcess->requestStop();
    ui->Stop_run->setEnabled(false);
    is_stop = !is_stop;
}

// 结果重置
void PlatformGUI_Widget::on_result_reset_clicked()
{
    litimgWidget->m_pProcess->result_reset();
    resultlitwidget->m_dstImages.clear();
    resultlitwidget->updateLitimages();

}
