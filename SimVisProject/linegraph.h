#ifndef LINEGRAPH_H
#define LINEGRAPH_H

#include "graph.h"

class LineGraphDataSource : public QObject {
    Q_OBJECT
public:
    QVector<QPointF> m_points;
    Q_INVOKABLE void addPoint(float x, float y);
    QPointF operator[](int index) { return m_points[index]; }
    QPointF get(int index) { return m_points[index]; }
    int size();
    void setPoints(const QVector<QPointF> &points);

public slots:
    void reset();
signals:
    void dataChanged();
};

class LineGraph : public Graph
{
    Q_OBJECT
    Q_PROPERTY(LineGraphDataSource* dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(Qt::PenStyle style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
private:
    LineGraphDataSource m_defaultDataSource;
    LineGraphDataSource* m_dataSource = nullptr;
    bool isInValueRange(const QPointF &point, const float &xMin, const float &xMax, const float &yMin, const float &yMax);
    QColor m_color = "black";
    Qt::PenStyle m_style = Qt::SolidLine;
    int m_width = 1;

public:
    LineGraph();
    ~LineGraph();
    virtual void paint(Figure *figure, QPainter *painter);
    LineGraphDataSource* dataSource() const;
    QColor color() const;
    Qt::PenStyle style() const;
    int width() const;

public slots:
    void addPoint(float x, float y);
    void setDataSource(LineGraphDataSource* dataSource);
    void setColor(QColor color);
    void setStyle(Qt::PenStyle style);
    void setWidth(int width);

signals:
    void dataSourceChanged(LineGraphDataSource* dataSource);
    void colorChanged(QColor color);
    void styleChanged(Qt::PenStyle style);
    void widthChanged(int width);
};

#endif // LINEGRAPH_H
