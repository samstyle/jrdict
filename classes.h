#ifndef __mwindef
#define __mwindef

#include <QTextEdit>

#include "vars.h"

#include "ui_mainwindow.h"
#include "ui_wordwin.h"

class MainWin : public QMainWindow {
	Q_OBJECT
	public:
		MainWin(QWidget*);
	public slots:
		void gotranslate();
		void tranCbrd();
		void waneword();
		void reload();
		void wanoldword(QModelIndex);
		void reselect(int);
		void delword();
	signals:
		void wannaeditword(dWord);
		void wannaform();
	private:
		Ui::MWindow ui;
		QClipboard *cbrd;
};

class WordWin : public QDialog {
	Q_OBJECT
	public:
		WordWin(QWidget*, dNode*);
	public slots:
		void goeditword(dWord);
		void searchword(QString);
		void saveword();
	signals:
		void wantranslate();
	private slots:
		void recallword(QModelIndex);
	private:
		Ui::WordForm ui;
		dNode *dicton;
		dWord eword;
};

class FormWin : public QDialog {
	Q_OBJECT
	public:
		FormWin(QWidget*, QString);
	public slots:
		void goforms();
		void saveforms();
	signals:
		void wantranslate();
	private:
		QTextEdit *editor;
		QPushButton *okbut,*canbut;
		QString formpath;
};

#endif
