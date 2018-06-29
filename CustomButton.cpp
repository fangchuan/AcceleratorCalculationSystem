#include "custombutton.h"

CutomButton::CutomButton(QWidget *parent) : QPushButton(parent)
{
    //保存图片成员初始化
    btnPicture = new QPixmap();
    pressPicture = new QPixmap();
    releasePicture = new QPixmap();

    enterPicture = new QPixmap();
    leavePicture = new QPixmap();

    //关闭按钮的默认显示
    this->setFlat(true);
    this->setFocusPolicy(Qt::NoFocus);

}

CutomButton::~CutomButton()
{

}

void CutomButton::setBtnPicture(QPixmap& pic)
{
    *btnPicture = pic;
    this -> setIcon(QIcon(*btnPicture));
}

void CutomButton::setPressPicture(QPixmap& pic)
{
    *pressPicture = pic;
}

void CutomButton::setReleasePicture(QPixmap& pic)
{
    *releasePicture = pic;
}

void CutomButton::setEnterPicture(QPixmap& pic)
{
    *enterPicture = pic;
}

void CutomButton::setLeavePicture(QPixmap& pic)
{
    *leavePicture = pic;
}

void CutomButton::setSize(int x, int y, int width, int height)
{
    this -> setIconSize(QSize(width, height));
    this -> setGeometry(x, y, width, height);
}

void CutomButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPushButton::mouseDoubleClickEvent(event);
}

void CutomButton::mousePressEvent(QMouseEvent *event)
{
    this -> setIcon (QIcon(*pressPicture));
    QPushButton::mousePressEvent(event);
}

void CutomButton::mouseMoveEvent(QMouseEvent *event)
{
    QPushButton::mouseMoveEvent(event);
}


void CutomButton::mouseReleaseEvent(QMouseEvent *event)
{
    this -> setIcon(QIcon(*releasePicture));
    emit clicked();

    QPushButton::mouseReleaseEvent(event);
}

void CutomButton::enterEvent(QEvent *event)
{
    this->setIcon(QIcon(*enterPicture));

    QPushButton::enterEvent(event);

}

void CutomButton::leaveEvent(QEvent *event)
{
    this->setIcon(QIcon(*leavePicture));

    QPushButton::leaveEvent(event);
}

void CutomButton::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);
}

void CutomButton::resizeButton(int w, int h)
{
    this->raise();
    this->resize(w,h);
    this->setIconSize(QSize(w, h));
}