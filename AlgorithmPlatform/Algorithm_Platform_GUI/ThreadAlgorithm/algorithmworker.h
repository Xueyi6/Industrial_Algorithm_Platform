#ifndef ALGORITHMWORKER_H
#define ALGORITHMWORKER_H

#include <QObject>
#include <Plugins/MainInterface.h>

class AlgorithmWorker : public QObject {
    Q_OBJECT
public:
    AlgorithmWorker();
    void setPlugin(MainInterface* plugin);
    void setInputImgs(QVector<QImage> inputImgs);
    void setCurrentrow(int current_row);
signals:
    void finished();
    void resultReady(QVector<QImage> result_imgs);

public slots:
    void run();  // 调用插件方法
private:
    MainInterface* plugin;
    QVector<QImage> inputImgs;
    int current_row;
};

#endif // ALGORITHMWORKER_H
