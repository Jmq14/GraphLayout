#include "SceneClass.h"
#include "ItemClass.h"
#include "graphicsvisualization.h"
#include "ui_graphicsvisualization.h"

#include <vtk-6.2/vtkSmartPointer.h>
#include <vtk-6.2/vtkMutableUndirectedGraph.h>
#include <vtk-6.2/vtkCircularLayoutStrategy.h>
#include <vtk-6.2/vtkClustering2DLayoutStrategy.h>
#include <vtk-6.2/vtkFast2DLayoutStrategy.h>
#include <vtk-6.2/vtkSimple2DLayoutStrategy.h>
#include <vtk-6.2/vtkForceDirectedLayoutStrategy.h>
#include <vtk-6.2/vtkSpanTreeLayoutStrategy.h>
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
#include <QWidget>
#include <QPainter>
#include <QFile>
#include <QRegExp>
#include <QRectF>
#include <QDebug>
#include <QString>
#include <QGraphicsSceneMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QTimerEvent>
#include <QTextBrowser>
#include <QDialog>
#include <QSizePolicy>
#include <QGradient>

#define VTK_CREATE(type, name) \
	vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

View::View(QWidget *parent /* = 0 */)
{
	timerId = 0;
	currentScale = 100;
}

void View::itemMoved()
{
	/*if (timerId != 0)
		timerId = startTimer(1000 / 25);*/
	//qDebug() << "itemMoved";
}

void View::timerEvent(QTimerEvent * event)
{
	Q_UNUSED(event);

/*
	qDebug() << "timer Event";

	QList<Nodes *> nodes;
	foreach (QGraphicsItem *item, scene()->items(Qt::DescendingOrder)) {
		if (item->type() == QGraphicsItem::UserType + 1)
			nodes << (Nodes *)item;
	}

	foreach (Nodes *node, nodes)
		node->calculateForces();

	bool itemsMoved = false;
	foreach (Nodes *node, nodes) {
		if (node->advance())
			itemsMoved = true;
	}

	if (!itemsMoved) {
		killTimer(timerId);
		timerId = 0;
	}*/
}

void View::drawBackground(QPainter *painter, const QRectF &rect)
{
	if (colorStrategy == "Dark")
	{
		QLinearGradient darkBack(sceneRect().width()/2,0,sceneRect().width()/2,sceneRect().height());
		darkBack.setColorAt(0.0, Qt::black);
		darkBack.setColorAt(0.7, QColor(166,201,232));
		darkBack.setColorAt(1.0, QColor(95,133,159));
		painter->setBrush(darkBack);
		painter->drawRect(rect);
	}
	else if (colorStrategy == "Light")
	{
		painter->setBrush(QColor(252,250,242));
		painter->drawRect(rect);
	}
}

void View::setColorStrategy(QString strategy)
{
	colorStrategy = strategy;
	resetCachedContent();
}

void View::setScale(int slideValue)
{
	double scaleFactor;
	scaleFactor = (double)(slideValue) / (double)currentScale ;
	scale(scaleFactor, scaleFactor);
	currentScale = slideValue;
}


Graph::Graph()
{
	parentView = NULL;
	currentPressedNode = NULL;
	switchTimer = new QTimer;
	switchTimer->setInterval(25);
	switchTimes = 1;
	hasLoaded = false;
	connect(switchTimer, &QTimer::timeout, this, &Graph::switchAnimation);
}

void Graph::findNodebyNum(Nodes *(&node1), Nodes *(&node2), int num1, int num2)
{
	Nodes *node;
	int flag = 0;
	foreach (QGraphicsItem* item, items(Qt::AscendingOrder))
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

void Graph::switchLayoutStrategy(QString strategy)
{
	if (strategy == "default") currentLayout = defaultLayout;
	if (strategy == "circle") currentLayout = circleLayout;
	if (strategy == "cluster") currentLayout = clusterLayout;
	if (strategy == "fast2D") currentLayout = fast2DLayout;
	if (strategy == "simple2D") currentLayout = simple2DLayout;
	if (strategy == "forceDirected2D") currentLayout = forceDirected2D;
	if (strategy == "spanTree") currentLayout = spanTree;

	switch (currentLayout)
	{
	case PCAGraph::defaultLayout:
		{
			foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					((Nodes *)item)->newPos.setX(((Nodes*)item)->strategyPos[0].x());
					((Nodes *)item)->newPos.setY(((Nodes*)item)->strategyPos[0].y());
					//item->setPos(((Nodes*)item)->m_x * 2.5 , ((Nodes*)item)->m_y * 2.5 - 150);
					((Nodes *)item)->oldPos.setX(item->pos().x());
					((Nodes *)item)->oldPos.setY(item->pos().y());
				}
				else break;
			}
			switchTimer->start();
			break;
		}

	case PCAGraph::circleLayout:
		{
			foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					((Nodes *)item)->newPos.setX(((Nodes*)item)->strategyPos[1].x());
					((Nodes *)item)->newPos.setY(((Nodes*)item)->strategyPos[1].y());
					((Nodes *)item)->oldPos.setX(item->pos().x());
					((Nodes *)item)->oldPos.setY(item->pos().y());	
				}
				else break;
			}
			switchTimer->start();
			break;
		}
	case clusterLayout:
		{
			foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
			{
				if (item->type() == QGraphicsItem::UserType + 1)
				{
					((Nodes *)item)->newPos.setX(((Nodes*)item)->strategyPos[2].x());
					((Nodes *)item)->newPos.setY(((Nodes*)item)->strategyPos[2].y());
					((Nodes *)item)->oldPos.setX(item->pos().x());
					((Nodes *)item)->oldPos.setY(item->pos().y());	
				}
				else break;
			}
			switchTimer->start();
			break;
		}
	case fast2DLayout:
		foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
		{
			if (item->type() == QGraphicsItem::UserType + 1)
			{
				((Nodes *)item)->newPos.setX(((Nodes*)item)->strategyPos[3].x());
				((Nodes *)item)->newPos.setY(((Nodes*)item)->strategyPos[3].y());
				((Nodes *)item)->oldPos.setX(item->pos().x());
				((Nodes *)item)->oldPos.setY(item->pos().y());	
			}
			else break;
		}
		switchTimer->start();
		break;
	case simple2DLayout:
		foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
		{
			if (item->type() == QGraphicsItem::UserType + 1)
			{
				((Nodes *)item)->newPos.setX(((Nodes*)item)->strategyPos[4].x());
				((Nodes *)item)->newPos.setY(((Nodes*)item)->strategyPos[4].y());
				((Nodes *)item)->oldPos.setX(item->pos().x());
				((Nodes *)item)->oldPos.setY(item->pos().y());	
			}
			else break;
		}
		switchTimer->start();
		break;
	case forceDirected2D:
		foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
		{
			if (item->type() == QGraphicsItem::UserType + 1)
			{
				((Nodes *)item)->newPos.setX(((Nodes*)item)->strategyPos[5].x());
				((Nodes *)item)->newPos.setY(((Nodes*)item)->strategyPos[5].y());
				((Nodes *)item)->oldPos.setX(item->pos().x());
				((Nodes *)item)->oldPos.setY(item->pos().y());	
			}
			else break;
		}
		switchTimer->start();
		break;
	case spanTree:
		foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
		{
			if (item->type() == QGraphicsItem::UserType + 1)
			{
				((Nodes *)item)->newPos.setX(((Nodes*)item)->strategyPos[6].x());
				((Nodes *)item)->newPos.setY(((Nodes*)item)->strategyPos[6].y());
				((Nodes *)item)->oldPos.setX(item->pos().x());
				((Nodes *)item)->oldPos.setY(item->pos().y());	
			}
			else break;
		}
		switchTimer->start();
		break;
	}
}

void Graph::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	double scaleFactor = 1.15; //How fast we zoom

	if (event->orientation()==Qt::Vertical){
		if(event->delta() > 0) {
			//Zoom in
			(getParentView())->scale(scaleFactor, scaleFactor);
		} else {
			scaleFactor = 1.0 / scaleFactor;
			//Zoom out
			(getParentView())->scale(scaleFactor,  scaleFactor);
		}
	}
}

void Graph::switchAnimation()
{
	double tempX, tempY;
	if ((switchTimes < 15) && (switchTimes > 0))
	{
		foreach (QGraphicsItem * item, items())
		{
			if (item->type() == QGraphicsItem::UserType + 1)
			{
				tempX = ((Nodes *)item)->newPos.x() * switchTimes 
					- ((Nodes *)item)->oldPos.x() * (switchTimes - 15);
				tempX = tempX / 15.0;
				tempY = ((Nodes *)item)->newPos.y() * switchTimes 
					- ((Nodes *)item)->oldPos.y() * (switchTimes - 15);
				tempY = tempY / 15.0;
				item->setPos(tempX, tempY);		
			}
		}
		switchTimes ++;
	}
	else if (switchTimes == 15)
	{
		foreach (QGraphicsItem * item, items())
		{
			if (item->type() == QGraphicsItem::UserType + 1)
			{
				item->setPos(((Nodes *)item)->newPos.x(),((Nodes *)item)->newPos.y());
				switchTimes = 1;
				switchTimer->stop();

			}
		}
	}

	else return;
}

void Graph::setCurrentPressedNode(Nodes *node)
{
	currentPressedNode = node;
}

void Graph::setColorStrategy(QString strategy)
{
	foreach (QGraphicsItem* item, items())
	{
		if (item->type() == QGraphicsItem::UserType + 1)
			((Nodes *)item)->setColorStrategy(strategy);  //viewColor = Qt::white;
		if (item->type() == QGraphicsItem::UserType + 2)
			((Edges *)item)->setColorStrategy(strategy);
	}
}


PCAGraph::PCAGraph()
{
	g = vtkSmartPointer<vtkMutableUndirectedGraph>::New();
	currentLayout = clusterLayout;
	QPainter *p = new QPainter;
	setSceneRect(0,0,1000,600);

	editWindow = new QDialog;
	editWindow->setWindowFlags(Qt::WindowStaysOnTopHint);
	editText = new QTextEdit();

	button_yes = new QPushButton("Save");
	button_cancle = new QPushButton("Cancel");
	Hlayout = new QHBoxLayout;
	button_yes->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	button_cancle->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	Hlayout->addWidget(button_yes);
	Hlayout->addWidget(button_cancle);
	VLayout = new QVBoxLayout(editWindow);
	VLayout->addWidget(editText);
	VLayout->addLayout(Hlayout);

	editWindow->resize(400,600);
	editText->resize(400,550);

	connect(button_yes, &QPushButton::clicked, this, &PCAGraph::editProperties);
	connect(button_yes, &QPushButton::clicked, editWindow, &QDialog::close);
	connect(button_cancle, &QPushButton::clicked, editWindow, &QDialog::close);
}

void PCAGraph::LoadData()
{
	if (!hasLoaded)
	{
		getParentView()->timerId = 0;
		LoadNodeDataOfPCA("PCANodes.txt");
		LoadEdgeDataOfPCA("PCAEdges.txt");
		hasLoaded = true;
		generateLayoutPosition();
	}
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
						tempItem->addInformation("Node type: Paper\n");
						while(tempLine.indexOf(rxEmpty) >= 0)
						{ 
							tempLine = inputFile.readLine();
							
							pos = tempLine.indexOf(rx);
							if (rx.cap(1) == "viewColor") 
							{
								rxColor.indexIn(rx.cap(2));
								tempItem->inputViewColor(QColor(rxColor.cap(4).toInt(&ok),
									rxColor.cap(1).toInt(&ok),
									rxColor.cap(2).toInt(&ok),
									rxColor.cap(3).toInt(&ok)
									));
							}
							else if (rx.cap(1) == "viewLabel")
							{
								tempItem->inputViewLabel(rx.cap(2));
							}
							else if (rx.cap(1) == "viewLayout")
							{
								rxPos.indexIn(rx.cap(2));
								tempItem->inputDefaultPosition(rxPos.cap(1).toDouble(&ok),
									rxPos.cap(2).toDouble(&ok),
									rxPos.cap(3).toDouble(&ok));
								tempItem->setPos(sceneRect().width()/2,sceneRect().height()/2);
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
						tempItem->addInformation("Node type: Conference\n");
						while(tempLine.indexOf(rxEmpty) >= 0)
						{ 
							tempLine = inputFile.readLine();

							pos = tempLine.indexOf(rx);
							if (rx.cap(1) == "viewColor") 
							{
								rxColor.indexIn(rx.cap(2));
								tempItem->inputViewColor(QColor(rxColor.cap(4).toInt(&ok),
									rxColor.cap(1).toInt(&ok),
									rxColor.cap(2).toInt(&ok),
									rxColor.cap(3).toInt(&ok)));
							}
							else if (rx.cap(1) == "viewLabel")
							{
								tempItem->inputViewLabel(rx.cap(2));
							}
							else if (rx.cap(1) == "viewLayout")
							{
								rxPos.indexIn(rx.cap(2));
								tempItem->inputDefaultPosition(rxPos.cap(1).toDouble(&ok),
									rxPos.cap(2).toDouble(&ok),
									rxPos.cap(3).toDouble(&ok));
								tempItem->setPos(sceneRect().width()/2,sceneRect().height()/2);
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
						tempItem->addInformation("Node type: Author\n");
						while(tempLine.indexOf(rxEmpty) >= 0)
						{ 
							tempLine = inputFile.readLine();
							pos = tempLine.indexOf(rx);
							if (rx.cap(1) == "viewColor") 
							{
								rxColor.indexIn(rx.cap(2));
								tempItem->inputViewColor(QColor(rxColor.cap(4).toInt(&ok),
									rxColor.cap(1).toInt(&ok),
									rxColor.cap(2).toInt(&ok),
									rxColor.cap(3).toInt(&ok)));
							}
							else if (rx.cap(1) == "viewLabel")
							{
								tempItem->inputViewLabel(rx.cap(2));
							}
							else if (rx.cap(1) == "viewLayout")
							{
								rxPos.indexIn(rx.cap(2));
								tempItem->inputDefaultPosition(rxPos.cap(1).toDouble(&ok),
									rxPos.cap(2).toDouble(&ok),
									rxPos.cap(3).toDouble(&ok));
								tempItem->setPos(sceneRect().width()/2,sceneRect().height()/2);
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
	
}

void PCAGraph::generateLayoutPosition()
{
	Nodes * node;
	vtkGraph *outputGraph;
	double pt[3] = {0.0, 0.0, 0.0};
	VTK_CREATE(vtkGraphLayout, layout);
	layout->SetInputData(g);

	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes*)item;
			node->strategyPos[0].setX(node->m_x);
			node->strategyPos[0].setY(node->m_y);	
		}
		else break;
	}

	VTK_CREATE(vtkCircularLayoutStrategy, circular);
	layout->SetLayoutStrategy(circular);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[1].setX(pt[0]* 300 + 500);
			node->strategyPos[1].setY(pt[1]*300 + 300);
		}
		else break;
	}

	VTK_CREATE(vtkClustering2DLayoutStrategy, cluster);
	layout->SetLayoutStrategy(cluster);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[2].setX(pt[0]* 300 + 180);
			node->strategyPos[2].setY(pt[1]*300 + 550);
		}
		else break;
	}

	VTK_CREATE(vtkFast2DLayoutStrategy, fast2D);
	layout->SetLayoutStrategy(fast2D);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[3].setX(pt[0]* 200 + 300);
			node->strategyPos[3].setY(pt[1]* 200 + 300);
		}
		else break;
	}

	VTK_CREATE(vtkSimple2DLayoutStrategy, simple2D);
	layout->SetLayoutStrategy(simple2D);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[4].setX(pt[0]* 50 + 300);
			node->strategyPos[4].setY(pt[1]* 50 + 200);
		}
		else break;
	}

	VTK_CREATE(vtkForceDirectedLayoutStrategy, forceDirected2D);
	layout->SetLayoutStrategy(forceDirected2D);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[5].setX(pt[0]* 500 + 400);
			node->strategyPos[5].setY(pt[1]* 500 + 300);
		}
		else break;
	}

	VTK_CREATE(vtkSpanTreeLayoutStrategy, spanTree);
	layout->SetLayoutStrategy(spanTree);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[6].setX(pt[0] * 15 + 200);
			node->strategyPos[6].setY(pt[1] * 15 + 200);
		}
		else break;
	}
	qDebug() <<"generate PCA!";
}

void PCAGraph::EditWindow()
{
	if (!currentPressedNode) return;

	editText->setText(currentPressedNode->m_information);
	editWindow->show();


}

void PCAGraph::editProperties()
{
	currentPressedNode->m_information = editText->toPlainText();
	emit sendInformation(currentPressedNode->m_information);
}



TopicGraph::TopicGraph()
{
	g = vtkSmartPointer<vtkMutableUndirectedGraph>::New();
	currentLayout = clusterLayout;
	QPainter *p = new QPainter;
	setSceneRect(0,0,1000,600);
}

void TopicGraph::LoadData()
{
	if (!hasLoaded)
	{
		LoadNodeDataOfTopic("TopicNodes.txt");
		LoadEdgeDataOfTopic("TopicEdges.txt");
		LoadDocumentContent("TopicDocumentContent.txt");
		generateLayoutPosition();
		hasLoaded = true;
	}

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
			tempItem->addInformation("Node Type: Topic Node\n");
			do 
			{
				tempLine = inputFile.readLine();
				tempItem->addInformation(tempLine);
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
			tempItem->setPos(sceneRect().width()/2,sceneRect().height()/2);
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
		findNodebyNum(node1, node2, tempList[0].toInt(&ok), tempList[1].toInt(&ok));
		newEdge = new UndirectedEdge(this, node1, node2, tempList[2].toDouble(&ok)* 4);
		this->addItem(newEdge);
		node1->UndirectedEdgeLIst.push_back(newEdge);
		node2->UndirectedEdgeLIst.push_back(newEdge); //to debug
		g->AddEdge(node1->idType, node2->idType);
	}
}

void TopicGraph::LoadDocumentContent(QString inputFileName)
{
	QFile inputFile(inputFileName);
	if (!inputFile.open(QIODevice::ReadOnly)) return;
	QString tempLine;
	int num = 0;
	bool ok = false;
	while (!inputFile.atEnd())
	{
		tempLine = inputFile.readLine();
		num = tempLine.toInt(&ok);
		if (ok)
		{
			DocId newid;
			newid.idNum = num;
			newid.title = inputFile.readLine();
			newid.filePos = inputFile.pos();
			m_docId.push_back(newid);
		}
	}
}

void TopicGraph::generateLayoutPosition()
{
	Nodes * node;
	vtkGraph *outputGraph;
	double pt[3] = {0.0, 0.0, 0.0};
	VTK_CREATE(vtkGraphLayout, layout);
	layout->SetInputData(g);

	VTK_CREATE(vtkCircularLayoutStrategy, defaultS);
	layout->SetLayoutStrategy(defaultS);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[0].setX(pt[0]* 300 + 500);
			node->strategyPos[0].setY(pt[1]*300 + 300);
		}
		else break;
	}

	VTK_CREATE(vtkCircularLayoutStrategy, circular);
	layout->SetLayoutStrategy(circular);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[1].setX(pt[0]* 300 + 500);
			node->strategyPos[1].setY(pt[1]*300 + 300);
		}
		else break;
	}

	VTK_CREATE(vtkClustering2DLayoutStrategy, cluster);
	layout->SetLayoutStrategy(cluster);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[2].setX(pt[0]* 15 + 300);
			node->strategyPos[2].setY(pt[1]* 15 + 300);
		}
		else break;
	}

	VTK_CREATE(vtkFast2DLayoutStrategy, fast2D);
	layout->SetLayoutStrategy(fast2D);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[3].setX(pt[0]* 50 + 400);
			node->strategyPos[3].setY(pt[1]* 50 + 300);
		}
		else break;
	}

	VTK_CREATE(vtkSimple2DLayoutStrategy, simple2D);
	layout->SetLayoutStrategy(simple2D);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[4].setX(pt[0]* 15 + 400);
			node->strategyPos[4].setY(pt[1]* 15 + 300);
		}
		else break;
	}

	VTK_CREATE(vtkForceDirectedLayoutStrategy, forceDirected2D);
	layout->SetLayoutStrategy(forceDirected2D);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[5].setX(pt[0]* 400 + 400);
			node->strategyPos[5].setY(pt[1]* 400 + 300);
		}
		else break;
	}

	VTK_CREATE(vtkSpanTreeLayoutStrategy, spanTree);
	layout->SetLayoutStrategy(spanTree);
	layout->Update();
	outputGraph = layout->GetOutput();
	foreach (QGraphicsItem * item, items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			node = (Nodes *)item;
			outputGraph->GetPoint(((Nodes*)item)->idType, pt);
			node->strategyPos[6].setX(pt[0]* 18 + 800);
			node->strategyPos[6].setY(pt[1]* 18 + 300);
		}
		else break;
	}
	
	qDebug() << "generate!";
}

void TopicGraph::displayDocument(QString docId)
{
	bool ok;
	bool flag = false;
	int num = docId.toInt(&ok);
	if(!ok) return;
	QDialog *viewDialog = new QDialog;
	QTextBrowser *docView = new QTextBrowser(viewDialog);
	docView->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	std::vector<DocId>::iterator r = m_docId.begin();
	for ( ; r != m_docId.end(); r++)
	{
		if (r->idNum == num) { flag = true; break;}
	}
	if (flag == false) 
	{
		return;
	}
	QFile docFile("TopicDocumentContent.txt");
	QString passage;
	if (docFile.open(QIODevice::ReadOnly))
	{
		docFile.seek(r->filePos);
		passage = docFile.readLine();
	}
	docView->setText(r->title + "\n" + passage);
	docView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	docView->resize(400, 600);
	viewDialog->resize(400, 600);
	viewDialog->show();
}

