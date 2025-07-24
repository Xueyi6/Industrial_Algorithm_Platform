#include "CustomImageView.h"
#include <QLayout>
#include "CustomGraphicsView.h"

CustomImageView::CustomImageView(QWidget *parent)
    : QWidget(parent)
    , m_pGraphicsview(Q_NULLPTR)
{
    if(false == InitWidget())
    {
        throw std::bad_alloc();
    }
}

CustomImageView::~CustomImageView()
{
}

bool CustomImageView::InitWidget()
{
    //初始化变量
    m_pGraphicsview = new CustomGraphicsView(this);

    //初始化布局
    QHBoxLayout* pViewLayout = new QHBoxLayout();
    pViewLayout->setSpacing(0);
    pViewLayout->setContentsMargins(0,0,0,0);
    pViewLayout->addWidget(m_pGraphicsview);
    this->setLayout(pViewLayout);

    return true;
}

void CustomImageView::SetImage(QImage &qImage)
{
    m_qImage = qImage;
    m_pGraphicsview->SetImage(qImage);
}

void CustomImageView::SetImage(QString &strPath)
{
    QImage img = QImage(strPath);
    if(!img.isNull())
    {
        m_qImage = img;
        m_pGraphicsview->SetImage(img);
    }
}

void CustomImageView::OnSendImage(QImage & qImage)
{
    SetImage(qImage);
}
