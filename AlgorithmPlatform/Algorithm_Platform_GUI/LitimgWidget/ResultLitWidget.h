#ifndef RESULTLITWIDGET_H
#define RESULTLITWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "litimgitemwidget.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QImage>

class ResultLitWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ResultLitWidget(QWidget *parent = nullptr);
    void updateLitimages();

protected:
    bool InitWidget();

signals:
    void sendImg(const QImage& img);  // 推荐使用const引用
    void load_result_img();

public slots:
    void OncurrentItemChanged(int currentrow);
    void on_changeResultList(QVector<QImage> m_DstImages);
    void OnitemClicked(QListWidgetItem *item);
    void handleResult(QVector<QImage> result_imgs);

private:
    // void updateLitimages();
    void resizeEvent(QResizeEvent*) override;

public:
    // 每次运行后设置窗口的图片
    QImage last_result_img;
    // 该算法运行的所有结果
    QVector<QImage> m_dstImages;
    QListWidget* m_pImgListWidget;
    QStringList m_imagePaths;//存储缩略图读取的图像的绝对路径
    QVBoxLayout *mainLayout; // 添加主布局
    int current_row;
};

#endif // RESULTLITWIDGET_H

