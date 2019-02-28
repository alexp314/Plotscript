#include "notebook_app.hpp"
#include "expression.hpp"
#include "output_widget.hpp"
//#include "interpreter.hpp"
//#include "semantic_error.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <complex>
#include <QRegularExpression>
#include <thread>
#include "message_queue.hpp"

//void NotebookApp::thread_eval(Interpreter *interp, InputQueue *inq, OututQueue *outq, outputq *outType);

void error(const std::string & err_str) {
	std::cerr << "Error: " << err_str << std::endl;
}

NotebookApp::NotebookApp()
{
	//outputText = new QGraphicsScene;
	//QWidget * parent = nullptr;
	input = new InputWidget();
	output = new OutputWidget();
	layout = new QVBoxLayout();
	//auto button = new QPushButton();
	auto buttonLayout = new QHBoxLayout();
	button1 = new QPushButton();
	button2 = new QPushButton();
	button3 = new QPushButton();
	button4 = new QPushButton();

	eval_from_file_nApp(STARTUP_FILE);
	button1->setText("Start Kernel");
	button1->adjustSize();
	button1->animateClick();
	button2->setText("Stop Kernel");
	button2->adjustSize();
	button2->animateClick();
	button3->setText("Reset Kernel");
	button3->adjustSize();
	button3->animateClick();
	button4->setText("Interrupt Kernel");
	button4->adjustSize();
	button4->animateClick();
	button1->setObjectName("start");
	button2->setObjectName("stop");
	button3->setObjectName("reset");
	button4->setObjectName("interrupt");

	

	buttonLayout->addWidget(button1);
	buttonLayout->addWidget(button2);
	buttonLayout->addWidget(button3);
	buttonLayout->addWidget(button4);
	layout->addLayout(buttonLayout);
	//input->setFocus();
	layout->addWidget(input);
	layout->addWidget(output);
	//layout->addLayout(buttonLayout);
	setLayout(layout);
	//QWidget::setFocusPolicy(Qt::focus)
	//mythread = std::thread(thread_eval, &interp, &inq, &outq, &outType);

	//QWidget	* window = new QWidget;
	
	mythread = std::thread(&NotebookApp::thread_eval, std::ref(thread_interpreter), std::ref(inq), std::ref(outq));
	
	QObject::connect(input, SIGNAL(sendDataFlag(QString)), this, SLOT(getInput(QString)));
	QObject::connect(this, SIGNAL(sendStringToOutput(QString, qreal, qreal, qreal, qreal)), output, SLOT(getOutputText(QString, qreal, qreal, qreal, qreal)));
	QObject::connect(this, SIGNAL(sendEllipseToOutput(qreal, qreal, qreal, qreal)), output, SLOT(getOutputEllipse(qreal, qreal, qreal, qreal)));
	QObject::connect(this, SIGNAL(sendLineToOutput(qreal, qreal, qreal, qreal, int)), output, SLOT(getOutputLine(qreal, qreal, qreal, qreal, int)));
	QObject::connect(this, SIGNAL(clearOutput(QString)), output, SLOT(clear(QString)));
	QObject::connect(this, SIGNAL(sendString(QString, qreal, qreal)), output, SLOT(getOutputString(QString, qreal, qreal)));

	//handle buttons here
	connect(button1, SIGNAL(pressed()), this, SLOT(handleButton1()));
	connect(button2, SIGNAL(pressed()), this, SLOT(handleButton2()));
	connect(button3, SIGNAL(pressed()), this, SLOT(handleButton3()));
	connect(button4, SIGNAL(pressed()), this, SLOT(handleButton4()));
	//setCentralWidget(window);
	//inq = new InputQueue;
	//outq = new OututQueue;
	//outType = new outputq;
	//fucniton called spawna thread 
	//mythread = new std::thread(thread_eval, &interp, &inq, &outq, &outType);
	//make_thread(thread_interpreter);


}
void NotebookApp::getInput(QString in) {
	//qDebug() << input->toPlainText();

	//output->clear();
	//outputText->addText(input->toPlainText());
	//output->show();
	//eval_from_file(STARTUP_FILE);
	emit clearOutput("yote");

	notebook_eval(in);

	input->clear();
	//sceneText->clear();

}
void NotebookApp::eval_from_file_nApp(std::string filename)
{
	std::ifstream ifs(filename);
	//Interpreter interp;
	qreal qx = 0;
	qreal qy = 0;
	//qreal r1 = 0;
	if (!ifs) {
		//error("Could not open file for reading.");
		QString str = "Error: 53";
		emit sendString(str, qx, qy);
		//return EXIT_FAILURE;
	}

	//repl();
	eval_from_stream_nApp(ifs, filename);
}

void NotebookApp::eval_from_stream_nApp(std::istream & stream, std::string filename)
{
	filename = "ooga booga";
	qreal qx = 0;
	qreal qy = 0;
	//qreal r1 = 0;
	if (!thread_interpreter.parseStream(stream)) {
		//error("Invalid Program. Could not parse.");
		//return EXIT_FAILURE;
		QString str = "Error: 67";
		emit sendString(str, qx, qy);
	}
	else {
		try {
			Expression exp = thread_interpreter.evaluate();
			//std::cout << exp << std::endl;
		}
		catch (const SemanticError & ex) {
			QString str = ex.what();
			emit sendString(str, qx, qy);
		}
	}
}

void NotebookApp::handleStrings(Expression exp)
{
	const double PI = std::atan2(0, -1);
	//std::ostringstream oss;
	//oss << exp << std::endl;
	//qDebug() << oss.str().c_str();
	Expression position = exp.getProperty("\"position\"");
	Expression size = exp.getProperty("\"size\"");
	Expression scale = exp.getProperty("\"text-scale\"");
	Expression rotation = exp.getProperty("\"text-rotation\"");
	QString str = QString::fromStdString(exp.head().asSymbol());
		//QString::fromStdString(oss.str().c_str());


	Expression p1 = *(position.tailConstBegin());
	Expression p2 = *(position.tail());
	qreal po1 = p1.head().asNumber();
	qreal po2 = p2.head().asNumber();

	qreal s = scale.head().asNumber();

	qreal rotate = rotation.head().asNumber() * 180/PI;

	QRegularExpression regex;
	regex.setPattern("\"");
	str.remove(regex);
	//qDebug() << "string: " << str << " po1: " << po1 << " po2: " << po2 << " rotation: " << rotate << " scale: " << s;
	emit sendStringToOutput(str, po1, po2, rotate, s);
}

void NotebookApp::handlePoint(Expression exp)
{
	Expression p1 = *(exp.tailConstBegin());
	Expression p2 = *(exp.tail());

	qreal po1 = p1.head().asNumber();
	qreal po2 = p2.head().asNumber();


	Expression size = exp.getProperty("\"size\"");

	qreal sz = size.head().asNumber();
	//qDebug() << po1 << "po1 ";
	//qDebug() << po2 << "po2 ";
	//qDebug() << sz << "sz \n";
	emit sendEllipseToOutput(po1, po2, sz, sz);
}
void NotebookApp::handleLine(Expression exp)
{
	Expression p12t = *(exp.tailConstBegin());
	Expression p22t = *(exp.tail());
	Expression p1 = *(p12t.tailConstBegin());
	Expression p2 = *(p12t.tail());
	Expression p3 = *(p22t.tailConstBegin());
	Expression p4 = *(p22t.tail());
	qreal po1 = p1.head().asNumber();
	qreal po2 = p2.head().asNumber();
	qreal po3 = p3.head().asNumber();
	qreal po4 = p4.head().asNumber();


	Expression thickness = exp.getProperty("\"thickness\"");

	int thicc = thickness.head().asNumber();
	//qDebug() << po1 << " ";
	//qDebug() << po2 << " ";
	//qDebug() << po3 << " ";
	//qDebug() << po4 << " ";
	//qDebug() << thicc << " ";
	emit sendLineToOutput(po1, po2, po3, po4, thicc);
}

void NotebookApp::evalAll(Expression exp)
{
	qreal qx = 0;
	qreal qy = 0;
	//qreal r1 = 0;
	Expression propertyName = exp.getProperty("\"object-name\"");
	if (exp.head().isNone())
	{
		QString str = "NONE";
		QRegularExpression regex;
		regex.setPattern("\n");
		str.remove(regex);
		//qDebug() << str;
		emit sendString(str, qx, qy);
	}
	
	else if ((exp.head().asSymbol() == "discrete-plot") || (exp.head().isSymbol() && exp.head().asSymbol() == "list" && 
		(propertyName.head().asSymbol() != "\"point\"" && propertyName.head().asSymbol() != "\"line\"" &&
			propertyName.head().asSymbol() != "\"text\"")))
	{
		for (auto it = exp.tailConstBegin(); it != exp.tailConstEnd(); ++it)
		{
			Expression littyTitties = *it;
			evalAll(littyTitties);
		}
		//for (const_iterator it = exp.tailConstBegin())
		//{

		//}
		//Expression fuckME = *exp.tailConstBegin();
		//Expression fuckMEMORE = *exp.tailConstBegin()++;


	}
	else if (exp.head().asSymbol() != "lambda")
	{
		//Expression exp = interp.evaluate();
		//std::cout << exp << std::endl;
		std::ostringstream oss;
		oss << exp << std::endl;
		//qDebug() << oss.str().c_str();

		
		if (propertyName.head().asSymbol() == "\"point\"")
		{
			handlePoint(exp);
		}
		else if (propertyName.head().asSymbol() == "\"line\"")
		{
			handleLine(exp);
		}
		else if (propertyName.head().asSymbol() == "\"text\"")
		{

			handleStrings(exp);
		}
		else
		{
			
			QString str = QString::fromStdString(oss.str().c_str());
			QRegularExpression regex;
			regex.setPattern("\n");
			str.remove(regex);
			//qDebug() << str << "string";
			emit sendString(str, qx, qy);
		}

		//}



	}
	else
	{
		//QString str = "";
		emit clearOutput("yote");
		//emit sendString(str, qx, qy);
	}
}

void NotebookApp::notebook_eval(QString input)
{
	qreal qx = 0;
	qreal qy = 0;
	//qreal r1 = 0;
	QString str;
	outputq outType;
	//Interpreter interp;
	static bool thread_started = true;
	Expression exp;
	std::string out;

	//std::string out;
	//mythread = std::thread(thread_eval, &interp, &inq, &outq, &outType);
	std::string in = input.toUtf8().constData();
	//std::istringstream expression(in);
	//
	//if (!interp.parseStream(expression)) {
	//	//error("Invalid Expression. Could not parse.");
	//	QString str = "Error: invalid expression. Could not parse";
	//	emit sendString(str, qx, qy);
	//}
	//else {
	//	try {
	//		Expression exp = interp.evaluate();
	//		//std::cout << exp << std::endl;
	//		//std::string newString = this->ast

	//		evalAll(exp);
	//	}
	//	catch (const SemanticError & ex) {
	//		//qDebug() << "error" << endl;
	//		//std::cerr << ex.what() << std::endl;
	//		//emit sendToOutput(error(ex.what()));
	//		QString str = ex.what();
	//		emit sendString(str, qx, qy);
	//	}
	//}

	if (in == "start" && !thread_started)
	{
		//mythread = new std::thread(thread_eval, &interp, &inq, &outq, &outType);
		mythread = std::thread(&NotebookApp::thread_eval, std::ref(thread_interpreter), std::ref(inq), std::ref(outq));
		thread_started = true;
	}
	else if (in == "stop" && thread_started)
	{
		std::string stop_thread = "stop";
		inq.push(stop_thread);
		//outq.wait_and_pop(outType);
		thread_started = false;
		if (mythread.joinable())
		{
			mythread.join();
		}
		//mythread.~thread();
	}
	else if (thread_started && in == "reset")
	{
		std::string reset_thread = "reset";
		inq.push(reset_thread);
		thread_started = false;
		if (mythread.joinable())
		{
			mythread.join();
		}
		//Interpreter reset_interp = copy_interp;
		//interp = reset_interp;
		Interpreter interp2;
		thread_interpreter = interp2;
		//make_thread(thread_interpreter);
		mythread = std::thread(&NotebookApp::thread_eval, std::ref(thread_interpreter), std::ref(inq), std::ref(outq));
		//*mythread = std::thread(thread_eval, &interp, &inq, &outq, &outType);
		thread_started = true;
	}
	else if (!thread_started)
	{
		QString str = "Error: thread not running";
		emit sendString(str, qx, qy);
	}
	else if (in == "start" && thread_started)
	{
		QString str = "Error: thread already running";
		emit sendString(str, qx, qy);
	}
	else
	{
		inq.push(in);
		outq.wait_and_pop(outType);

		if (outType.type == 0)
		{
				str = QString::fromStdString(outType.str);
				emit sendString(str, qx, qy);
			
		}
		else if (outType.type == 1)
		{
			evalAll(outType.exp);
			//		evalAll(exp);

		}
	}
	//mythread.~thread();
	//emit sendString(str, qx, qy);
}

void NotebookApp::thread_eval(Interpreter & thread_interpreter, InputQueue & inq, OututQueue & outq)
{
	std::string in;
	outputq outType;
	while (1)
	{

		inq.wait_and_pop(in);
		std::istringstream expression(in);
		//bool passes = false;
		if (in == "stop")
		{
			break;
		}
		else if (in == "reset")
		{
			//Environment * env = new Environment;
			//env.reset();
			break;
		}
		else if (in == "exit")
		{
			break;
		}
		else
		{
			if (!thread_interpreter.parseStream(expression)) {
				//error("Invalid Expression. Could not parse.");
				//QString str = "Error: invalid expression. Could not parse";
				std::string out = "Error: invalid expression. could not parse";
				outType.str = out;
				outType.type = 0;
				outq.push(outType);
			}
			else {
				try {
					Expression exp = thread_interpreter.evaluate();
					//std::cout << exp << std::endl;
					//std::string newString = this->ast
					outType.exp = exp;
					outType.type = 1;
					outq.push(outType);

				}
				catch (const SemanticError & ex) {
					//qDebug() << "error" << endl;
					//std::cerr << ex.what() << std::endl;
					//emit sendToOutput(error(ex.what()));
					//QString str = ex.what();
					//QString str = ex.what();
					std::string out = ex.what();
					outType.str = out;
					outType.type = 0;
					outq.push(outType);
				}
			}
		}
	}
	//mythread.~thread();
	//mythread.~thread();
		//eval_from_file_napp(startup_file);

		//if (in.empty()) continue;
		//std::ifstream ifs(startup_file);
		/*Expression exp1 = interp->evaluate();
		if (expression.str().empty() || expression.str() == "\n")
		{
			std::string out = "yote";
			outType->str = out;
			outType->type = 0;
			outq->push(*outType);

		}



		if (!interp->parseStream(expression)) {
			//error("invalid expression. could not parse.");
			std::string out = "error: invalid expression. could not parse";
			outType->str = out;
			outType->type = 0;
			outq->push(*outType);
			//error("error: invalid expression. could not parse");
			//app.exec();

		}

		else {
			try {
				Expression exp = interp->evaluate();
				//std::cout << exp << std::endl;
				//std::string newstring = this->ast
				outType->exp = exp;
				outType->type = 1;
				outq->push(*outType);


			}
			catch (const SemanticError & ex) {
				//qdebug() << "error" << endl;
				//std::cerr << ex.what() << std::endl;
				//emit sendtooutput(error(ex.what()));
				QString str = ex.what();
				std::string out = ex.what();
				outType->str = out;
				outType->type = 0;
				outq->push(*outType);
				//std::cerr << ex.what() << std::endl;
			}
		}*/
//	}
}


void NotebookApp::handleButton1()
{
	//std::cout<< "pressed" << std::endl;
	//QString str = "start";
	QString str = "start";
	emit clearOutput("yote");
	notebook_eval(str);
}

void NotebookApp::handleButton2()
{
	//std::cout<< "pressed stop" << std::endl;
	QString str = "stop";
	emit clearOutput("yote");
	notebook_eval(str);
}

void NotebookApp::handleButton3()
{
	//std::cout<< "pressed reset" << std::endl;
	QString str = "reset";
	emit clearOutput("yote");
	notebook_eval(str);
}


void NotebookApp::handleButton4()
{
	//std::cout<< "pressed" << std::endl;
	QString str = "interrupt";
	emit clearOutput("yote");
	notebook_eval(str);
}
