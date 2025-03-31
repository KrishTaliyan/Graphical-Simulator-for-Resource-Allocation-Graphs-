#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ResourceAllocationModel.h"
#include "GraphWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief The MainWindow class handles user interaction and updates both the model and graph.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAddProcess_triggered();
    void on_actionAddResource_triggered();
    void on_actionRequestResource_triggered();
    void on_actionAllocateResource_triggered();
    void on_actionDetectDeadlock_triggered();

    /**
     * @brief Slot called when a node in the graph is clicked.
     * @param name The name of the clicked node.
     * @param isProcess True if it's a process node, false if it's a resource.
     */
    void onNodeClicked(const QString &name, bool isProcess);

private:
    Ui::MainWindow *ui;
    ResourceAllocationModel *model;
    GraphWidget *graphWidget;
};

#endif // MAINWINDOW_H
