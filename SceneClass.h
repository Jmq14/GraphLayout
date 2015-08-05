#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H

#include <vtk-6.2/vtkMutableUndirectedGraph.h>
#include <vtk-6.2/vtkSmartPointer.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QString>
#include <QTimer>
#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QColor>
#include <vector>

class Nodes;

class View :public QGraphicsView
{
public:
	View(QWidget *parent = 0);
	QString colorStrategy;
public:
	int timerId;
	int currentScale;

public:
	void timerEvent(QTimerEvent *);
	void itemMoved();
	void drawBackground(QPainter *painter, const QRectF &rect);
	void setColorStrategy(QString);
	void setScale(int slideValue);
};

class Graph:public QGraphicsScene
{
	Q_OBJECT

public:
	Graph();
	void findNodebyNum(Nodes *(&node1), Nodes *(&node2), int num1, int num2);
	void setParentView(View * parent) {parentView = parent;};
	View* getParentView(){return parentView;}
	void wheelEvent(QGraphicsSceneWheelEvent *event);
	void switchAnimation();
	void switchLayoutStrategy(QString strategy);
	Nodes *currentPressedNode;
	QString colorStrategy;

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
	void setCurrentPressedNode(Nodes *node);
	void setColorStrategy(QString);
};

class PCAGraph :public Graph
{
	Q_OBJECT

private:
	QDialog *editWindow;
	QTextEdit *editText;
	QPushButton *button_yes;
	QPushButton *button_cancle;
	QHBoxLayout *Hlayout;
	QVBoxLayout *VLayout;
public:
	PCAGraph();
signals:
	void loadedPCA();
	void sendInformation(QString);
public:
	void LoadData();
	void LoadNodeDataOfPCA(QString inputFileName);
	void LoadEdgeDataOfPCA(QString inputFileName);

	vtkSmartPointer<vtkMutableUndirectedGraph> g;
	void generateLayoutPosition();

	void EditWindow();
	void editProperties();
};

class TopicGraph :public Graph
{
	Q_OBJECT

public:
	TopicGraph();
signals:
	void loadedTopic();
	
public:
	class DocId 
	{
	public:
		int idNum;
		int filePos;
		QString title;
	};

private:
	std::vector<DocId> m_docId;

public:
	void LoadData();
	void LoadNodeDataOfTopic(QString inputFileName);
	void LoadEdgeDataOfTopic(QString inputFileName);
	void LoadDocumentContent(QString inputFileName);
	vtkSmartPointer<vtkMutableUndirectedGraph> g;
	void generateLayoutPosition();

public slots:
	void displayDocument(QString docId);

	
};

#endif