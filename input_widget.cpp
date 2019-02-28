#include "input_widget.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

InputWidget::InputWidget(QWidget * parent) : QPlainTextEdit(parent)
{
	//input = new QPlainTextEdit();
	InputWidget::setObjectName("input");
	//input->setFocus();
}

void InputWidget::keyPressEvent(QKeyEvent * event)
{

	//if (event->key() == Qt::Key_Enter && event->modifiers() == Qt::ShiftModifier)
	//if(event->key() == (Qt::Key_Enter | Qt::Key_Shift))
	if (event->key() == Qt::Key_Return
		&& (event->modifiers() == Qt::ShiftModifier))
	{
		clearScreenBullshit("input");
		//QString str = *this;
		//auto var = this->toPlainText().toUtf8().constData();
		emit sendDataFlag(this->toPlainText());

	}
	else
	QPlainTextEdit::keyPressEvent(event);
}

bool clearScreenBullshit(std::string yeet)
{
	static bool yote = false;
	if (yeet == "input")
		yote = true;
	else
		yote = false;

	return yote;
}
