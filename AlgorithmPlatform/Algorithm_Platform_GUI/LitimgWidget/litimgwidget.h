#ifndef LITIMGWIDGET_H
#define LITIMGWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "litimgitemwidget.h"
#include "Plugins/MainInterface.h"
#include <ThreadAlgorithm/algorithmworker.h>
#include <ThreadAlgorithm/exportworker.h>
#include <QVBoxLayout>
#include <QTimer>
#include <QImage>

class LitimgWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LitimgWidget(QWidget *parent = nullptr);
    void runOnce(bool is_continue);
    void export_result(QString dirpath);
    void setAlgorithmWorker(AlgorithmWorker* Worker);
    void setExportWorker(ExportWorker* Worker);
    int getCurrentIndex();
protected:
    bool InitWidget();

signals:
    void sendImg(const QImage& img);  // 推荐使用const引用
public slots:
    void onOpenPushbuttonClicked();
    void OncurrentItemChanged(int currentrow);
    void on_updataProcess(MainInterface *porcess);
    void OnitemClicked(QListWidgetItem *item);

private:
    void updateLitimages();
    void resizeEvent(QResizeEvent*) override;
    QString getImagePath(int index);
public:
    QImage qimge;
    QListWidget* m_pImgListWidget;
    QStringList m_imagePaths;//存储缩略图读取的图像的绝对路径
    QVBoxLayout *mainLayout; // 添加主布局
    MainInterface *m_pProcess;
    AlgorithmWorker *algoworker;
    ExportWorker *exportworker;
    // 运行结果图
    // QImage m_dstImage;
    // 前者是图片加载时保存的，后者是算法运行结果保存
    QVector<QImage> m_loadImages;
    int current_row ;

};

#endif // LITIMGWIDGET_H
