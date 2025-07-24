#include "exportworker.h"
#include <QDebug>
ExportWorker::ExportWorker()
{

}
void ExportWorker::setPlugin(MainInterface *Plugin)
{
    plugin = Plugin;
}

void ExportWorker::setFilePath(QString filepath)
{
    file_path = filepath;
}

void ExportWorker::run()
{
    qDebug()<< "33333333333333333";
    bool flag = plugin->Export(file_path);
    emit resultReady(flag);
}


