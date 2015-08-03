#ifndef GRAPHICSVISUALIZATION_H
#define GRAPHICSVISUALIZATION_H

#include <QtWidgets/QMainWindow>
#include "ui_graphicsvisualization.h"
#include "SceneClass.h";

class GraphicsVisualization : public QMainWindow
{
	Q_OBJECT

public:
	GraphicsVisualization(QWidget *parent = 0);
	~GraphicsVisualization();

private:
	Ui::GraphicsVisualizationClass ui;
	PCAGraph *s;
	TopicGraph *t;

public slots:
	void setPCAScene();
	void setTopicScene();

};

#endif // GRAPHICSVISUALIZATION_H
