#include "SceneClass.h"
#include "ItemClass.h"
#include "graphicsvisualization.h"
#include "ui_graphicsvisualization.h"

#include <vtk-6.2/vtkSmartPointer.h>
#include <vtk-6.2/vtkMutableUndirectedGraph.h>
#include <vtk-6.2/vtkCircularLayoutStrategy.h>
#include <vtk-6.2/vtkClustering2DLayoutStrategy.h>
#include <vtk-6.2/vtkCommunity2DLayoutStrategy.h>
#include <vtk-6.2/vtkDataSetAttributes.h>
#include <vtk-6.2/vtkDoubleArray.h>
#include <vtk-6.2/vtkGraphLayoutView.h>
#include <vtk-6.2/vtkGraphLayout.h>
#include <vtk-6.2/vtkIntArray.h>
#include <vtk-6.2/vtkMutableUndirectedGraph.h>
#include <vtk-6.2/vtkRenderWindowInteractor.h>
#include <vtk-6.2/vtkPoints.h>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QFile>
#include <QRegExp>
#include <QRectF>
#include <QDebug>
#include <QString>
#include <QGraphicsSceneMouseEvent>
#include <QWheelEvent>

#define VTK_CREATE(type, name) \
	vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

PCAGraph::PCAGraph()
{
	g = vtkSmartPointer<vtkMutableUndirectedGraph>::New();
	currentLayout = clusterLayout;
	QPainter *p = new QPainter;
	setSceneRect(0,0,1000,600);
	p->setBrush(Qt::black);
	drawBackground(p,sceneRect());
}

void Graph::findNodebyNum(Nodes *(&node1), Nodes *(&node2), int num1, int num2)
{
	Nodes *node;
	int flag = 0;
	foreach (QGraphicsItem *item, items())
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			if (node->m_num == num1) 
			{node1 = node; flag++;}
			else if (node->m_num == num2) 
			{node2 = node; flag++;}
		}
		if (flag == 2) 
		{ break; }
	}
}


void PCAGraph::LoadData()
{
	LoadNodeDataOfPCA("Nodes.txt");
	LoadEdgeDataOfPCA("Edges.txt");
	emit loadedPCA();
}

void PCAGraph::LoadNodeDataOfPCA(QString inputFileName)
{
	QFile inputFile(inputFileName);
	if (!inputFile.open(QIODevice::ReadOnly)) return;
	QString tempLine;
	QStringList tempList;
	QRegExp rx("(.*):\\s(.*)\\r\\n");
	QRegExp rxColor("(\\d*),(\\d*),(\\d*),(\\d*)\\)");
	QRegExp rxPos("\\((.*),(.*),(.*)\\)");
	QRegExp rxEmpty("^[a-z]");
	int pos;
	int num;
	bool ok;
	while (!inputFile.atEnd())
	{		
		tempLine = inputFile.readLine();
		num = tempLine.toInt(&ok);
		if (ok)
		{
			tempLine = inputFile.readLine();
			if (tempLine.indexOf(rxEmpty) >= 0)
			{
				pos = rx.indexIn(tempLine);
				tempList = tempLine.split(": ");
				if (rx.cap(1) == "type")
				{
					if (rx.cap(2) == "paper")
					{
						PaperNode *tempItem = new PaperNode(this, num);
						while(tempLine.indexOf(rxEmpty) >= 0)
						{ 
							tempLine = inputFile.readLine();
							
							pos = tempLine.indexOf(rx);
							if (rx.cap(1) == "viewColor") 
							{
								rxColor.indexIn(rx.cap(2));
								tempItem->setViewColor(QColor(rxColor.cap(4).toInt(&ok),
									rxColor.cap(1).toInt(&ok),
									rxColor.cap(2).toInt(&ok),
									rxColor.cap(3).toInt(&ok)
									));

								continue;
							}
							else if (rx.cap(1) == "viewLabel")
							{
								tempItem->setViewLabel(rx.cap(2));
								continue;
							}
							else if (rx.cap(1) == "viewLayout")
							{
								rxPos.indexIn(rx.cap(2));
								tempItem->inputDefaultPosition(rxPos.cap(1).toDouble(&ok),
									rxPos.cap(2).toDouble(&ok),
									rxPos.cap(3).toDouble(&ok));
								continue;
							}
							else{
								tempItem->addInformation(tempLine);
							}
						}
						addItem(tempItem);
						tempItem->idType = tempItem->getScene()->g->AddVertex();

					}
					else if (rx.cap(2) == "conference")
					{
						ConferenceNode *tempItem = new ConferenceNode(this, num);
						while(tempLine.indexOf(rxEmpty) >= 0)
						{ 
							tempLine = inputFile.readLine();

							pos = tempLine.indexOf(rx);
							if (rx.cap(1) == "viewColor") 
							{
								rxColor.indexIn(rx.cap(2));
								tempItem->setViewColor(QColor(rxColor.cap(4).toInt(&ok),
									rxColor.cap(1).toInt(&ok),
									rxColor.cap(2).toInt(&ok),
									rxColor.cap(3).toInt(&ok)));

								continue;
							}
							else if (rx.cap(1) == "viewLabel")
							{
								tempItem->setViewLabel(rx.cap(2));
								continue;
							}
							else if (rx.cap(1) == "viewLayout")
							{
								rxPos.indexIn(rx.cap(2));
								tempItem->inputDefaultPosition(rxPos.cap(1).toDouble(&ok),
									rxPos.cap(2).toDouble(&ok),
									rxPos.cap(3).toDouble(&ok));
								continue;
							}
							else
							{
								tempItem->addInformation(tempLine);
							}
						}
						addItem(tempItem);
						tempItem->idType = tempItem->getScene()->g->AddVertex();
					}
					else if (rx.cap(2) == "author")
					{
						AuthorNode *tempItem = new AuthorNode(this, num);
						while(tempLine.indexOf(rxEmpty) >= 0)
						{ 
							tempLine = inputFile.readLine();
							pos = tempLine.indexOf(rx);
							if (rx.cap(1) == "viewColor") 
							{
								rxColor.indexIn(rx.cap(2));
								tempItem->setViewColor(QColor(rxColor.cap(4).toInt(&ok),
									rxColor.cap(1).toInt(&ok),
									rxColor.cap(2).toInt(&ok),
									rxColor.cap(3).toInt(&ok)));

								continue;
							}
							else if (rx.cap(1) == "viewLabel")
							{
								tempItem->setViewLabel(rx.cap(2));
								continue;
							}
							else if (rx.cap(1) == "viewLayout")
							{
								rxPos.indexIn(rx.cap(2));
								tempItem->inputDefaultPosition(rxPos.cap(1).toDouble(&ok),
									rxPos.cap(2).toDouble(&ok),
									rxPos.cap(3).toDouble(&ok));
								continue;
							}
							else
							{
								tempItem->addInformation(tempLine);
							}
						}
						addItem(tempItem);
						tempItem->idType = tempItem->getScene()->g->AddVertex();

					}
				}
			}
		}
	}
}

void PCAGraph::LoadEdgeDataOfPCA(QString inputFileName)
{
	QFile inputFile(inputFileName);
	if (!inputFile.open(QIODevice::ReadOnly)) return;
	QString tempLine;
	QStringList tempList;
	Nodes *source = NULL;
	Nodes *target = NULL;
	DirectedEdge *newEdge = NULL;
	bool ok;
	while(!inputFile.atEnd())
	{
		tempLine = inputFile.readLine();
		tempList = tempLine.split(" ");
		findNodebyNum(source, target, tempList[0].toInt(&ok), tempList[1].toInt(&ok));
		newEdge = new DirectedEdge(this, source,target);
		addItem(newEdge);
		source->DirectedEdgeList.push_back(newEdge);
		target->DirectedEdgeList.push_back(newEdge);
		g->AddEdge(source->idType, target->idType);
	}
	generateLayoutPosition("default");
}



void PCAGraph::generateLayoutPosition(QString strategy)
{
	if (strategy == "default") currentLayout = defaultLayout;
	if (strategy == "circle") currentLayout = circleLayout;
	if (strategy == "cluster") currentLayout = clusterLayout;
	if (strategy == "community") currentLayout = communityLayout;
	switch (currentLayout)
	{
	case PCAGraph::defaultLayout:
		{
			foreach (QGraphicsItem * item, items())
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					item->setPos(((Nodes*)item)->m_x * 2.5 , ((Nodes*)item)->m_y * 2.5 - 150);		
				}
			}
			break;
		}

	case PCAGraph::circleLayout:
		{
			VTK_CREATE(vtkGraphLayout, layout);
			layout->SetInputData(g);
			VTK_CREATE(vtkCircularLayoutStrategy, circular);
			layout->SetLayoutStrategy(circular);
			layout->Update();
			vtkGraph* outputGraph = layout->GetOutput();
			double pt[3] = {0.0, 0.0, 0.0};
			foreach (QGraphicsItem * item, items())
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					outputGraph->GetPoint(((Nodes*)item)->idType, pt);
					item->setPos(pt[0]* 300 + 500, pt[1]*300 + 300);		
				}
			}
			break;
		}
	case clusterLayout:
		{
			VTK_CREATE(vtkGraphLayout, layout);
			layout->SetInputData(g);
			VTK_CREATE(vtkClustering2DLayoutStrategy, cluster);
			layout->SetLayoutStrategy(cluster);
			layout->Update();
			vtkGraph* outputGraph = layout->GetOutput();
			double pt[3] = {0.0, 0.0, 0.0};
			foreach (QGraphicsItem * item, items())
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					outputGraph->GetPoint(((Nodes*)item)->idType, pt);
					item->setPos(pt[0]* 300 + 180, pt[1]*300 + 550);		
				}
			}
			break;
		}
	case communityLayout:
		{
			VTK_CREATE(vtkGraphLayout, layout);
			layout->SetInputData(g);
			VTK_CREATE(vtkCommunity2DLayoutStrategy, community);
			community->SetCommunityArrayName("paper");
			layout->SetLayoutStrategy(community);
			layout->Update();
			vtkGraph* outputGraph = layout->GetOutput();
			double pt[3] = {0.0, 0.0, 0.0};
			foreach (QGraphicsItem * item, items())
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					outputGraph->GetPoint(((Nodes*)item)->idType, pt);
					item->setPos(pt[0]* 200 + 300, pt[1]*200 + 300);		
				}
			}
			break;
		}
	default:
		break;
	}
}

//��ʱ�رմ˹���
void PCAGraph::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	qDebug() <<"wheel event!";
	qDebug() << event->scenePos().rx() << event->scenePos().ry();
	double scaleFactor = 0.8; //How fast we zoom

	if (event->orientation()==Qt::Vertical){
		if(event->delta() > 0) {

			//Zoom in
			//((PCAGraphicsView *)parent())->scale(scaleFactor, scaleFactor);
			foreach (QGraphicsItem * item, items())
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					((Nodes *)item)->setZoom(scaleFactor, event->scenePos());
				}
			}
		} else {
			scaleFactor = 1.0 / scaleFactor;
			//Zoom out
			//((PCAGraphicsView *)parent())->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
			foreach (QGraphicsItem * item, items())
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					((Nodes *)item)->setZoom(scaleFactor, event->scenePos());					
				}
			}

			//update();
		}
	}
	qDebug() << event->delta();
}

/*
PCAGraphicsView::PCAGraphicsView()
{
	setRenderHint(QPainter::Antialiasing);
	setCacheMode(QGraphicsView::CacheBackground);
	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	//setBackgroundBrush(Qt::black);
}*/

/*
void PCAGraphicsView::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	qDebug() << "wheel event!";
	double scaleFactor = 1.15; //How fast we zoom
	if (event->orientation()==Qt::Vertical){
		if(event->delta() > 0) {
			//Zoom in
			scale(scaleFactor, scaleFactor);
		} else {
			//Zoom out
			scale(1.0 / scaleFactor, 1.0 / scaleFactor);
		}
	}
	else if (event->orientation()==Qt::Horizontal) {
		if(event->delta() > 0) {
			scroll(10,0);
		}
		else {
			scroll(-10,0);
		}
	}
}*/

/*

void PCAGraphicsView::scaleView(double scaleFactor)
{
	qDebug() << scaleFactor;
	double factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	scale(scaleFactor, scaleFactor);
}*/

TopicGraph::TopicGraph()
{
	g = vtkSmartPointer<vtkMutableUndirectedGraph>::New();
	currentLayout = clusterLayout;
	QPainter *p = new QPainter;
	setSceneRect(0,0,1000,600);
	p->setBrush(Qt::black);
	drawBackground(p,sceneRect());
}


void TopicGraph::LoadData()
{
	LoadNodeDataOfTopic("Nodes.txt");
	LoadEdgeDataOfTopic("Edges.txt");
	emit loadedTopic();
}

void TopicGraph::LoadNodeDataOfTopic(QString inputFileName)
{
	QFile inputFile(inputFileName);
	if (!inputFile.open(QIODevice::ReadOnly)) return;
	QString tempLine;
	QStringList tempList;
	QRegExp rx("(.*):\\s(.*)\\r\\n");
	QRegExp rxEmpty("^[a-z]");
	int pos;
	int num;
	bool ok;
	while (!inputFile.atEnd())
	{
		tempLine = inputFile.readLine();
		num = tempLine.toInt(&ok);
		if (ok)
		{
			TopicNode *tempItem = new TopicNode(this, num);
			do 
			{
				tempLine = inputFile.readLine();
				pos = rx.indexIn(tempLine);
				if (pos < 0) break;;
				if (rx.cap(1) == "topicWords")
				{
					tempItem->inputTopicWords(rx.cap(2).split(" "));
				}
				else if (rx.cap(1) == "topicDocuments")
				{
					tempItem->inputTopicDocuments(rx.cap(2).split(" "));
				}
			}while(tempLine.indexOf(rxEmpty) >= 0);
			this->addItem(tempItem);
			tempItem->idType = tempItem->getScene()->g->AddVertex();
		}
	}

}

void TopicGraph::LoadEdgeDataOfTopic(QString inputFileName)
{
	QFile inputFile(inputFileName);
	if (!inputFile.open(QIODevice::ReadOnly)) return;
	QString tempLine;
	QStringList tempList;
	Nodes *node1 = NULL;
	Nodes *node2 = NULL;
	UndirectedEdge *newEdge = NULL;
	bool ok;
	while(!inputFile.atEnd())
	{
		tempLine = inputFile.readLine();
		tempList = tempLine.split(" ");
		//qDebug() << tempList[0] << tempList[1] << tempList[2];
		//qDebug() << tempList[2].toDouble(&ok) << ok;
		findNodebyNum(node1, node2, tempList[0].toInt(&ok), tempList[1].toInt(&ok));
		newEdge = new UndirectedEdge(this, node1, node2, tempList[2].toInt(&ok));
		addItem(newEdge);
		node1->UndirectedEdgeLIst.push_back(newEdge);
		node2->UndirectedEdgeLIst.push_back(newEdge);
		g->AddEdge(node1->idType, node2->idType);
	}
	generateLayoutPosition("default");
}

void TopicGraph::generateLayoutPosition(QString strategy)
{
	if (strategy == "default") currentLayout = defaultLayout;
	if (strategy == "circle") currentLayout = circleLayout;
	if (strategy == "cluster") currentLayout = clusterLayout;
	if (strategy == "community") currentLayout = communityLayout;
	switch (currentLayout)
	{
	case PCAGraph::defaultLayout:
		{
			foreach (QGraphicsItem * item, items())
			{
				VTK_CREATE(vtkGraphLayout, layout);
				layout->SetInputData(g);
				VTK_CREATE(vtkCircularLayoutStrategy, circular);
				layout->SetLayoutStrategy(circular);
				layout->Update();
				vtkGraph* outputGraph = layout->GetOutput();
				double pt[3] = {0.0, 0.0, 0.0};
				foreach (QGraphicsItem * item, items())
				{
					if (item->type() == QGraphicsItem::UserType + 1)
					{
						outputGraph->GetPoint(((Nodes*)item)->idType, pt);
						item->setPos(pt[0]* 300 + 500, pt[1]*300 + 300);		
					}
				}
				break;
			}
			break;
		}

	case PCAGraph::circleLayout:
		{
			VTK_CREATE(vtkGraphLayout, layout);
			layout->SetInputData(g);
			VTK_CREATE(vtkCircularLayoutStrategy, circular);
			layout->SetLayoutStrategy(circular);
			layout->Update();
			vtkGraph* outputGraph = layout->GetOutput();
			double pt[3] = {0.0, 0.0, 0.0};
			foreach (QGraphicsItem * item, items())
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					outputGraph->GetPoint(((Nodes*)item)->idType, pt);
					item->setPos(pt[0]* 300 + 500, pt[1]*300 + 300);		
				}
			}
			break;
		}
	case clusterLayout:
		{
			VTK_CREATE(vtkGraphLayout, layout);
			layout->SetInputData(g);
			VTK_CREATE(vtkClustering2DLayoutStrategy, cluster);
			layout->SetLayoutStrategy(cluster);
			layout->Update();
			vtkGraph* outputGraph = layout->GetOutput();
			double pt[3] = {0.0, 0.0, 0.0};
			foreach (QGraphicsItem * item, items())
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					outputGraph->GetPoint(((Nodes*)item)->idType, pt);
					item->setPos(pt[0]* 20 + 300, pt[1]* 20 + 200);		
				}
			}
			break;
		}
	case communityLayout:
		{
			VTK_CREATE(vtkGraphLayout, layout);
			layout->SetInputData(g);
			VTK_CREATE(vtkCommunity2DLayoutStrategy, community);
			community->SetCommunityArrayName("paper");
			layout->SetLayoutStrategy(community);
			layout->Update();
			vtkGraph* outputGraph = layout->GetOutput();
			double pt[3] = {0.0, 0.0, 0.0};
			foreach (QGraphicsItem * item, items())
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					outputGraph->GetPoint(((Nodes*)item)->idType, pt);
					item->setPos(pt[0]* 200 + 300, pt[1]*200 + 300);		
				}
			}
			break;
		}
	default:
		break;
	}
}
