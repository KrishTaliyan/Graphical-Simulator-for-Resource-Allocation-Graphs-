#include "ResourceAllocationModel.h"

ResourceAllocationModel::ResourceAllocationModel(QObject *parent)
    : QObject(parent)
{
}

void ResourceAllocationModel::addProcess(const QString &processName)
{
    processes.insert(processName);
}

void ResourceAllocationModel::addResource(const QString &resourceName)
{
    resources.insert(resourceName);
}

void ResourceAllocationModel::requestResource(const QString &processName, const QString &resourceName)
{
    requests[processName].insert(resourceName);
}

void ResourceAllocationModel::allocateResource(const QString &processName, const QString &resourceName)
{
    allocations[processName].insert(resourceName);
    if (requests.contains(processName))
        requests[processName].remove(resourceName);
}

void ResourceAllocationModel::removeProcess(const QString &processName)
{
    if (!processes.contains(processName))
        return;

    // Remove from the set of processes
    processes.remove(processName);

    // Remove any requests or allocations associated with this process
    requests.remove(processName);
    allocations.remove(processName);
}

void ResourceAllocationModel::removeResource(const QString &resourceName)
{
    if (!resources.contains(resourceName))
        return;

    // Remove from the set of resources
    resources.remove(resourceName);

    // Remove this resource from all requests
    for (auto it = requests.begin(); it != requests.end(); ++it) {
        it.value().remove(resourceName);
    }

    // Remove this resource from all allocations
    for (auto it = allocations.begin(); it != allocations.end(); ++it) {
        it.value().remove(resourceName);
    }
}

bool ResourceAllocationModel::dfs(const QString &process,
                                  QSet<QString> &visited,
                                  QSet<QString> &recStack,
                                  QSet<QString> &cycle) const
{
    if (recStack.contains(process)) {
        cycle = recStack; // Capture the current recursion stack as the cycle.
        return true;
    }
    if (visited.contains(process))
        return false;

    visited.insert(process);
    recStack.insert(process);

    // For each resource requested by this process...
    QSet<QString> reqs = requests.value(process);
    for (const QString &res : reqs) {
        // For every process that holds this resource...
        for (auto it = allocations.begin(); it != allocations.end(); ++it) {
            if (it.value().contains(res)) {
                if (dfs(it.key(), visited, recStack, cycle))
                    return true;
            }
        }
    }
    recStack.remove(process);
    return false;
}

QSet<QString> ResourceAllocationModel::detectDeadlockCycle() const
{
    QSet<QString> visited;
    QSet<QString> recStack;
    QSet<QString> cycle;
    for (const QString &process : processes) {
        if (dfs(process, visited, recStack, cycle))
            return cycle;  // Found a cycle; return the involved processes.
    }
    return QSet<QString>();  // No deadlock detected.
}
