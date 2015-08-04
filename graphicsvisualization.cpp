#include "graphicsvisualization.h"
#include "SceneClass.h"
#include "ItemClass.h"
#include <QTextBrowser>
#include <QGraphicsItem>
#include <QDebug>

GraphicsVisualization::GraphicsVisualization(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QMainWindow::setWindowFlags(Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);


	v = new View;

	v->setRenderHint(QPainter::Antialiasing);
	//v->setCacheMode(QGraphicsView::CacheBackground);
	v->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	v->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	v->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	v->setBackgroundBrush(Qt::black);

	s = new PCAGraph;
	t = new TopicGraph;
	//t->LoadNodeDataOfTopic("Nodes.txt");
	//t->LoadEdgeDataOfTopic("Edges.txt");


	ui.graphicsView->setRenderHint(QPainter::Antialiasing);
	ui.graphicsView->setCacheMode(QGraphicsView::CacheBackground);
	ui.graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	ui.graphicsView->setBackgroundBrush(Qt::black);
	ui.graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
	ui.graphicsView->setScene(t);
	s->setParentView(ui.graphicsView);
	t->setParentView(ui.graphicsView);

	ui.comboBoxLayout->addItem("default");
	ui.comboBoxLayout->addItem("cluster");
	ui.comboBoxLayout->addItem("circle");
	ui.comboBoxLayout->addItem("fast2D");
	ui.comboBoxLayout->addItem("simple2D");
	ui.comboBoxLayout->addItem("forceDirected2D");
	ui.comboBoxLayout->addItem("spanTree");
	
	s->LoadData();
	t->LoadData();
	connect(ui.actionLoadPaperConferenceAuthorData, &QAction::triggered, this, &GraphicsVisualization::setPCAScene);
	connect(ui.actionTopic, &QAction::triggered,this, &GraphicsVisualization::setTopicScene);
/*

	connect(ui.actionLoadPaperConferenceAuthorData, &QAction::triggered, s, &PCAGraph::LoadData);
	connect(ui.actionTopic, &QAction::triggered, t, &TopicGraph::LoadData);
	connect(s, &PCAGraph::loadedPCA, this, &GraphicsVisualization::setPCAScene);
	connect(t, &TopicGraph::loadedTopic, this, &GraphicsVisualization::setTopicScene);*/
	qDebug() << "finish the construction";
}

GraphicsVisualization::~GraphicsVisualization()
{
	
}

void GraphicsVisualization::setPCAScene()
{
	ui.graphicsView->setScene(s);
	foreach (QGraphicsItem * item, s->items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			connect(((Nodes *)item), &Nodes::sendInfomation, ui.textBrowser, &QTextBrowser::setText);
		}
	}
	connect(ui.comboBoxLayout, &QComboBox::currentTextChanged, s, &PCAGraph::switchLayoutStrategy);
	disconnect(ui.comboBoxLayout, &QComboBox::currentTextChanged, t, &TopicGraph::switchLayoutStrategy);
	s->switchLayoutStrategy("default");
}

void GraphicsVisualization::setTopicScene()
{
	ui.graphicsView->setScene(t);
	foreach (QGraphicsItem * item, t->items(Qt::AscendingOrder))
	{
		if (item->type() == QGraphicsItem::UserType + 1)
		{
			connect(((Nodes *)item), &Nodes::sendInfomation, ui.textBrowser, &QTextBrowser::setText);
		}
	}
	disconnect(ui.comboBoxLayout, &QComboBox::currentTextChanged, s, &PCAGraph::switchLayoutStrategy);
	connect(ui.comboBoxLayout, &QComboBox::currentTextChanged, t, &TopicGraph::switchLayoutStrategy);
	t->switchLayoutStrategy("default");
}
