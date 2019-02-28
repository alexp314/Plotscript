#include "output_widget.hpp"
#include "input_widget.hpp"
#include <QVBoxLayout>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QDebug>
#include <QPen>
//#include "environment.cpp"


OutputWidget::OutputWidget() {
	sceneText = new QGraphicsScene();
	//outputText = new QGraphicsTextItem;
	//ellipse = new QGraphicsEllipseItem;
	viewText = new QGraphicsView(sceneText);
	OutputWidget::setObjectName("output");
	auto outputLayout = new QVBoxLayout;
	outputLayout->addWidget(viewText);
	viewText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	viewText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setLayout(outputLayout);
}

void OutputWidget::resizeEvent(QResizeEvent * event)
{
	QWidget::resizeEvent(event);
	viewText->fitInView(sceneText->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::clear(QString yote)
{
	if (yote == "yote")
	{
		sceneText->clear();
	}
}
void OutputWidget::getOutputString(QString text, qreal x, qreal y)
{

	QGraphicsTextItem *outputText = new QGraphicsTextItem;
	QPen pen;
	pen.setWidth(1);
	pen.setColor(Qt::black);

	//qreal x2, y1, y2;
	outputText->setPlainText(text);

	QFont font("Courier");
	outputText->setX(x);
	outputText->setY(y);

	outputText->setFont(font);
	sceneText->addItem(outputText);
	viewText->fitInView(sceneText->itemsBoundingRect(), Qt::KeepAspectRatio);


}
void OutputWidget::getOutputText(QString text, qreal x, qreal y, qreal r, qreal s)
{

	auto font = QFont("Monospace");
	font.setStyleHint(QFont::TypeWriter);
	font.setPointSize(1);

	QGraphicsTextItem *outputText = new QGraphicsTextItem;
	outputText->setFont(font);
	outputText->setPlainText(text);

	//QPen pen(Qt::black);
	//pen.setWidth(1);
	////pen.setColor(Qt::black);
	////line->setPen(pen);
	//sceneText->addLine(-10, 0, 10, 0, pen);
	//sceneText->addLine(0, -10, 0, 10, pen);


	QPointF center = outputText->boundingRect().center();
	outputText->setPos(x - center.x(), y - center.y());
	outputText->setScale(s);
	outputText->setTransformOriginPoint(center);
	outputText->setRotation(r);
	sceneText->addItem(outputText);

	viewText->fitInView(sceneText->itemsBoundingRect(), Qt::KeepAspectRatio);
}
void OutputWidget::getOutputEllipse(qreal x, qreal y, qreal w, qreal h)
{
	QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem();
	QPen * pen = new QPen;
	pen->setWidth(0);


	//x = ;
	//y = ;
	//qDebug() << w << " " << h;
	x = x - (w / 2.);
	y = y - (h / 2.);
	QRectF rect(x, y, w, w);
	//qDebug() << x << " " << y << " " << w << " here ye here ye";
	//QPen * pen = new QPen;

	//ellipse->setScale(w);
	ellipse->setPen(*pen);
	ellipse->setBrush((Qt::black));
	ellipse->setRect(rect);
	//sceneText->addEllipse(rect, QPen(), QBrush(Qt::black));
	sceneText->addItem(ellipse);
	//viewText->setScene(sceneText);

	viewText->fitInView(sceneText->itemsBoundingRect(), Qt::KeepAspectRatio);


}

void OutputWidget::getOutputLine(qreal po1, qreal po2, qreal po3, qreal po4, int thicc)
{
	QGraphicsLineItem *line = new QGraphicsLineItem(po1, po2, po3, po4);
	QPen pen(Qt::black);
	pen.setWidth(thicc);
	//pen.setColor(Qt::black);
	line->setPen(pen);
	//sceneText->addLine(po1, po2, po3, po4, pen);
	//sceneText->addLine(-10, 0, 10, 0, pen);
	//sceneText->addLine(0, -10, 0, 10, pen);
	//pen.setWidth(1);
	//viewText->setScene(sceneText);
	sceneText->addItem(line);
	viewText->fitInView(sceneText->itemsBoundingRect(), Qt::KeepAspectRatio);

}