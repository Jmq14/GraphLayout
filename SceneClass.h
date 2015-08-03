#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H

#include <vtk-6.2/vtkMutableUndirectedGraph.h>
#include <vtk-6.2/vtkSmartPointer.h>
#include <QGraphicsScene>
#include <QString>

class Nodes;

class Graph:public QGraphicsScene
{
	Q_OBJECT

public:
	virtual void LoadData() = 0;
	void findNodebyNum(Nodes *(&node1), Nodes *(&node2), int num1, int num2);
public:
	enum LayoutStrategy {defaultLayout, circleLayout, clusterLayout, communityLayout };
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
	void wheelEvent(QGraphicsSceneWheelEvent *event);
	vtkSmartPointer<vtkMutableUndirectedGraph> g;
	void generateLayoutPosition(QString);


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
	void generateLayoutPosition(QString);
};

#endif