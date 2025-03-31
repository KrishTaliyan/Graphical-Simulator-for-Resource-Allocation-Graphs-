#include "GraphWidget.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>
#include <QFontMetrics>
#include <QFont>
#include <QLinearGradient>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent)
{
    // Create the scene and set it on the view
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);

    // Dark gradient background
    QLinearGradient gradient(0, 0, 0, 600);
    gradient.setColorAt(0, QColor("#3b3b3b"));  // Dark gray top
    gradient.setColorAt(1, QColor("#1e1e1e"));  // Even darker gray bottom
    m_scene->setBackgroundBrush(gradient);

    // Enable antialiasing for smoother edges
    setRenderHint(QPainter::Antialiasing, true);

    // Set a default scene rect (adjust as needed)
    m_scene->setSceneRect(0, 0, 800, 600);
}

GraphWidget::~GraphWidget()
{
    // The scene and its items are managed by Qt's parent-child system.
}

void GraphWidget::addProcessNode(const QString &processName)
{
    // Create an ellipse for the process node
    QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(0, 0, 60, 60);
    ellipse->setBrush(Qt::cyan);  // Default color for processes

    // Optional: Add a drop shadow effect
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(8.0);
    shadowEffect->setOffset(3.0, 3.0);
    ellipse->setGraphicsEffect(shadowEffect);

    // Create a text item as a child of the ellipse
    QGraphicsTextItem *text = new QGraphicsTextItem(processName, ellipse);
    QFont font = text->font();
    font.setPointSize(10);
    font.setBold(true);
    text->setFont(font);
    text->setDefaultTextColor(Qt::white); // White text on dark background

    // Center the text within the ellipse
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(processName);
    qreal xOffset = (ellipse->rect().width() - textRect.width()) / 2;
    qreal yOffset = (ellipse->rect().height() - textRect.height()) / 2;
    text->setPos(xOffset, yOffset);

    // Position the ellipse in the scene
    QPointF pos = getNextProcessPosition();
    ellipse->setPos(pos);
    m_scene->addItem(ellipse);

    // Store it in the map
    processNodes[processName] = ellipse;
    processCount++;
}

void GraphWidget::addResourceNode(const QString &resourceName)
{
    // Create a rectangle for the resource node
    QGraphicsRectItem *rect = new QGraphicsRectItem(0, 0, 60, 60);
    rect->setBrush(Qt::yellow);  // Default color for resources

    // Optional: Add a drop shadow effect
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(8.0);
    shadowEffect->setOffset(3.0, 3.0);
    rect->setGraphicsEffect(shadowEffect);

    // Create a text item as a child of the rectangle
    QGraphicsTextItem *text = new QGraphicsTextItem(resourceName, rect);
    QFont font = text->font();
    font.setPointSize(10);
    font.setBold(true);
    text->setFont(font);
    text->setDefaultTextColor(Qt::black); // Black text on yellow background

    // Center the text within the rectangle
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(resourceName);
    qreal xOffset = (rect->rect().width() - textRect.width()) / 2;
    qreal yOffset = (rect->rect().height() - textRect.height()) / 2;
    text->setPos(xOffset, yOffset);

    // Position the rectangle in the scene
    QPointF pos = getNextResourcePosition();
    rect->setPos(pos);
    m_scene->addItem(rect);

    // Store it in the map
    resourceNodes[resourceName] = rect;
    resourceCount++;
}

void GraphWidget::addRequestEdge(const QString &processName, const QString &resourceName)
{
    if (!processNodes.contains(processName) || !resourceNodes.contains(resourceName))
        return;

    QGraphicsEllipseItem *pItem = processNodes.value(processName);
    QGraphicsRectItem *rItem = resourceNodes.value(resourceName);

    QPointF pCenter = pItem->sceneBoundingRect().center();
    QPointF rCenter = rItem->sceneBoundingRect().center();

    // Draw a dashed line from process to resource
    QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(pCenter, rCenter));
    line->setPen(QPen(Qt::blue, 2, Qt::DashLine));
    m_scene->addItem(line);
}

void GraphWidget::addAllocationEdge(const QString &processName, const QString &resourceName)
{
    if (!processNodes.contains(processName) || !resourceNodes.contains(resourceName))
        return;

    QGraphicsEllipseItem *pItem = processNodes.value(processName);
    QGraphicsRectItem *rItem = resourceNodes.value(resourceName);

    QPointF pCenter = pItem->sceneBoundingRect().center();
    QPointF rCenter = rItem->sceneBoundingRect().center();

    // Draw a solid line from resource to process
    QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(rCenter, pCenter));
    line->setPen(QPen(Qt::green, 2, Qt::SolidLine));
    m_scene->addItem(line);
}

void GraphWidget::highlightProcess(const QString &processName, const QColor &color)
{
    if (!processNodes.contains(processName))
        return;

    QGraphicsEllipseItem *ellipse = processNodes.value(processName);
    if (ellipse)
        ellipse->setBrush(color);
}

void GraphWidget::resetProcessColors()
{
    for (auto ellipse : processNodes) {
        if (ellipse)
            ellipse->setBrush(Qt::cyan);
    }
}

void GraphWidget::removeProcessNode(const QString &processName)
{
    if (!processNodes.contains(processName))
        return;

    QGraphicsEllipseItem *ellipse = processNodes.value(processName);
    if (!ellipse)
        return;

    // Remove all edges connected to this ellipse
    removeEdgesConnectedTo(ellipse);

    // Remove from scene and map
    m_scene->removeItem(ellipse);
    processNodes.remove(processName);
    delete ellipse;  // Free memory
}

void GraphWidget::removeResourceNode(const QString &resourceName)
{
    if (!resourceNodes.contains(resourceName))
        return;

    QGraphicsRectItem *rect = resourceNodes.value(resourceName);
    if (!rect)
        return;

    // Remove all edges connected to this rect
    removeEdgesConnectedTo(rect);

    // Remove from scene and map
    m_scene->removeItem(rect);
    resourceNodes.remove(resourceName);
    delete rect;  // Free memory
}

// Helper to remove edges connected to a given item
void GraphWidget::removeEdgesConnectedTo(QGraphicsItem *item)
{
    // Remove any QGraphicsLineItem that touches the center of 'item'.
    QPointF center = item->sceneBoundingRect().center();

    // Collect items to remove to avoid modifying the scene during iteration.
    QList<QGraphicsItem*> itemsToRemove;

    for (QGraphicsItem *other : m_scene->items()) {
        QGraphicsLineItem *line = qgraphicsitem_cast<QGraphicsLineItem*>(other);
        if (!line)
            continue;

        QPointF p1 = line->line().p1();
        QPointF p2 = line->line().p2();

        // If either endpoint is close to 'center', mark for removal
        if ((p1 - center).manhattanLength() < 1.0 ||
            (p2 - center).manhattanLength() < 1.0) {
            itemsToRemove << line;
        }
    }

    // Now remove the collected items from the scene.
    for (QGraphicsItem *toRemove : itemsToRemove) {
        m_scene->removeItem(toRemove);
        delete toRemove;
    }
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    // Convert mouse coordinates to scene coordinates
    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem *item = m_scene->itemAt(scenePos, QTransform());
    if (item) {
        // Determine whether the clicked item is a process or resource
        for (auto it = processNodes.constBegin(); it != processNodes.constEnd(); ++it) {
            if (it.value() == item) {
                emit nodeClicked(it.key(), true);
                break;
            }
        }
        for (auto it = resourceNodes.constBegin(); it != resourceNodes.constEnd(); ++it) {
            if (it.value() == item) {
                emit nodeClicked(it.key(), false);
                break;
            }
        }
    }

    // Call the base class implementation for standard behavior
    QGraphicsView::mousePressEvent(event);
}

QPointF GraphWidget::getNextProcessPosition()
{
    // Example logic: place processes on the left, spaced vertically.
    return QPointF(100, 80 + processCount * 100);
}

QPointF GraphWidget::getNextResourcePosition()
{
    // Example logic: place resources on the right, spaced vertically.
    return QPointF(500, 80 + resourceCount * 100);
}
