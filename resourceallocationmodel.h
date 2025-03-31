#ifndef RESOURCEALLOCATIONMODEL_H
#define RESOURCEALLOCATIONMODEL_H

#include <QObject>
#include <QString>
#include <QSet>
#include <QMap>

/**
 * @brief The ResourceAllocationModel class
 * Manages processes, resources, requests, and allocations.
 * Provides deadlock detection via DFS.
 */
class ResourceAllocationModel : public QObject
{
    Q_OBJECT
public:
    explicit ResourceAllocationModel(QObject *parent = nullptr);

    void addProcess(const QString &processName);
    void addResource(const QString &resourceName);
    void requestResource(const QString &processName, const QString &resourceName);
    void allocateResource(const QString &processName, const QString &resourceName);

    /**
     * @brief Remove a process from the model.
     * @param processName The name of the process to remove.
     */
    void removeProcess(const QString &processName);

    /**
     * @brief Remove a resource from the model.
     * @param resourceName The name of the resource to remove.
     */
    void removeResource(const QString &resourceName);

    /**
     * @brief Detects a deadlock cycle.
     * @return A set of process names involved in a deadlock cycle; empty if none.
     */
    QSet<QString> detectDeadlockCycle() const;

    // Accessors
    QSet<QString> getProcesses() const { return processes; }
    QSet<QString> getResources() const { return resources; }

private:
    QSet<QString> processes;
    QSet<QString> resources;
    QMap<QString, QSet<QString>> requests;
    QMap<QString, QSet<QString>> allocations;

    /**
     * @brief Helper function to perform DFS for deadlock detection.
     * @param process The current process.
     * @param visited Set of visited processes.
     * @param recStack Current recursion stack.
     * @param cycle Set capturing the processes forming the cycle.
     * @return true if a cycle is found, false otherwise.
     */
    bool dfs(const QString &process, QSet<QString> &visited, QSet<QString> &recStack, QSet<QString> &cycle) const;
};

#endif // RESOURCEALLOCATIONMODEL_H
