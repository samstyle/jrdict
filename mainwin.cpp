#include <QtGui>

#include "vars.h"
#include "classes.h"

QList<dictfind> findres;

MainWin::MainWin(QWidget *parent):QMainWindow(parent) {
	ui.setupUi(this);

	ui.result->weight[0] = .1;
	ui.result->weight[1] = .1;
	ui.result->weight[2] = .1;
	ui.result->weight[3] = .1;
	ui.result->weight[4] = .35;
	ui.result->weight[5] = .25;
	cbrd = QApplication::clipboard();

	QObject::connect(ui.leText,SIGNAL(textChanged(QString)),this,SLOT(gotranslate()));
	QObject::connect(ui.tbNew,SIGNAL(released()),this,SLOT(waneword()));
	QObject::connect(ui.tbReload,SIGNAL(released()),this,SLOT(reload()));
	QObject::connect(ui.tbForms,SIGNAL(released()),this,SIGNAL(wannaform()));
	QObject::connect(ui.tbDelete,SIGNAL(released()),this,SLOT(delword()));
	QObject::connect(ui.result,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(wanoldword(QModelIndex)));
	QObject::connect(ui.result,SIGNAL(indexchanged(int)),this,SLOT(reselect(int)));
	QObject::connect(cbrd,SIGNAL(dataChanged()),this,SLOT(tranCbrd()));
}

void MainWin::delword() {
	int row = ui.result->currentIndex().row();
	if (row < 0) return;
	delWord(findres[row].word);
	saveDict();
	gotranslate();
}

void MainWin::reselect(int idx) {
	ui.leText->setSelection(findres[idx].begin,findres[idx].len);
}

void MainWin::wanoldword(QModelIndex idx) {
	int row = idx.row();
	emit wannaeditword(findres[row].word);
}

void MainWin::waneword() {
	dWord newWord;
	emit wannaeditword(newWord);
}

void MainWin::reload() {
	loadDict();
	loadForms();
	gotranslate();
}

void MainWin::tranCbrd() {
	if (!ui.cbClipboard->isChecked()) return;
	ui.leText->setText(cbrd->text());
}

void MainWin::gotranslate() {
	QString string = ui.leText->text();
	QString current;
	QStringList lst;
	QList<dictfind> curr,burr;
	int maxlen = (string.size()<10)?string.size():10;
	int i,j,k,m,len,pos=0;
	bool kap;
	QList<formfind> finded;
	MTableModel *model = (MTableModel*)ui.result->model();
	model->setColumnCount(6);
	model->clear();
	findres.clear();
	while (string.size()>0) {
		burr.clear();
		for(i=maxlen;(i>0 && burr.size()==0);i--) {
			current = string.left(i);
			finded=getbackforms(current,"*",0,"");
			curr.clear();
			for (j=0;j<finded.size();j++) {
				curr = scanWords(finded[j],true);
				if (curr.size()==0)
					curr = scanWords(katatohira(finded[j]),true);
				for (k = 0; k < curr.size(); k++) {
					kap = true;
					for(m = 0; m < burr.size(); m++) {
						if (wrdCompare(burr[m].word, curr[k].word))
							kap=false;
					}
					if (kap) {
						curr[k].src = current;
						curr[k].begin=pos;
						curr[k].len=current.size();
						burr.append(curr[k]);
					}
				}
			}
		}
		if (burr.size() != 0) {
			findres.append(burr);
			for(i = 0;i < burr.size(); i++) {
				lst.clear();
				lst.append(burr[i].src);
				if (burr[i].src == burr[i].word.word) {
					lst.append("");
				} else {
					lst.append(burr[i].word.word);
				}
				lst.append(burr[i].word.read);
				lst.append(burr[i].word.type);
				lst.append(burr[i].word.trans);
				lst.append(burr[i].comment);
				model->addrow(lst);
			}
			len=current.size();
		} else {len = 1;}
		pos += len;
		string.remove(0,len);
	}
	model->update();
}
