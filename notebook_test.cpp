#include <QTest>
#include "notebook_app.hpp"
#include "input_widget.hpp"
#include "output_widget.hpp"
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <iostream>

class NotebookTest : public QObject {
	Q_OBJECT

		private slots:

	void initTestCase();
	void testDiscretePlotLayout();
	void start_test();
	void stop_test();
	void restart_test();
	void interrupt_test();
	void NONE_test();
	void cospi();
	void algebra1();
	void stringtype();
	void algebra2();
	void point1();
	void point2();
	void point3();
	void point4();
	void line1();
	void line2();
	void line3();
	void line4();
	void line5();
	void text1();
	void error1();
	void error2();
	
	//void testContinuousPlotLayout();
	//void testMakeText();

	// TODO: implement additional tests here
private:

	NotebookApp notebook;

};

void NotebookTest::initTestCase() {

}

/*
findLines - find lines in a scene contained within a bounding box
with a small margin
*/
int findLines(QGraphicsScene * scene, QRectF bbox, qreal margin) {

	QPainterPath selectPath;

	QMarginsF margins(margin, margin, margin, margin);
	selectPath.addRect(bbox.marginsAdded(margins));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

	int numlines(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsLineItem::Type) {
			numlines += 1;
		}
	}

	return numlines;
}

/*
findPoints - find points in a scene contained within a specified rectangle
*/
int findPoints(QGraphicsScene * scene, QPointF center, qreal radius) {

	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

	int numpoints(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsEllipseItem::Type) {
			numpoints += 1;
		}
	}

	return numpoints;
}

/*
findText - find text in a scene centered at a specified point with a given
rotation and string contents
*/
int findText(QGraphicsScene * scene, QPointF center, qreal rotation, QString contents) {

	int numtext(0);
	foreach(auto item, scene->items(center)) {
		if (item->type() == QGraphicsTextItem::Type) {
			QGraphicsTextItem * text = static_cast<QGraphicsTextItem *>(item);
			if ((text->toPlainText() == contents) &&
				(text->rotation() == rotation) &&
				(text->pos() + text->boundingRect().center() == center)) {
				numtext += 1;
			}
		}
	}

	return numtext;
}

/*
intersectsLine - find lines in a scene that intersect a specified rectangle
*/
int intersectsLine(QGraphicsScene * scene, QPointF center, qreal radius) {

	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
	scene->setSelectionArea(selectPath, Qt::IntersectsItemShape);

	int numlines(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsLineItem::Type) {
			numlines += 1;
		}
	}

	return numlines;
}


void NotebookTest::testDiscretePlotLayout() {

	std::string program = R"( 
(discrete-plot (list (list -1 -1) (list 1 1)) 
    (list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label") ))
)";

	auto inputWidget = notebook.findChild<InputWidget *>("input");
	auto outputWidget = notebook.findChild<OutputWidget *>("output");
	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);

	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	QCOMPARE(items.size(), 17);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 2.0;
	double scaley = 20.0 / 2.0;

	double xmin = scalex * -1;
	double xmax = scalex * 1;
	double ymin = scaley * -1;
	double ymax = scaley * 1;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("The Title")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("X Label")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("Y Label")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-1")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("1")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-1")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("1")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

	// check the abscissa axis
	QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

	// check the ordinate axis 
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

	// check the point at (-1,-1)
	QCOMPARE(findPoints(scene, QPointF(-10, 10), 0.6), 1);

	// check the point at (1,1)
	QCOMPARE(findPoints(scene, QPointF(10, -10), 0.6), 1);
}
void NotebookTest::start_test() {

	std::string program = R"(
(begin
    (define f (lambda (x) 
        (list x (+ (* 2 x) 1))))
    (discrete-plot (map f (range -2 2 0.5))
       (list
       (list "title" "The Data")
       (list "abscissa-label" "X Label")
       (list "ordinate-label" "Y Label")
       (list "text-scale" 1))))
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 31);


	auto start = notebook.findChild<QPushButton *>("start");
	QVERIFY2(start, "Could not find QGraphicsView as child of QPushButton");
	

	

	start->click();

	items = scene->items();
	QCOMPARE(items.size(), 1);

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 31);

	start->click();

	items = scene->items();
	QCOMPARE(items.size(), 1);

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 31);



	/*foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}
	QCOMPARE(findPoints(scene, QPointF(1, 1), 10), 1);*/

}

void NotebookTest::stop_test() {

	std::string program = R"(
(begin
    (define f (lambda (x) 
        (list x (+ (* 2 x) 1))))
    (discrete-plot (map f (range -2 2 0.5))
       (list
       (list "title" "The Data")
       (list "abscissa-label" "X Label")
       (list "ordinate-label" "Y Label")
       (list "text-scale" 1))))
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 31);


	auto start = notebook.findChild<QPushButton *>("start");
	QVERIFY2(start, "Could not find QGraphicsView as child of QPushButton");
	auto stop = notebook.findChild<QPushButton *>("stop");
	QVERIFY2(stop, "Could not find QGraphicsView as child of QPushButton");


	stop->click();

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 1);

	start->click();

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 31);

	start->click();

	items = scene->items();
	QCOMPARE(items.size(), 1);


	/*foreach(auto item, items) {
	item->setFlag(QGraphicsItem::ItemIsSelectable);
	}
	QCOMPARE(findPoints(scene, QPointF(1, 1), 10), 1);*/

}

void NotebookTest::restart_test() {

	std::string program = R"(
(begin
    (define f (lambda (x) 
        (list x (+ (* 2 x) 1))))
    (discrete-plot (map f (range -2 2 0.5))
       (list
       (list "title" "The Data")
       (list "abscissa-label" "X Label")
       (list "ordinate-label" "Y Label")
       (list "text-scale" 1))))
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 31);


	auto start = notebook.findChild<QPushButton *>("start");
	QVERIFY2(start, "Could not find QGraphicsView as child of QPushButton");
	auto stop = notebook.findChild<QPushButton *>("stop");
	QVERIFY2(stop, "Could not find QGraphicsView as child of QPushButton");
	auto reset = notebook.findChild<QPushButton *>("reset");
	QVERIFY2(reset, "Could not find QGraphicsView as child of QPushButton");


	stop->click();

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 1);

	start->click();

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 31);

	start->click();
	items = scene->items();
	QCOMPARE(items.size(), 1);

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 31);

	reset->click();

	items = scene->items();
	QCOMPARE(items.size(), 0);

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 31);

	
	/*foreach(auto item, items) {
	item->setFlag(QGraphicsItem::ItemIsSelectable);
	}
	QCOMPARE(findPoints(scene, QPointF(1, 1), 10), 1);*/

}

void NotebookTest::interrupt_test() {

	std::string program = R"(
(begin
    (define f (lambda (x) 
        (list x (+ (* 2 x) 1))))
    (discrete-plot (map f (range -2 2 0.5))
       (list
       (list "title" "The Data")
       (list "abscissa-label" "X Label")
       (list "ordinate-label" "Y Label")
       (list "text-scale" 1))))
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 31);


	auto start = notebook.findChild<QPushButton *>("start");
	QVERIFY2(start, "Could not find QGraphicsView as child of QPushButton");
	auto stop = notebook.findChild<QPushButton *>("stop");
	QVERIFY2(stop, "Could not find QGraphicsView as child of QPushButton");
	auto reset = notebook.findChild<QPushButton *>("reset");
	QVERIFY2(reset, "Could not find QGraphicsView as child of QPushButton");
	auto interrupt = notebook.findChild<QPushButton *>("interrupt");
	QVERIFY2(interrupt, "Could not find QGraphicsView as child of QPushButton");

	stop->click();

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 1);

	start->click();

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 31);

	start->click();
	items = scene->items();
	QCOMPARE(items.size(), 1);

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 31);

	reset->click();

	items = scene->items();
	QCOMPARE(items.size(), 0);

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 31);

	interrupt->click();

	items = scene->items();
	QCOMPARE(items.size(), 1);

	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	items = scene->items();
	QCOMPARE(items.size(), 31);


	/*foreach(auto item, items) {
	item->setFlag(QGraphicsItem::ItemIsSelectable);
	}
	QCOMPARE(findPoints(scene, QPointF(1, 1), 10), 1);*/

}

void NotebookTest::NONE_test()
{
	std::string program = R"(
(get-property "key" 3)
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}

void NotebookTest::cospi()
{
	std::string program = R"(
(cos pi)
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}
void NotebookTest::algebra1()
{
	std::string program = R"(
(^ e (- (* I pi)))
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}
void NotebookTest::stringtype()
{
	std::string program = R"(

(begin
(define title "the title")
(title)
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}
void NotebookTest::algebra2()
{
	std::string program = R"(
(define inc (lambda (x) (+ x 1)))
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 0);
}

void NotebookTest::point1()
{
	std::string program = R"(
(make-point 1 1)
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}
void NotebookTest::point2()
{
	std::string program = R"(
(set-property "size" 20 (make-point 0 0))
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}

void NotebookTest::point3()
{
	std::string program = R"(
(list
(set-property "size" 1 (make-point 0 0))
(set-property "size" 2 (make-point 4 0))
(set-property "size" 4 (make-point 8 0))
(set-property "size" 8 (make-point 16 0))
(set-property "size" 16 (make-point 32 0))
(set-property "size" 32 (make-point 64 0))
)
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}

void NotebookTest::point4()
{
	std::string program = R"(
(begin
(list
(set-property "size" 1 (make-point 0 0))
(set-property "size" 2 (make-point 0 4))
(set-property "size" 4 (make-point 0 8))
(set-property "size" 8 (make-point 0 16))
(set-property "size" 16 (make-point 0 32))
(set-property "size" 32 (make-point 0 64))
)
)
)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}
void NotebookTest::line1()
{
	std::string program = R"(
(make-line
(make-point 0 0)
(make-point 1 1)

)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}
void NotebookTest::line2()
{
	std::string program = R"(
(make-line
(make-point 0 0)
(make-point 1 1)

)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}

void NotebookTest::line3()
{
	std::string program = R"(
(set-property "thickness" (4)
(make-line
(make-point 0 0)
(make-point 1 1)
)

)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}


void NotebookTest::line4()
{
	std::string program = R"(
(list
(make-line
(make-point 0 0)
(make-point 1 1)
)
(make-line
(make-point 0 0)
(make-point 1 1)
)
(make-line
(make-point 0 0)
(make-point 1 1)
)
(make-line
(make-point 0 0)
(make-point 1 1)
)

)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}

void NotebookTest::line5()
{
	std::string program = R"(
(list
(make-line
(make-point 0 0)
(make-point 20 0)
)
(make-line
(make-point 0 10)
(make-point 20 10)
)
(make-line
(make-point 0 20)
(make-point 20 20)
)
(make-line
(make-point 0 40)
(make-point 20 40)
)

)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}
void NotebookTest::text1()
{
	std::string program = R"(
(make-text "Hello World")

)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}

void NotebookTest::error1()
{
	std::string program = R"(
(begin))

)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}
void NotebookTest::error2()
{
	std::string program = R"(
(begin
(define a I)
(first a)
)	

)";

	auto input = notebook.findChild<InputWidget *>("input");
	auto output = notebook.findChild<OutputWidget *>("output");
	input->setPlainText(QString::fromStdString(program));
	QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

	auto view = output->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	std::cout << "scene here: " << items.size() << std::endl;
	QCOMPARE(items.size(), 1);
}

QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
