#pragma once
#include <QPushButton>
#include <QPixmap>

class CutomButton : public QPushButton
{
	Q_OBJECT
public:
	CutomButton(QWidget *parent);
	~CutomButton();

	void setBtnPicture(QPixmap& pic);
	void setPressPicture(QPixmap& pic);
	void setReleasePicture(QPixmap& pic);
	void setEnterPicture(QPixmap& pic);
	void setLeavePicture(QPixmap& pic);
	void setSize(int x, int y, int width, int height);
	void resizeButton(int w, int h);

protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void enterEvent(QEvent* event);
	virtual void leaveEvent(QEvent *event);
	virtual void resizeEvent(QResizeEvent *event);

private:
	QPixmap*   btnPicture;
	QPixmap*   pressPicture;
	QPixmap*   releasePicture;
	QPixmap*   enterPicture;
	QPixmap*   leavePicture;

};
