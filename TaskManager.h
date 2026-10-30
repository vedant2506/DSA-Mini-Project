#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <cstddef>
#include <string>
#include <vector>
#include <unordered_map>

struct Task {
    int id;
    std::string title;
    std::string subject;
    std::string deadline; // YYYY-MM-DD, so sorting is reliable
    int priority;         // 1 = High, 2 = Medium, 3 = Low
    bool completed;
};

class TaskManager {
public:
    TaskManager();

    void loadFromFile();
    void saveToFile() const;

    void addTask();
    void viewAllTasks() const;
    void viewUrgentTasks() const;
    void markTaskCompleted();
    void deleteTask();

private:
    std::vector<Task> tasks;
    int nextId;
    std::unordered_map<int, std::size_t> taskIndex; // Hash table: task ID -> position in tasks

    static int priorityRank(int priority);
    static std::string priorityName(int priority);
    static bool isValidDate(const std::string& date);
    static bool isValidDateShape(const std::string& date);

    void printTaskTable(const std::vector<Task>& list) const;
    void rebuildTaskIndex();
};

#endif
