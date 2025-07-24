#ifndef EXPORTWORKER_H
#define EXPORTWORKER_H

#include <QObject>
#include <Plugins/MainInterface.h>

class ExportWorker : public QObject
{
    Q_OBJECT
public:
    ExportWorker();
    void setPlugin(MainInterface* plugin);
    void setFilePath(QString filepath);
signals:
    void finished();
    void resultReady(bool result);

public slots:
    void run();  // 调用插件方法
private:
    MainInterface* plugin;
    QString file_path;
};

#endif // EXPORTWORKER_H
