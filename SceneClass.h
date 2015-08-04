#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H

#include <vtk-6.2/vtkMutableUndirectedGraph.h>
#include <vtk-6.2/vtkSmartPointer.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QString>
#include <QTimer>

class Nodes;

class View :public QGraphicsView
{
public:
	View(QWidget *parent = 0);

public:
	int timerId;

public:
	void timerEvent(QTimerEvent *);
	void itemMoved();
};

class Graph:public QGraphicsScene
{
	Q_OBJECT

public:
	Graph();
	virtual void LoadData() = 0;
	void findNodebyNum(Nodes *(&node1), Nodes *(&node2), int num1, int num2);
	void setParentView(View * parent) {parentView = parent;};
	View* getParentView(){return parentView;}
	void wheelEvent(QGraphicsSceneWheelEvent *event);
	void switchAnimation();
	virtual void switchLayoutStrategy(QString strategy);

public:
	QTimer* switchTimer;
	int switchTimes;

private:
	View* parentView;
protected:	
	bool hasLoaded;

public:
	enum LayoutStrategy {defaultLayout, circleLayout, clusterLayout, fast2DLayout, simple2DLayout, forceDirected2D, spanTree };
	LayoutStrategy currentLayout;
};

class PCAGraph :public Graph
{
	Q_OBJECT

public:
	PCAGraph();
signals:
	void loadedPCA();
public:
	void LoadData();
	void LoadNodeDataOfPCA(QString inputFileName);
	void LoadEdgeDataOfPCA(QString inputFileName);

	vtkSmartPointer<vtkMutableUndirectedGraph> g;
	void generateLayoutPosition();


};

class TopicGraph :public Graph
{
	Q_OBJECT

public:
	TopicGraph();
signals:
	void loadedTopic();
public:
	void LoadData();
	void LoadNodeDataOfTopic(QString inputFileName);
	void LoadEdgeDataOfTopic(QString inputFileName);
	vtkSmartPointer<vtkMutableUndirectedGraph> g;
	void generateLayoutPosition();
};

#endif