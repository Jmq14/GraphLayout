#include "graphicsvisualization.h"
#include "SceneClass.h"
#include "ItemClass.h"
#include <QTextBrowser>
#include <QGraphicsItem>
#include <QDebug>
#include <QButtonGroup>
#include <QRadioButton>

GraphicsVisualization::GraphicsVisualization(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QMainWindow::setWindowFlags(Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

	s = new PCAGraph;
	t = new TopicGraph;

	ui.graphicsView->setRenderHint(QPainter::Antialiasing);
	ui.graphicsView->setCacheMode(QGraphicsView::CacheBackground);
	ui.graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	//设置鼠标模式
	ui.graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
	//ui.graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

	ui.graphicsView->setScene(t);
	s->setParentView(ui.graphicsView);
	t->setParentView(ui.graphicsView);


	ui.graphicsView->setColorStrategy("Dark");
	ui.comboBoxColor->addItem("Dark");
	ui.comboBoxColor->addItem("Light");
	connect(ui.comboBoxColor, &QComboBox::currentTextChanged, ui.graphicsView, &View::setColorStrategy);
	connect(ui.comboBoxColor, &QComboBox::currentTextChanged, s, &PCAGraph::setColorStrategy);
	connect(ui.comboBoxColor, &QComboBox::currentTextChanged, t, &TopicGraph::setColorStrategy);

	ui.comboBoxLayout->addItem("default");
	ui.comboBoxLayout->addItem("cluster");
	ui.comboBoxLayout->addItem("circle");
	ui.comboBoxLayout->addItem("fast2D");
	ui.comboBoxLayout->addItem("simple2D");
	ui.comboBoxLayout->addItem("forceDirected2D");
	ui.comboBoxLayout->addItem("spanTree");
	
	ui.textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	connect(ui.horizontalSlider, &QSlider::valueChanged, ui.graphicsView, &View::setScale);

	ui.buttonGroup->setId(ui.radioButtonNoDrag, 1);
	ui.buttonGroup->setId(ui.radioButtonRubberBand, 2);
	ui.buttonGroup->setId(ui.radioButtonDragHand,3);
	connect(ui.buttonGroup, SIGNAL(buttonPressed(int)), this, SLOT(setGraphDragMode(int)));

	s->LoadData();
	t->LoadData();
	connect(ui.actionLoadPaperConferenceAuthorData, &QAction::triggered, this, &GraphicsVisualization::setPCAScene);
	connect(ui.actionTopic, &QAction::triggered,this, &GraphicsVisualization::setTopicScene);
	connect(s, &PCAGraph::sendInformation, ui.textBrowser, &QTextBrowser::setText);
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
			connect(((Nodes *)item), &Nodes::sendCurrentPressesNode, s, &PCAGraph::setCurrentPressedNode);
		}
	}
	connect(ui.EditProperties, &QPushButton::clicked, s, &PCAGraph::EditWindow);
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
			connect(((TopicNode*)item), &TopicNode::setDocComboBox, this, &GraphicsVisualization::setComboBox);
		}
	}
	connect(ui.pushButtonViewDoc, &QPushButton::clicked, this, &GraphicsVisualization::showDoc);
	disconnect(ui.comboBoxLayout, &QComboBox::currentTextChanged, s, &PCAGraph::switchLayoutStrategy);
	connect(ui.comboBoxLayout, &QComboBox::currentTextChanged, t, &TopicGraph::switchLayoutStrategy);
	t->switchLayoutStrategy("default");
}

void GraphicsVisualization::showDoc()
{
	t->displayDocument(ui.comboBoxViewDoc->currentText());
}

void GraphicsVisualization::setComboBox(TopicNode* node)
{
		ui.comboBoxViewDoc->clear();
		foreach (QString id, node->getTopicDocuments())
		{
			ui.comboBoxViewDoc->addItem(id);
		}
}

void GraphicsVisualization::setGraphDragMode(int mode)
{
	switch (mode)
	{
	case 1:
		ui.graphicsView->setDragMode(QGraphicsView::NoDrag);
		break;
	case 2:
		ui.graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
		break;
	case 3:
		ui.graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
		break;
	default:
		break;
	}
}

