#ifndef ITEM_CLASS_H
#define ITEM_CLASS_H

#include <vtk-6.2/vtkSmartPointer.h>
#include <vtk-6.2/vtkMutableUndirectedGraph.h>
#include <vtk-6.2/vtkIntArray.h>
#include <QGraphicsItem>
#include <QString>
#include <QColor>
#include <QLabel>
#include <QPointF>
#include <QList>
#include "SceneClass.h"

class DirectedEdge;
class UndirectedEdge;

class Nodes :public QGraphicsItem
{
public:
	Nodes();
	enum { Type = UserType + 1 };

	int type() const
	{
		// Enable the use of qgraphicsitem_cast with this item.
		return Type;
	}
public:
	int m_num;
	QString m_information;

	QColor viewColor;
	QLabel viewLabel;
	double m_x;
	double m_y;
	double m_z;
	QList<DirectedEdge *> DirectedEdgeList;
	QList<UndirectedEdge*> UndirectedEdgeLIst;

	QPointF newPos;
	vtkIdType idType;

public:
	virtual QRectF boundingRect()const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	QPainterPath shape() const;
	bool advance();
	void calculateForces();

	void addInformation(QString);
	void setViewColor(QColor);
	void setViewLabel(QString);
	void inputDefaultPosition(double, double, double);
	void setZoom(double scaleFctor, QPointF pos);
};

class PaperNode :public Nodes
{
public:
	PaperNode(PCAGraph *parent, int num = 0);
private:
	int m_year;
	QString m_authors;
	int m_dateFrom;
	QString m_id;
	int m_pageFrom;
	QString m_paperTitle;
	QString m_paperTitleShort;
	PCAGraph *m_scene;

public:
	void setYear(int);
	void setAuthors(QString);
	void setDateFrom(int);
	void setId(QString);
	void setPageFrom(int);
	void setPaperTitle(QString);
	void setPaperTitleShort(QString);
	PCAGraph * getScene(){return m_scene;}
};

class ConferenceNode :public Nodes
{
public:
	ConferenceNode(PCAGraph *parent, int num = 0);
private:
	int m_year;
	QString m_id;
	QString m_conferenceName;
	QString m_conferenceNameShort;
	PCAGraph *m_scene;

public:
	PCAGraph * getScene(){return m_scene;}
};

class AuthorNode :public Nodes
{
public:
	AuthorNode(PCAGraph *parent, int num = 0);
private:
	int m_year;
	QString m_id;
	QString m_authorName;
	QString m_authorNameShort;
	PCAGraph *m_scene;
public:
	PCAGraph * getScene(){return m_scene;}

};

class TopicNode :public Nodes
{
public:
	TopicNode(TopicGraph *parent = 0, int num = 0);

private:
	QStringList topicWords;
	QStringList topicDocuments;
	TopicGraph *m_scene;
public:
	void inputTopicWords(QStringList input) {topicWords = input;}
	void inputTopicDocuments(QStringList input) {topicDocuments = input;}
	QStringList getTopicWords(){return topicWords;}
	QStringList getTopicDocuments(){return topicDocuments;}
	TopicGraph *getScene(){return m_scene;}
};



class Edges:public QGraphicsItem
{
public:
	enum { Type = UserType + 2 };

	int type() const
	{
		// Enable the use of qgraphicsitem_cast with this item.
		return Type;
	}
public:
	QGraphicsScene *m_scene;
public:
	QRectF boundingRect()const;
};


class DirectedEdge :public Edges
{
public:
	DirectedEdge(QGraphicsScene *parent, Nodes* source = NULL, Nodes *target = NULL, int weight = 10);

public:
	Nodes* sourceNode;
	Nodes* targetNode;
	int m_weight;

public:
	Nodes* getSourceNode() {return sourceNode;}
	Nodes* getTargetNode() {return targetNode;}
	void adjust();

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget  = 0 );
};


class UndirectedEdge :public Edges
{
public:
	UndirectedEdge(QGraphicsScene *parent, Nodes* Node1 = NULL, Nodes* Node2 = NULL, int weight = 10);

private:
	Nodes* m_node1;
	Nodes* m_node2;
	int m_weight;

public:
	Nodes* getNode1() {return m_node1;}
	Nodes* getNode2() {return m_node2;}
	int getWeight() {return m_weight;}
	void adjust();

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget  = 0 );
};

#endif