#include "graphicsvisualization.h"
#include "SceneClass.h"


GraphicsVisualization::GraphicsVisualization(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QMainWindow::setWindowFlags(Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
	ui.graphicsView->setRenderHint(QPainter::Antialiasing);
	ui.graphicsView->setCacheMode(QGraphicsView::CacheBackground);
	ui.graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	ui.graphicsView->setBackgroundBrush(Qt::black);

	s = new PCAGraph;
	t = new TopicGraph;
	//t->LoadNodeDataOfTopic("Nodes.txt");
	//t->LoadEdgeDataOfTopic("Edges.txt");
	ui.graphicsView->setScene(s);

	ui.comboBoxLayout->addItem("default");
	ui.comboBoxLayout->addItem("cluster");
	ui.comboBoxLayout->addItem("circle");
	
	connect(ui.actionLoadPaperConferenceAuthorData, &QAction::triggered, s, &PCAGraph::LoadData);
	connect(ui.actionTopic, &QAction::triggered, t, &TopicGraph::LoadData);
	connect(s, &PCAGraph::loadedPCA, this, &GraphicsVisualization::setPCAScene);
	connect(t, &TopicGraph::loadedTopic, this, &GraphicsVisualization::setTopicScene);
}

GraphicsVisualization::~GraphicsVisualization()
{
	
}

void GraphicsVisualization::setPCAScene()
{
	ui.graphicsView->setScene(s);
	connect(ui.comboBoxLayout, &QComboBox::currentTextChanged, s, &PCAGraph::generateLayoutPosition);
	disconnect(ui.comboBoxLayout, &QComboBox::currentTextChanged, t, &TopicGraph::generateLayoutPosition);
}

void GraphicsVisualization::setTopicScene()
{
	ui.graphicsView->setScene(t);
	disconnect(ui.comboBoxLayout, &QComboBox::currentTextChanged, s, &PCAGraph::generateLayoutPosition);
	connect(ui.comboBoxLayout, &QComboBox::currentTextChanged, t, &TopicGraph::generateLayoutPosition);
}
