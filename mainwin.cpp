#include <QtGui>

#include "vars.h"
#include "classes.h"

QList<dictfind> findres;
extern QMap<QString, kanjitem> kanji;

MainWin::MainWin(QWidget *parent):QMainWindow(parent) {
	ui.setupUi(this);

	ui.result->weight[0] = .1;
	ui.result->weight[1] = .1;
	ui.result->weight[2] = .1;
	ui.result->weight[3] = .1;
	ui.result->weight[4] = .40;
	ui.result->weight[5] = .20;
	cbrd = QApplication::clipboard();

	QObject::connect(ui.leText,SIGNAL(textChanged(QString)),this,SLOT(gotranslate()));
	QObject::connect(ui.tbNew,SIGNAL(released()),this,SLOT(waneword()));
	QObject::connect(ui.tbReload,SIGNAL(released()),this,SLOT(reload()));
	QObject::connect(ui.tbForms,SIGNAL(released()),this,SIGNAL(wannaform()));
	QObject::connect(ui.tbDelete,SIGNAL(released()),this,SLOT(delword()));
	QObject::connect(ui.result,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(wanoldword(QModelIndex)));
	QObject::connect(ui.result,SIGNAL(indexchanged(int)),this,SLOT(reselect(int)));
	QObject::connect(cbrd,SIGNAL(dataChanged()),this,SLOT(tranCbrd()));

	opt.setDefaultFormat(QSettings::IniFormat);
	opt.setValue("global/test", 1);
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
	loadKanji("kana.jrk", 1);
	loadKanji("kanji.jrk");
	gotranslate();
}

void MainWin::tranCbrd() {
	if (!ui.cbClipboard->isChecked()) return;
	ui.leText->setText(cbrd->text());
}

void addfindkanji(MTableModel* model, QString kanjisrc, QString kanjird, int pos) {
	dictfind df;
	QStringList lst;
	df.src = kanjisrc;
	df.len = kanjisrc.size();
	df.begin = pos - df.len;
	findres.append(df);
	lst.append(kanjisrc);
	lst.append("");
	lst.append(kanjird);
	lst.append("");
	lst.append("");
	model->addrow(lst);
}

void MainWin::gotranslate() {
	QString string = ui.leText->text();
	QString current;
	QString ch;
	QStringList lst;
	QString kanjisrc;
	QString kanjird;
	QList<dictfind> curr,burr;
	dictfind df;
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
		if (burr.size() > 0) {
			if (kanjisrc.size() > 0) {
				addfindkanji(model, kanjisrc, kanjird, pos);
				kanjisrc.clear();
				kanjird.clear();
			}
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
		} else {
			len = 2;
			ch = string.left(2);
			if (!kanji.contains(ch)) {
				len = 1;
				ch = string.left(1);
				if (!kanji.contains(ch))
					len = 0;
			}
			if (len == 0) {
				if (kanjisrc.size() > 0) {
					addfindkanji(model, kanjisrc, kanjird, pos);
					kanjisrc.clear();
					kanjird.clear();
				}
				len = 1;
			} else {
				kanjisrc.append(ch);
				kanjird.append(kanji[ch].rd_on);
			}
		}
		pos += len;
		string.remove(0,len);
	}
	if (!kanjisrc.isEmpty()) {
		addfindkanji(model, kanjisrc, kanjird, pos);
	}
	model->update();
}
