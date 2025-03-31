#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Instantiate the ResourceAllocationModel (parented to MainWindow)
    model = new ResourceAllocationModel(this);

    // Create the GraphWidget and set it as the central widget
    graphWidget = new GraphWidget(this);
    setCentralWidget(graphWidget);

    // Connect the nodeClicked signal from GraphWidget to our slot
    connect(graphWidget, &GraphWidget::nodeClicked,
            this, &MainWindow::onNodeClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAddProcess_triggered()
{
    qDebug() << "on_actionAddProcess_triggered() called";
    bool ok;
    QString processName = QInputDialog::getText(this,
                                                tr("Add Process"),
                                                tr("Enter process name:"),
                                                QLineEdit::Normal,
                                                QString(),
                                                &ok).trimmed();
    if (ok && !processName.isEmpty()) {
        // Check for duplicate processes
        if (model->getProcesses().contains(processName)) {
            QMessageBox::warning(this, tr("Duplicate Process"),
                                 tr("Process '%1' already exists.").arg(processName));
            return;
        }
        model->addProcess(processName);
        QMessageBox::information(this, tr("Process Added"),
                                 tr("Process '%1' has been added.").arg(processName));
        graphWidget->addProcessNode(processName);
    }
}

void MainWindow::on_actionAddResource_triggered()
{
    qDebug() << "on_actionAddResource_triggered() called";
    bool ok;
    QString resourceName = QInputDialog::getText(this,
                                                 tr("Add Resource"),
                                                 tr("Enter resource name:"),
                                                 QLineEdit::Normal,
                                                 QString(),
                                                 &ok).trimmed();
    if (ok && !resourceName.isEmpty()) {
        // Check for duplicate resources
        if (model->getResources().contains(resourceName)) {
            QMessageBox::warning(this, tr("Duplicate Resource"),
                                 tr("Resource '%1' already exists.").arg(resourceName));
            return;
        }
        model->addResource(resourceName);
        QMessageBox::information(this, tr("Resource Added"),
                                 tr("Resource '%1' has been added.").arg(resourceName));
        graphWidget->addResourceNode(resourceName);
        qDebug() << "Resource added:" << resourceName;
    } else {
        qDebug() << "Add Resource canceled or empty input.";
    }
}

void MainWindow::on_actionRequestResource_triggered()
{
    bool ok;
    QString processName = QInputDialog::getText(this,
                                                tr("Request Resource"),
                                                tr("Enter process name:"),
                                                QLineEdit::Normal,
                                                QString(),
                                                &ok).trimmed();
    if (!ok || processName.isEmpty())
        return;

    QString resourceName = QInputDialog::getText(this,
                                                 tr("Request Resource"),
                                                 tr("Enter resource name:"),
                                                 QLineEdit::Normal,
                                                 QString(),
                                                 &ok).trimmed();
    if (!ok || resourceName.isEmpty())
        return;

    model->requestResource(processName, resourceName);
    QMessageBox::information(this, tr("Resource Requested"),
                             tr("Process '%1' requested resource '%2'.")
                                 .arg(processName).arg(resourceName));
    graphWidget->addRequestEdge(processName, resourceName);
}

void MainWindow::on_actionAllocateResource_triggered()
{
    bool ok;
    QString processName = QInputDialog::getText(this,
                                                tr("Allocate Resource"),
                                                tr("Enter process name:"),
                                                QLineEdit::Normal,
                                                QString(),
                                                &ok).trimmed();
    if (!ok || processName.isEmpty())
        return;

    QString resourceName = QInputDialog::getText(this,
                                                 tr("Allocate Resource"),
                                                 tr("Enter resource name:"),
                                                 QLineEdit::Normal,
                                                 QString(),
                                                 &ok).trimmed();
    if (!ok || resourceName.isEmpty())
        return;

    model->allocateResource(processName, resourceName);
    QMessageBox::information(this, tr("Resource Allocated"),
                             tr("Resource '%1' allocated to process '%2'.")
                                 .arg(resourceName).arg(processName));
    graphWidget->addAllocationEdge(processName, resourceName);
}

void MainWindow::on_actionDetectDeadlock_triggered()
{
    QSet<QString> deadlockedProcesses = model->detectDeadlockCycle();
    if (!deadlockedProcesses.isEmpty()) {
        QMessageBox::warning(this, tr("Deadlock Detected"),
                             tr("A deadlock has been detected!"));
        // Highlight processes involved in the deadlock cycle
        for (const QString &processName : deadlockedProcesses)
            graphWidget->highlightProcess(processName, Qt::red);
    } else {
        QMessageBox::information(this, tr("Deadlock Detection"),
                                 tr("No deadlock detected."));
        graphWidget->resetProcessColors();
    }
}

void MainWindow::onNodeClicked(const QString &name, bool isProcess)
{
    // Ask user for confirmation before removal
    QString nodeType = isProcess ? tr("process") : tr("resource");
    QMessageBox::StandardButton reply =
        QMessageBox::question(this,
                              tr("Remove %1").arg(nodeType),
                              tr("Do you want to remove the %1 '%2'?").arg(nodeType).arg(name),
                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Remove from the model and graph
        if (isProcess) {
            model->removeProcess(name);
            graphWidget->removeProcessNode(name);
        } else {
            model->removeResource(name);
            graphWidget->removeResourceNode(name);
        }

        QMessageBox::information(this, tr("Removed"),
                                 tr("The %1 '%2' has been removed.").arg(nodeType).arg(name));
    }
}
