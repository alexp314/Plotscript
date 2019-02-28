#ifndef NOTEBOOK_APP_HPP
#define NOTEBOOK_APP_HPP
#include "input_widget.hpp"
#include "output_widget.hpp"
#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "startup_config.hpp"
#include "expression.hpp"
#include "environment.hpp"
#include "message_queue.hpp"
#include <thread>

#include <QPushButton>
//#include <QMainWindow>
#include <QWidget>
#include <QLayout>
#include <QObject>

//#include <QtGui> 
//#include <QtCore>


class NotebookApp : public QWidget {
	Q_OBJECT

public:
	NotebookApp();

	//QMainWindow * mainWindow;
	QVBoxLayout * layout;

	InputWidget * input;
	OutputWidget * output;

	QPushButton * button1;
	QPushButton * button2;
	QPushButton * button3;
	QPushButton * button4;
	//std::thread thread;

	std::thread  mythread;

	typedef struct
	{
		std::string str;
		Expression exp;
		bool type;
		//SemanticError ex;

		//void getType()

	}outputq;

	typedef MessageQueue<std::string> InputQueue;
	typedef MessageQueue<outputq> OututQueue;
	InputQueue inq;
	OututQueue outq;
	//outputq outType;
	QGraphicsScene * outputText;
	//void resized()
	Interpreter interp;
	Interpreter thread_interpreter;
	void notebook_eval(QString input);
	void eval_from_file_nApp(std::string filename);
	void eval_from_stream_nApp(std::istream & stream, std::string filename);
	void handleStrings(Expression exp);
	void evalAll(Expression exp);

	void handlePoint(Expression exp);
	void handleLine(Expression exp);
	/*std::thread make_thread(Interpreter &inter) {
		return std::thread(&NotebookApp::thread_eval, std::ref(inter), std::ref(inq), std::ref(outq));
	}*/

	static void thread_eval(Interpreter & interp, InputQueue & inq, OututQueue & outq);
//	void thread_eval(Interpreter *interp, InputQueue *inq, OututQueue *outq, outputq *outType);

	~NotebookApp()
	{
		std::string input = "stop";
		inq.push(input);
		mythread.join();
		//notebook_eval(input);
	}


signals:
	void sendStringToOutput(QString text, qreal x, qreal y, qreal rotate, qreal s);
	void sendEllipseToOutput(qreal x, qreal y, qreal w, qreal h);
	void sendLineToOutput(qreal po1, qreal po2, qreal po3, qreal po4, int thicc);
	void sendString(QString, qreal, qreal);
	void clearOutput(QString yeeted);
	//void sendPointToOutput()

	public slots:
	void getInput(QString in);
	void handleButton1();
	void handleButton2();
	void handleButton3();
	void handleButton4();
};
#endif 