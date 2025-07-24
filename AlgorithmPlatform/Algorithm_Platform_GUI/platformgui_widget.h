#ifndef PLATFORMGUI_WIDGET_H
#define PLATFORMGUI_WIDGET_H

#include <QWidget>
#include <LitimgWidget/litimgwidget.h>
#include <LitimgWidget/ResultLitWidget.h>
#include <ViewWidget/CustomGraphicsView.h>
#include <ProcessManger/processlist.h>
#include <ThreadAlgorithm/algorithmworker.h>
#include <ThreadAlgorithm/exportworker.h>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class PlatformGUI_Widget; }
QT_END_NAMESPACE

class PlatformGUI_Widget : public QWidget
{
    Q_OBJECT

public:
    PlatformGUI_Widget(QWidget *parent = nullptr);
    ~PlatformGUI_Widget();
protected:
    bool InitWidget();
    bool InitProcessList();
    bool initAlgorithmThread();
    bool initExportThread();

private slots:
    void on_run_algorithm_clicked();
    void exportHandle(bool flag);
    void on_Export_clicked();

    void on_Stop_run_clicked();

    void on_result_reset_clicked();

private:
    void on_changeProcessPanel(QWidget* panel);
    void on_changeProcessView(CustomGraphicsView *customView);
    void Load_Result_Imgs();
private:
    Ui::PlatformGUI_Widget *ui;
    LitimgWidget* litimgWidget;
    ResultLitWidget * resultlitwidget;
    CustomGraphicsView* customview;
    ProcessList * processlist;
    QWidget *lastProcessPanel; // 记录上一次显示的算法面板，在此保存指针，方便在切换面板时先清除上一次的面板

    QThread* algoThread = nullptr;
    AlgorithmWorker* algoWorker = nullptr;

    QThread* exportThread = nullptr;
    ExportWorker* exportWorker = nullptr;
    bool is_stop = false;

};
#endif // PLATFORMGUI_WIDGET_H
