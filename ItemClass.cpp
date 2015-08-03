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

Nodes::Nodes()
{
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);
}

PaperNode::PaperNode(PCAGraph *parent, int num)
{
	m_scene = parent;
	m_num = num;
}

ConferenceNode::ConferenceNode(PCAGraph *parent, int num)
{
	m_scene = parent;
	m_num = num;
}

AuthorNode::AuthorNode(PCAGraph *parent, int num)
{
	m_scene = parent;
	m_num = num;
}

TopicNode::TopicNode(TopicGraph *parent, int num)
{
	m_scene = parent;
	m_num = num;
}

QRectF Nodes::boundingRect() const
{
	qreal adjust = 2;
	return QRectF( -5 - adjust, -5 - adjust, 10 + adjust, 10 + adjust);
}

void Nodes::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	//qDebug() <<"painted nodes!";
	/*if ((pos().rx() < 0) || (pos().rx() > 1000 / *scene()->sceneRect().x()* /) 
		||(pos().ry() < 0) || (pos().ry() > 1000 / *scene()->sceneRect().y()* /) )
		return;*/
	painter->setPen(Qt::NoPen);
	painter->setBrush(Qt::white);
	painter->drawEllipse( - 4,  - 4,8,8);
	painter->setPen(Qt::white);
	painter->setBrush(Qt::NoBrush);
	painter->drawEllipse(- 5,- 5,10,10);
}

void Nodes::addInformation(QString info)
{
	m_information = m_information + "\n" + info;
}

void Nodes::setViewColor(QColor c)
{
	viewColor = c;
}

void Nodes::setViewLabel(QString l)
{
	viewLabel.setText(l);
}

//坐标进行了改动
void Nodes::inputDefaultPosition(double x, double y, double z)
{
	m_x = x ;
	m_y = y ;
	m_z = z;
}

//半径为5的圆
QPainterPath Nodes::shape() const
{
	QPainterPath path;
	path.addEllipse(-5, -5, 10, 10);
	return path;
}

void Nodes::mousePressEvent(QGraphicsSceneMouseEvent *ev) 
{
	update();
	QGraphicsItem::mousePressEvent(ev);
}

void Nodes::mouseMoveEvent(QGraphicsSceneMouseEvent *ev)
{
	setPos(ev->scenePos());
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
	foreach (QGraphicsItem *item, scene()->items()) {
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
}

void Nodes::setZoom(double scaleFactor, QPointF MPos)
{
	double nx = 0, ny = 0;
	nx = MPos.rx()
		+ (pos().rx() - MPos.rx()) / scaleFactor;
	ny = MPos.ry()
		+ (pos().ry() - MPos.ry()) / scaleFactor;
	setPos(nx, ny);
	prepareGeometryChange();
}

QVariant Nodes::itemChange(GraphicsItemChange change, const QVariant &value)
{
	switch (change) {
	case ItemPositionHasChanged:
		foreach (DirectedEdge *edge, DirectedEdgeList)
			edge->adjust();
		//graph->itemMoved();
		break;
	default:
		break;
	};

	return QGraphicsItem::itemChange(change, value);
}

DirectedEdge::DirectedEdge(QGraphicsScene *parent, Nodes* source /* = NULL */, Nodes *target /* = NULL */, int weight /* = 1 */)
{
	m_scene = parent;
	sourceNode = source;
	targetNode = target;
	m_weight = weight;
	setAcceptedMouseButtons(0);
}

//边长为1600的正方形
QRectF Edges::boundingRect() const
{
	return QRectF(-800,-800, 1600, 1600);
}

void DirectedEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */ )
{
	//qDebug() << "painted edge!";
	painter->setPen(Qt::white);
	if ((getSourceNode() == NULL) || (getTargetNode() == NULL)) return;
	painter->drawLine(getSourceNode()->pos().rx(),getSourceNode()->pos().ry(),
		getTargetNode()->pos().rx(), getTargetNode()->pos().ry());
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

UndirectedEdge::UndirectedEdge(QGraphicsScene *parent, Nodes* Node1 /* = NULL */, Nodes *Node2 /* = NULL */, int weight /* = 1 */)
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
	//qDebug() << "painted edge!";
	painter->setPen(Qt::white);
	if ((getNode1() == NULL) || (getNode2() == NULL)) return;
	painter->drawLine(getNode1()->pos().rx(),getNode1()->pos().ry(),
		getNode2()->pos().rx(), getNode2()->pos().ry());

}



