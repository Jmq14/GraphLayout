#include "SceneClass.h"
#include "ItemClass.h"
#include "graphicsvisualization.h"
#include "ui_graphicsvisualization.h"

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
#include <QTimerEvent>
#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QPainterPathStroker>

Nodes::Nodes()
{
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);
	setAcceptHoverEvents(true);
	hoverInfo = NULL;
	viewColor = Qt::white;
	m_radius = 3;
}

void Nodes::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	setCursor(Qt::PointingHandCursor);
	QString shortInfo;
	shortInfo = "No." + QString::number(m_num) + " type:" + m_type 
		+ "\n" + inputLabel.text()
		+ "\nCurrentPosition: (" + QString::number(pos().x()) + ", " + QString::number(pos().y()) + ")";
	hoverInfo = new QLabel(shortInfo);
	hoverInfo->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
	hoverInfo->move(event->screenPos());
	hoverInfo->show();
}

void Nodes::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) 
{
	if (hoverInfo)
	{
		//setCursor(Qt::ArrowCursor);
		hoverInfo->hide();
		delete hoverInfo;
		hoverInfo = NULL;
	}
	return;
}

QRectF Nodes::boundingRect() const
{
	qreal adjust = 3;
	return QRectF( -10 - adjust, -10 - adjust, 20 + adjust, 20 + adjust);
}

void Nodes::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	painter->setPen(Qt::NoPen);
	painter->setBrush(viewColor);
	painter->drawEllipse( -m_radius,  -m_radius, m_radius*2, m_radius*2);
	painter->setPen(viewColor);
	painter->setBrush(Qt::NoBrush);
	painter->drawEllipse(- m_radius-1, -m_radius-1, m_radius*2+2 ,m_radius*2+2);
}

void Nodes::addInformation(QString info)
{
	m_information = m_information + "\n" + info;
}

void Nodes::inputViewColor(QColor c)
{
	inputColor = c;
}

void Nodes::inputViewLabel(QString l)
{
	inputLabel.setText(l);
}

//坐标进行了改动
void Nodes::inputDefaultPosition(double x, double y, double z)
{
	m_x = x * 3 - 100 ;
	m_y = y * 3 - 280;
	m_z = z;
}

//半径为4的圆
QPainterPath Nodes::shape() const
{
	QPainterPath path;
	path.addEllipse(-6, -6, 12, 12);
	return path;
}

void Nodes::mousePressEvent(QGraphicsSceneMouseEvent *ev) 
{
	emit sendCurrentPressesNode(this);
	emit sendInfomation(m_information);
	update();
	QGraphicsItem::mousePressEvent(ev);
}

void Nodes::mouseMoveEvent(QGraphicsSceneMouseEvent *ev)
{
	newPos.setX(ev->scenePos().x());
	newPos.setY(ev->scenePos().y());
	foreach (DirectedEdge *edge, DirectedEdgeList)
		edge->adjust();
	update();
	QGraphicsItem::mouseMoveEvent(ev);
}

void Nodes::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
{
	update();
	QGraphicsItem::mouseReleaseEvent(ev);
}

bool Nodes::advance()
{
	if (newPos == pos())
		return false;

	setPos(newPos);
	return true;
}

void Nodes::calculateForces()
{
	if (!scene() || scene()->mouseGrabberItem() == this) {
		newPos = pos();
		return;
	}
	// Sum up all forces pushing this item away
	qreal xvel = 0;
	qreal yvel = 0;
	foreach (QGraphicsItem *item, scene()->items(Qt::DescendingOrder)) {
		Nodes *node = qgraphicsitem_cast<Nodes *>(item);
		if (!node)
			continue;

		QPointF vec = mapToItem(node, 0, 0);
		qreal dx = vec.x();
		qreal dy = vec.y();
		double l = 2.0 * (dx * dx + dy * dy);
		if (l > 0) {
			xvel += (dx * 150.0) / l;
			yvel += (dy * 150.0) / l;
		}
	}

	// Now subtract all forces pulling items together
	double weight = (DirectedEdgeList.size() + 1) * 10;
	foreach (DirectedEdge *edge, DirectedEdgeList) {
		QPointF vec;
		if (edge->getSourceNode() == this)
			vec = mapToItem(edge->getTargetNode(), 0, 0);
		else
			vec = mapToItem(edge->getSourceNode(), 0, 0);
		xvel -= vec.x() / weight;
		yvel -= vec.y() / weight;
	}
	if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
		xvel = yvel = 0;
	QRectF sceneRect = scene()->sceneRect();
	newPos = pos() + QPointF(xvel, yvel);
	newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
	newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
	//qDebug() << "newPos" << newPos.x() << newPos.y();
}




PaperNode::PaperNode(PCAGraph *parent, int num)
{
	m_scene = parent;
	m_num = num;
	m_type = "Paper Node";
}

void PaperNode::editInformation()
{
	QDialog *editWindow = new QDialog;
	QTextEdit *editText = new QTextEdit(editWindow);
	QPushButton *button_yes = new QPushButton(tr("Yes"),editWindow);
	QPushButton *button_cancle = new QPushButton(tr("Cancel"), editWindow);

}

void PaperNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	if (colorStrategy == "Dark") {
		viewColor = Qt::white;
		m_radius = 2;
	}
	else if (colorStrategy == "Light") {
		viewColor = QColor(124,198,153);
		m_radius = 4;
	}
	painter->setPen(Qt::NoPen);
	painter->setBrush(viewColor);
	painter->drawEllipse( -m_radius,  -m_radius, m_radius*2, m_radius*2);
	painter->setPen(viewColor);
	painter->setBrush(Qt::NoBrush);
	painter->drawEllipse(- m_radius-1, -m_radius-1, m_radius*2+2 ,m_radius*2+2);
}

ConferenceNode::ConferenceNode(PCAGraph *parent, int num)
{
	m_scene = parent;
	m_num = num;
	m_type = "Conference Node";
}

void ConferenceNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	if (colorStrategy == "Dark") {
		viewColor = Qt::white;
		m_radius = 2.5;
	}
	else if (colorStrategy == "Light") {
		viewColor = QColor(242,122,90);
		m_radius = 5;
	}
	painter->setPen(Qt::NoPen);
	painter->setBrush(viewColor);
	painter->drawEllipse( -m_radius,  -m_radius, m_radius*2, m_radius*2);
	painter->setPen(viewColor);
	painter->setBrush(Qt::NoBrush);
	painter->drawEllipse(- m_radius-1, -m_radius-1, m_radius*2+2 ,m_radius*2+2);
}

AuthorNode::AuthorNode(PCAGraph *parent, int num)
{
	m_scene = parent;
	m_num = num;
	m_type = "Author Node";
}

void AuthorNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	if (colorStrategy == "Dark") {
		viewColor = Qt::white;
		m_radius = 3;
	}
	else if (colorStrategy == "Light") {
		viewColor = QColor(239,73,38);
		m_radius = 8;
	}
	painter->setPen(Qt::NoPen);
	painter->setBrush(viewColor);
	painter->drawEllipse( -m_radius,  -m_radius, m_radius*2, m_radius*2);
	painter->setPen(viewColor);
	painter->setBrush(Qt::NoBrush);
	painter->drawEllipse(- m_radius-1, -m_radius-1, m_radius*2+2 ,m_radius*2+2);
}

TopicNode::TopicNode(TopicGraph *parent, int num)
{
	m_scene = parent;
	m_num = num;
	m_type = "Topic Node";
}

void TopicNode::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
	emit setDocComboBox(this);
	emit sendInfomation(m_information);
	setFocus(Qt::MouseFocusReason);
	update();
	QGraphicsItem::mousePressEvent(ev);
}

void TopicNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	if (colorStrategy == "Dark") {
		viewColor = Qt::white;
		m_radius = 4;
	}
	else if (colorStrategy == "Light") {
		viewColor = QColor(124,198,153);
		m_radius = 5;
	}
	painter->setPen(Qt::NoPen);
	painter->setBrush(viewColor);
	painter->drawEllipse( -m_radius,  -m_radius, m_radius*2, m_radius*2);
	painter->setPen(viewColor);
	painter->setBrush(Qt::NoBrush);
	painter->drawEllipse(- m_radius-1, -m_radius-1, m_radius*2+2 ,m_radius*2+2);
}


Edges::Edges()
{
	viewColor = Qt::white;
}

QRectF Edges::boundingRect() const
{
	return QRectF(-800,-800, 1600, 1600);
}


QPainterPath DirectedEdge::shape() const
{
	QPainterPath path;
	if ((!sourceNode) || (!targetNode)) return path;
	else 
	{
		QPainterPath Line(sourceNode->pos());
		Line.lineTo(targetNode->pos());
		return Line;
	}
}

DirectedEdge::DirectedEdge(QGraphicsScene *parent, Nodes* source /* = NULL */, Nodes *target /* = NULL */, double weight /* = 1 */)
{
	m_scene = parent;
	sourceNode = source;
	targetNode = target;
	m_weight = weight;
	setAcceptedMouseButtons(0);
}

void DirectedEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{
	if ((getSourceNode() == NULL) || (getTargetNode() == NULL)) return;
	if (colorStrategy == "Dark") {
		viewColor = Qt::white;
	}
	else if (colorStrategy == "Light") {
		viewColor = QColor(200,200,200);
	}

	QPen pen;
	pen.setColor(viewColor);
	pen.setWidth(0.2);
	painter->setPen(pen);
	painter->drawLine(getSourceNode()->pos(), getTargetNode()->pos());
}

void DirectedEdge::adjust()
{
	if (!getSourceNode() || !getTargetNode())
		return;

/*
	QLineF line(mapFromItem(getSourceNode(), 0, 0), mapFromItem(getTargetNode(), 0, 0));
	qreal length = line.length();

	prepareGeometryChange();

	if (length > qreal(20.)) {
		QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
		sourceNode->setPos(line.p1() + edgeOffset)  ;
		targetNode->setPos(line.p2() - edgeOffset) ;
		update();
	} else {
		sourceNode->setPos(line.p1());
		targetNode->setPos(line.p1());
	}*/
	prepareGeometryChange();
	update();
}


UndirectedEdge::UndirectedEdge(QGraphicsScene *parent, Nodes* Node1 /* = NULL */, Nodes *Node2 /* = NULL */, double weight /* = 1 */)
{
	m_scene = parent;
	m_node1 = Node1;
	m_node2 = Node2;
	m_weight = weight;
	setAcceptedMouseButtons(0);
}

void UndirectedEdge::adjust()
{
	if (!getNode1() || !getNode2())
		return;
	prepareGeometryChange();
	update();
}

void UndirectedEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{
	if ((getNode1() == NULL) || (getNode2() == NULL)) return;

	if ((getNode1() == NULL) || (getNode2() == NULL)) return;
	if (colorStrategy == "Dark") {
		viewColor = Qt::white;
	}
	else if (colorStrategy == "Light") {
		viewColor = QColor(200,200,200);
	}
	QPen pen;
	pen.setColor(viewColor);
	pen.setWidth(m_weight);
	painter->setPen(pen);
	painter->drawLine(getNode1()->pos(), getNode2()->pos());
}

QPainterPath UndirectedEdge::shape() const
{
	QPainterPath path;
	if ((!m_node1) || (!m_node2)) return path;
	else 
	{
		QPainterPath Line(m_node1->pos());
		Line.lineTo(m_node2->pos());
		return Line;
	}
}

