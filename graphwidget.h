#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMap>
#include <QString>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QMouseEvent>
#include <QColor>  // Needed for QColor

/**
 * @brief The GraphWidget class visualizes processes and resources as nodes and edges.
 */
class GraphWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphWidget(QWidget *parent = nullptr);
    ~GraphWidget();

    // Methods to add nodes and edges
    void addProcessNode(const QString &processName);
    void addResourceNode(const QString &resourceName);
    void addRequestEdge(const QString &processName, const QString &resourceName);
    void addAllocationEdge(const QString &processName, const QString &resourceName);

    // Methods to remove nodes
    void removeProcessNode(const QString &processName);
    void removeResourceNode(const QString &resourceName);

    // Highlight a process node with a given color
    void highlightProcess(const QString &processName, const QColor &color);

    // Reset all process nodes to the default color
    void resetProcessColors();

signals:
    /**
     * @brief Emitted when the user clicks on a node (process or resource).
     * @param name The name of the clicked node.
     * @param isProcess True if it's a process node, false if it's a resource node.
     */
    void nodeClicked(const QString &name, bool isProcess);

protected:
    /**
     * @brief Overridden to detect clicks on nodes and emit nodeClicked signal.
     */
    void mousePressEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *m_scene;

    // Store nodes using type-specific pointers to avoid casting
    QMap<QString, QGraphicsEllipseItem*> processNodes;
    QMap<QString, QGraphicsRectItem*> resourceNodes;

    // Helper functions for positioning nodes
    QPointF getNextProcessPosition();
    QPointF getNextResourcePosition();

    int processCount = 0;
    int resourceCount = 0;

    // Helper to remove all edges connected to a given item
    void removeEdgesConnectedTo(QGraphicsItem *item);
};

#endif // GRAPHWIDGET_H
