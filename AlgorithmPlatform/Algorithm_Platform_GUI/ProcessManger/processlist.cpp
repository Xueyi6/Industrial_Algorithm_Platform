#include "processlist.h"
#include <QPluginLoader>
#include <QDir>
#include <QLayout>
#include "Plugins/MainInterface.h"
#include "ViewWidget/CustomGraphicsView.h"
#include <QDebug>
#include <savingdialog.h>
// #include "LogWidget/logmessage.h"
// #include "include/CommonDefine.h"
/**************************************************
 * 算法树模块，也是插件管理器
 * 负责插件的查找、读取、加载，以及插件在界面上的显示，并负责保存、
 * 管理插件对象指针
 **************************************************/

// 实现
ProcessList::ProcessList(QListWidget* listWidget, QWidget* parent)
    : QWidget(parent)
    , m_pProcessListWidget(listWidget)  // 正确初始化
{
    Q_ASSERT(listWidget != nullptr);  // 建议添加断言检查
    //this->setFixedWidth(200);
    //    m_pProcessListWidget->setStyleSheet(QSS_READ(QSS_QLISTWIDGET_PROCESSLIST));

    //    //初始化布局
    //    QVBoxLayout* pListLayout = new QVBoxLayout(this);
    //    pListLayout->setContentsMargins(0,0,0,0);
    //    pListLayout->addWidget(m_pProcessListWidget);
    //    this->setLayout(pListLayout);

    //链接信号槽
    connect(m_pProcessListWidget,&QListWidget::currentRowChanged,
            this,&ProcessList::on_algorithm_listWidget_currentRowChanged);

    //初始化时加载所有插件
    loadAllPlugins();
}

ProcessList::~ProcessList()
{
}

//获取当前算法树所选择的算法
//通过判断ui页面上算法树控件所选择的算法，来得到算法队列里所保存的对应的算法，
//并将该算法返回
MainInterface* ProcessList::GetCurrentTool()
{
    int index = m_pProcessListWidget->currentRow();
    if((index < m_tools.size()) && index >= 0)
    {
        return m_tools[index];
    }
    return nullptr;
}

//当算法树选中算法更改时，会进入该槽函数
void ProcessList::on_algorithm_listWidget_currentRowChanged(int currentRow)
{
    // QDEBUG("currentRow：" + QString::number(currentRow));
    if(currentRow >= 0 && currentRow < m_tools.size())
    {
        QWidget *panel = m_tools[currentRow]->GetProcessPanel();

        //发送信号给主窗口，让主窗口刷新算法面板
        emit changeProcessPanel(panel);
        qDebug()<< "发送customview";

        //发送信号给缩略图，更新缩略图所绑定的算法
        MainInterface *process = m_tools[currentRow];
        emit updataProcess(process);

        // 发送信号给主窗口，让主窗口面板重新绑定新的customview
        CustomGraphicsView *customview = m_tools[currentRow]->getCustomGraphicsView();
        emit changeProcessView(customview);

        // 发送信号给主窗口，让结果列表切换到改算法的运行结果
        QVector<QImage> m_dstImages = m_tools[currentRow]->getAllResult();
        emit changeResultList(m_dstImages);
    }
}

//加载所有插件，主要依赖于loadPlugin()方法
//先获取/plugins目录下的所有算法插件dll,再对每一个进行加载
void ProcessList::loadAllPlugins()
{
    // QDEBUG("currentPath:" + QDir::currentPath());
    QDir pluginsdir(QDir::currentPath());
    pluginsdir.cd("plugins");
    QFileInfoList pluginsInfo = pluginsdir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    //遍历所有动态库，一一加载插件
    for(QFileInfo fileinfo : pluginsInfo)
    {
        loadPlugin(fileinfo.absoluteFilePath());
    }
}

//加载其中某个插件
void ProcessList::loadPlugin(const QString &libraryPath)
{
    if(!QLibrary::isLibrary(libraryPath))
    {
        return;
    }
    qDebug() << "current_Plugin_Path:" + libraryPath;
    // QDEBUG("current_Plugin_Path:" + libraryPath);
    //加载插件
    QPluginLoader *loader = new QPluginLoader(libraryPath);
    QString plugin_name, plugin_information;//保存插件信息用于显示
    if(loader->load())
    {
        MainInterface *plugin = qobject_cast<MainInterface *>(loader->instance());
        if(plugin)
        {
            plugin_name = plugin->name();
            plugin_information = plugin->information();
            // QDEBUG("plugin_name:" + plugin_name + "plugin_information:" + plugin_information);

            m_loaders.insert(libraryPath, loader);
            m_names.insert(libraryPath, plugin_name);
            //将算法指针保存到算法工具队列
            //loadDll(libraryPath);
            m_tools.push_back(plugin);
            //刷新算法在算法树显示
            populateTree(plugin_name, plugin_information);
        }
        else
        {
            delete loader;
            loader = nullptr;
        }
    }
    else
    {
        // QWARNING("load plugin failed:" + loader->errorString());
    }
}

//卸载所有插件
void ProcessList::unloadAllPlugins()
{
    for(QString libraryPath : m_loaders.keys())
    {
        unloadPlugin(libraryPath);
    }
}

void ProcessList::unloadPlugin(const QString &libraryPath)
{
    QPluginLoader *loader = m_loaders.value(libraryPath);
    //卸载插件，并从内部数据结构中移除
    if(loader->unload())
    {
        m_loaders.remove(libraryPath);
        delete loader;
        loader = nullptr;
    }
}

void ProcessList::populateTree(const QString &name, const QString &information)
{
    QFont font;//创建字体
    font.setFamily("Microsoft YaHei");
    font.setPointSize(10);

    QListWidgetItem* item = new QListWidgetItem();
    item->setFont(font);
    item->setText(getNameCN(name));//对插件名称的显示要中文字符转码
    item->setToolTip(information);
    item->setIcon(getPluginIcon(name));
    // 当所有插件添加完毕后，会自动触发currentRow信号，走算法改变函数（默认currentrow为1）
    m_pProcessListWidget->addItem(item);
}

//辅助函数，通过插件名称来查找该插件工具对应的图标
//当名称没有找到时，会以自定义工具的图标来替代
QIcon ProcessList::getPluginIcon(const QString &Name)
{
    QString IconName;
    if (Name.contains("ImageShow")) IconName = ":/PluginsIcon/ImageShow.png";
    else if (Name.contains("Photometric")) IconName = ":/icon/PluginIcon/Photometric.png";
    else if (Name.contains("PcbDetect")) IconName = ":/icon/PluginIcon/PCBDETECT.png";
    else if (Name.contains("Contour")) IconName = ":/icon/PluginIcon/Contour.png";
    else if (Name.contains("TemplateMatch")) IconName = ":/PluginsIcon/TemplateMatch.png";
    else if (Name.contains("Canny")) IconName = ":/icon/PluginIcon/Canny.png";
    else if (Name.contains("LineDetection")) IconName = ":/PluginsIcon/LineDetection.png";
    else IconName = ":/PluginsIcon/CustomComponent.png";

    return QIcon(IconName);

}

//辅助函数，通过传入插件的名称(英文)，来得到对应的中文名称
//该中文名称不是插件的唯一标识，而是用于在界面显示的工具名称
//当匹配不到对应名称时，则以原本的英文名称来替代
QString ProcessList::getNameCN(const QString &Name)
{
    QString NameCN;
    if (Name.contains("ImageShow")) NameCN = QStringLiteral("图像显示");
    else if (Name.contains("Photometric")) NameCN = QStringLiteral("光度立体");
    else if (Name.contains("PcbDetect")) NameCN = QStringLiteral("PCB缺陷检测");
    else if (Name.contains("Contour")) NameCN = QStringLiteral("轮廓检测");
    else if (Name.contains("Canny")) NameCN = QStringLiteral("Canny边缘检测");
    else if (Name.contains("LineDetection")) NameCN = QStringLiteral("直线检测");
    else if (Name.contains("CircleDetection")) NameCN = QStringLiteral("圆检测");
    else if (Name.contains("CaliperMeasure")) NameCN = QStringLiteral("卡尺测量");
    else if (Name.contains("TemplateMatch")) NameCN = QStringLiteral("模板匹配");
    else NameCN = Name;

    return NameCN;
}

//根据插件来获取某个插件名称，未使用
QVariant ProcessList::getPluginName(QPluginLoader *loader)
{
    if(loader)
        return m_names.value(m_loaders.key(loader));
    else
        return "";
}

//根据名称获得插件，未使用
QPluginLoader *ProcessList::getPlugin(const QString &name)
{
    return m_loaders.value(m_names.key(name));
}
