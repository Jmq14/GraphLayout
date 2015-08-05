#ifndef GRAPHICSVISUALIZATION_H
#define GRAPHICSVISUALIZATION_H

#include <QtWidgets/QMainWindow>
#include "ui_graphicsvisualization.h"
#include "SceneClass.h"
#include "ItemClass.h"

class GraphicsVisualization : public QMainWindow
{
	Q_OBJECT

public:
	GraphicsVisualization(QWidget *parent = 0);
	~GraphicsVisualization();

private:
	Ui::GraphicsVisualizationClass ui;
	View *v;
	PCAGraph *s;
	TopicGraph *t;
	

public slots:
	void setPCAScene();
	void setTopicScene();
	void showDoc();
	void setComboBox(TopicNode* node);
	void setGraphDragMode(int mode);
};

#endif // GRAPHICSVISUALIZATION_H
