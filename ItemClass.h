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
#include <QObject>
#include "SceneClass.h"

class DirectedEdge;
class UndirectedEdge;

class Nodes :public QObject, public QGraphicsItem
{
	Q_OBJECT

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
	QString m_type;
	QColor inputColor;
	QLabel inputLabel;
	QColor viewColor;
	QString colorStrategy;
	double m_radius;
	double m_x;
	double m_y;
	double m_z;
	QList<DirectedEdge *> DirectedEdgeList;
	QList<UndirectedEdge*> UndirectedEdgeLIst;

	QPointF newPos;
	QPointF oldPos;
	QPointF strategyPos[10];
	QLabel *hoverInfo;
	vtkIdType idType;

signals:
	void sendInfomation(QString);
	void sendCurrentPressesNode(Nodes *);

public:
	virtual QRectF boundingRect()const;
	void setViewColor(QColor c){viewColor = c;}
	void setColorStrategy(QString s){colorStrategy = s;update();}
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	QPainterPath shape() const;
	bool advance();
	void calculateForces();

	void addInformation(QString);
	void inputViewColor(QColor);
	void inputViewLabel(QString);
	void inputDefaultPosition(double, double, double);
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
	void editInformation();
	//QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	PCAGraph * getScene(){return m_scene;}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
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
	//void editInformation();
	PCAGraph * getScene(){return m_scene;}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
	//QVariant itemChange(GraphicsItemChange change, const QVariant &value);
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
	//void editInformation();
	PCAGraph * getScene(){return m_scene;}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
	//QVariant itemChange(GraphicsItemChange change, const QVariant &value);
};

class TopicNode :public QObject, public Nodes
{
	Q_OBJECT

public:
	TopicNode(TopicGraph *parent = 0, int num = 0);

private:
	QStringList topicWords;
	QStringList topicDocuments;
	TopicGraph *m_scene;

signals:
	void setDocComboBox(TopicNode* node);

public:
	void inputTopicWords(QStringList input) {topicWords = input;}
	void inputTopicDocuments(QStringList input) {topicDocuments = input;}
	QStringList getTopicWords(){return topicWords;}
	QStringList getTopicDocuments(){return topicDocuments;}
	TopicGraph *getScene(){return m_scene;}
	//QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
};



class Edges:public QObject, public QGraphicsItem
{
	Q_OBJECT

public:
	enum { Type = UserType + 2 };

	int type() const
	{
		// Enable the use of qgraphicsitem_cast with this item.
		return Type;
	}

public:
	Edges();

public:
	QGraphicsScene *m_scene;
	QColor viewColor;
	QString colorStrategy;

public:
	QRectF boundingRect()const;
	void setViewColor(QColor c){viewColor = c;}
	void setColorStrategy(QString s){colorStrategy = s;update();}
};


class DirectedEdge :public Edges
{
public:
	DirectedEdge(QGraphicsScene *parent, Nodes* source = NULL, Nodes *target = NULL, double weight = 1);

public:
	Nodes* sourceNode;
	Nodes* targetNode;
	double m_weight;

public:
	Nodes* getSourceNode() {return sourceNode;}
	Nodes* getTargetNode() {return targetNode;}
	void adjust();
	QPainterPath shape() const;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget  = 0 );
};


class UndirectedEdge :public Edges
{
public:
	UndirectedEdge(QGraphicsScene *parent, Nodes* Node1 = NULL, Nodes* Node2 = NULL, double weight = 0.5);

private:
	Nodes* m_node1;
	Nodes* m_node2;
	double m_weight;

public:
	Nodes* getNode1() {return m_node1;}
	Nodes* getNode2() {return m_node2;}
	int getWeight() {return m_weight;}
	void adjust();
	QPainterPath shape() const;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget  = 0 );
};

#endif