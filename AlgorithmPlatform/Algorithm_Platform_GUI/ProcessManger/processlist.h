#ifndef PROCESSLIST_H
#define PROCESSLIST_H
/**************************************************
 *ProcessList是自定义的算法树模块
 * 该类除了负责算法工具在页面的显示外，还负责插件算法的加载和保存
 * 因此该类也是一个插件管理器类
 * 算法树的初始化主要依赖于函数populateTree()
 * 算法插件的加载则依赖与函数loadPlugin()
 **************************************************/
#include <QWidget>
#include <QHash>
#include <QMap>
#include <QLibrary>
#include <QPluginLoader>
#include <QListWidget>
#include "Plugins/MainInterface.h"

class ProcessList : public QWidget
{
    Q_OBJECT
public:
    // explicit ProcessList(QWidget *parent = nullptr);

    // 头文件声明
    explicit ProcessList(QListWidget* listWidget, QWidget* parent = nullptr);
    ~ProcessList();
    MainInterface* GetCurrentTool();

signals:
    void changeProcessPanel(QWidget* panel);
    void updataProcess(MainInterface* process);
    void changeResultList(QVector<QImage> m_dstImages);
    void changeProcessView(CustomGraphicsView *customview);

private slots:
    void on_algorithm_listWidget_currentRowChanged(int currentRow);

private:
    void loadAllPlugins();
    void unloadAllPlugins();
    void loadPlugin(const QString &filepath);
    void unloadPlugin(const QString &filepath);
    void populateTree(const QString &name, const QString &information);
    QIcon getPluginIcon(const QString &name);
    QString getNameCN(const QString &Name);
    QPluginLoader* getPlugin(const QString &name);
    QVariant getPluginName(QPluginLoader *loader);

private:
    QListWidget* m_pProcessListWidget;
    QHash<QString, QPluginLoader *> m_loaders; //插件路径--QPluginLoader实例
    QHash<QString, QString> m_names; //插件路径--插件名称
    QMap<QString, int> m_processId;//插件名称--算法列表序号
    QVector<MainInterface*> m_tools;
};

#endif // PROCESSLIST_H
