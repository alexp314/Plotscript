#ifndef input_widget_OBJECT_H
#define input_widget_OBJECT_H
#include <QPlainTextEdit>
#include <QDebug>

#include <QObject>


bool clearScreenBullshit(std::string yeet);


class InputWidget : public QPlainTextEdit {
	Q_OBJECT

public:

	InputWidget(QWidget * parent = nullptr);

	QPlainTextEdit * input;

	

	void keyPressEvent(QKeyEvent * event);
	


signals:
	void sendDataFlag(QString inp);

};

#endif
