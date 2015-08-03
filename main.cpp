#include "stdafx.h"
#include "graphicsvisualization.h"
#include <QtWidgets/QApplication>
#include <QDesktopWidget>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GraphicsVisualization w;

	w.resize(QApplication::desktop()->availableGeometry().width(),QApplication::desktop()->availableGeometry().height());
	w.show();
	return a.exec();
}
