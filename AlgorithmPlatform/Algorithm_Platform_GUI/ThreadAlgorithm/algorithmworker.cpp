#include "algorithmworker.h"
#include <QDebug>

AlgorithmWorker::AlgorithmWorker()
{

}

void AlgorithmWorker::setPlugin(MainInterface *Plugin)
{
    plugin = Plugin;
}

void AlgorithmWorker::setInputImgs(QVector<QImage> InputImgs)
{
    inputImgs = InputImgs;
}

void AlgorithmWorker::setCurrentrow(int currentrow)
{
    current_row = currentrow;
}

void AlgorithmWorker::run()
{
    QVector<QImage> result_imgs = plugin->Execute(inputImgs, current_row);
    emit resultReady(result_imgs);
}
