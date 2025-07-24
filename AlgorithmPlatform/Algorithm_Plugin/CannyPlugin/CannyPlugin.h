#ifndef CANNYPLUGIN_H
#define CANNYPLUGIN_H

#include "CannyPlugin_global.h"
#include "MainInterface.h"
#include "cannyui.h"

class CANNYPLUGIN_EXPORT CannyPlugin: public MainInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID APPROCESSTERFACE_IID)
    Q_INTERFACES(MainInterface)
public:
    CannyPlugin();
    ~CannyPlugin();
    //插件名称
    virtual QString name() override;
    //插件描述
    virtual QString information() override;
    //算法初始化函数
    virtual int Init() override;
    //算法执行函数
    virtual QVector<QImage> Execute(QVector<QImage> images, int currentrow) override;
    //算法面板
    virtual QWidget* GetProcessPanel(QWidget* parent = nullptr) override;
    virtual CustomGraphicsView* getCustomGraphicsView() override;
    // 获取所有执行结果
    virtual QVector<QImage> getAllResult() override;
    // 导出算法结果
    virtual bool Export(QString dirPath) override;
    // 中途停止运行
    virtual void requestStop() override;
    // 重置所有结果
    virtual void result_reset() override;
private:
    void run_algorithm(QImage qimg, int max_threshold, int min_threshold, int kernel_sie, bool L2);
private:
    QWidget *mGenWidget;
    CustomGraphicsView *customView;
    // 保存当前执行结果
    QImage m_dstImage;
    // 保存所有执行结果
    QVector<QImage> m_dstImages;
    // 临时结果存储
    QVector<QImage> result_imgs;
    CannyUi * cannyUi;
    bool m_stop = false;
};

#endif // CANNYPLUGIN_H
