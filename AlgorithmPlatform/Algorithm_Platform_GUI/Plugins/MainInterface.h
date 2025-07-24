#ifndef MAINTNTERFACE_H_HANDLE
#define MAINTNTERFACE_H_HANDLE
/**************************************************
 *MainInterface是插件的接口类，它是一个纯虚接口类，所有算法插件
 * 均继承该虚类，并必须实现该类定义的五个接口
 **************************************************/
#include <QtPlugin>
#include <QObject>
#include"CustomGraphicsView.h"
class MainInterface : public QObject
{
    Q_OBJECT
public:
    virtual ~MainInterface(){};
    //获取插件名称，我们插件管理器会根据名称来决定，该插件在算法树中的名称以及图标
    virtual QString name() = 0;
    //插件描述，对插件的信息描述，没有实际用处
    virtual QString information() = 0;
    //算法初始化函数，一些算法必须要先初始化一些资源才可以运行，该接口内就需要实现对算法的初始化
    virtual int Init() = 0;
    //算法执行函数，算法每次运行都会调用该接口，如果算法由资源需要初始化，但init没有初始化成功，则该函数不会运行;-1表示所有图都需要
    virtual QVector<QImage> Execute(QVector<QImage> images, int currentrow) = 0;
    //算法面板，获取算法的配置面板
    virtual QWidget* GetProcessPanel(QWidget* parent = nullptr) = 0;
    // 获取自定义的展示面板
    virtual CustomGraphicsView* getCustomGraphicsView()=0;
    // 获取所有执行结果
    virtual QVector<QImage> getAllResult()=0;
    // 导出算法结果
    virtual bool Export(QString dirPath)=0;
    // 中途停止运行
    virtual void requestStop() = 0;
    // 重置所有结果
    virtual void result_reset() = 0;
};

//一些插件需要声明的宏，具体含义可以自行百度
#define APPROCESSTERFACE_IID "com.ap.processPlugin"
Q_DECLARE_INTERFACE(MainInterface, APPROCESSTERFACE_IID)

#endif
