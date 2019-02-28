#ifndef output_widget_OBJECT_H
#define output_widget_OBJECT_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
//#include <QObject>



class OutputWidget : public QWidget {
	Q_OBJECT

public:
	OutputWidget();
	//OutputWidget(QGraphicsScene * parent = nullptr);

	QGraphicsScene * sceneText;
	QGraphicsView * viewText;

	void resizeEvent(QResizeEvent * event);

	//QGraphicsEllipseItem * ellipse;
	//QGraphicsTextItem *outputText;
	//QString * name;


	public slots:

	void getOutputText(QString text, qreal x, qreal y, qreal r, qreal s);
	void getOutputEllipse(qreal x, qreal y, qreal w, qreal h);
	void getOutputLine(qreal po1, qreal po2, qreal po3, qreal po4, int thicc);
	void getOutputString(QString text, qreal x, qreal y);
	void clear(QString yeeted);

};
#endif