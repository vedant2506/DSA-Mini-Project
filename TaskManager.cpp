#include "TaskManager.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

namespace {
const char* DATA_FILE = "tasks.txt";

std::string trim(const std::string& value) {
    std::size_t first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    std::size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

bool parseInt(const std::string& text, int& result) {
    std::istringstream input(text);
    input >> result;
    return input && input.peek() == std::char_traits<char>::eof();
}

std::vector<std::string> splitLine(const std::string& line, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(line);
    std::string part;
    while (std::getline(stream, part, delimiter)) parts.push_back(part);
    return parts;
}

std::string safeField(std::string value) {
    // Keep the simple text-file format safe and predictable.
    for (char& ch : value) {
        if (ch == '|') ch = '/';
        if (ch == '\n' || ch == '\r') ch = ' ';
    }
    return value;
}

// A real Binary Search Tree node. Tasks are ordered by deadline, then by ID.
struct DeadlineNode {
    Task task;
    DeadlineNode* left;
    DeadlineNode* right;
    explicit DeadlineNode(const Task& value)
        : task(value), left(nullptr), right(nullptr) {}
};

bool comesBefore(const Task& a, const Task& b) {
    if (a.deadline != b.deadline) return a.deadline < b.deadline;
    return a.id < b.id;
}

DeadlineNode* insertDeadline(DeadlineNode* root, const Task& task) {
    if (root == nullptr) return new DeadlineNode(task);
    if (comesBefore(task, root->task))
        root->left = insertDeadline(root->left, task);
    else
        root->right = insertDeadline(root->right, task);
    return root;
}

void collectInOrder(DeadlineNode* root, std::vector<Task>& ordered) {
    if (root == nullptr) return;
    collectInOrder(root->left, ordered);
    ordered.push_back(root->task);
    collectInOrder(root->right, ordered);
}

void deleteDeadlineTree(DeadlineNode* root) {
    if (root == nullptr) return;
    deleteDeadlineTree(root->left);
    deleteDeadlineTree(root->right);
    delete root;
}
}

TaskManager::TaskManager() : nextId(1) {}

void TaskManager::rebuildTaskIndex() {
    taskIndex.clear();
    for (std::size_t i = 0; i < tasks.size(); ++i)
        taskIndex[tasks[i].id] = i;
}

int TaskManager::priorityRank(int priority) {
    return priority;
}

std::string TaskManager::priorityName(int priority) {
    if (priority == 1) return "HIGH";
    if (priority == 2) return "MEDIUM";
    return "LOW";
}

bool TaskManager::isValidDateShape(const std::string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
    for (std::size_t i = 0; i < date.size(); ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(static_cast<unsigned char>(date[i]))) return false;
    }
    return true;
}

bool TaskManager::isValidDate(const std::string& date) {
    if (!isValidDateShape(date)) return false;

    int year = 0, month = 0, day = 0;
    if (!parseInt(date.substr(0, 4), year) ||
        !parseInt(date.substr(5, 2), month) ||
        !parseInt(date.substr(8, 2), day)) return false;

    if (year < 1900 || month < 1 || month > 12 || day < 1) return false;

    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month - 1];
    bool leapYear = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    if (month == 2 && leapYear) maxDay = 29;
    return day <= maxDay;
}

void TaskManager::loadFromFile() {
    tasks.clear();
    nextId = 1;

    std::ifstream input(DATA_FILE);
    if (!input) return; // First run: file may not exist yet.

    std::string line;
    while (std::getline(input, line)) {
        std::vector<std::string> fields = splitLine(line, '|');
        if (fields.size() != 6) continue;

        Task task;
        int id = 0, priority = 0, completed = 0;
        if (!parseInt(fields[0], id) ||
            !parseInt(fields[4], priority) ||
            !parseInt(fields[5], completed)) continue;
        if (id < 1 || priority < 1 || priority > 3) continue;
        if (!isValidDate(fields[3])) continue;

        task.id = id;
        task.title = fields[1];
        task.subject = fields[2];
        task.deadline = fields[3];
        task.priority = priority;
        task.completed = (completed != 0);
        tasks.push_back(task);
        if (id >= nextId) nextId = id + 1;
    }
    rebuildTaskIndex();
}

void TaskManager::saveToFile() const {
    std::ofstream output(DATA_FILE);
    if (!output) {
        std::cout << "\n[ERROR] Could not save tasks.txt. Check folder permissions.\n";
        return;
    }

    for (const Task& task : tasks) {
        output << task.id << '|'
               << safeField(task.title) << '|'
               << safeField(task.subject) << '|'
               << task.deadline << '|'
               << task.priority << '|'
               << (task.completed ? 1 : 0) << '\n';
    }
}

void TaskManager::addTask() {
    Task task;
    task.id = nextId;

    std::cout << "\n--- Add New Task ---\n";
    std::cout << "Task title: ";
    std::getline(std::cin >> std::ws, task.title);
    task.title = trim(task.title);
    if (task.title.empty()) {
        std::cout << "[ERROR] Task title cannot be empty.\n";
        return;
    }

    std::cout << "Subject: ";
    std::getline(std::cin, task.subject);
    task.subject = trim(task.subject);
    if (task.subject.empty()) {
        std::cout << "[ERROR] Subject cannot be empty.\n";
        return;
    }

    std::cout << "Deadline (YYYY-MM-DD, e.g. 2026-10-19): ";
    std::getline(std::cin, task.deadline);
    task.deadline = trim(task.deadline);
    if (!isValidDate(task.deadline)) {
        std::cout << "[ERROR] Invalid date. Use YYYY-MM-DD and a real calendar date.\n";
        return;
    }

    std::string priorityInput;
    int priority = 0;
    std::cout << "Priority (1=High, 2=Medium, 3=Low): ";
    std::getline(std::cin, priorityInput);
    if (!parseInt(trim(priorityInput), priority) || priority < 1 || priority > 3) {
        std::cout << "[ERROR] Priority must be 1, 2, or 3.\n";
        return;
    }

    task.priority = priority;
    task.completed = false;
    tasks.push_back(task);
    ++nextId;
    rebuildTaskIndex();
    saveToFile();
    std::cout << "\n[SUCCESS] Task added and saved.\n";
}

void TaskManager::printTaskTable(const std::vector<Task>& list) const {
    if (list.empty()) {
        std::cout << "\nNo tasks to display.\n";
        return;
    }

    std::cout << "\n"
              << std::left
              << std::setw(5) << "ID"
              << std::setw(27) << "TASK"
              << std::setw(16) << "SUBJECT"
              << std::setw(13) << "DEADLINE"
              << std::setw(10) << "PRIORITY"
              << "STATUS\n";
    std::cout << std::string(83, '-') << '\n';

    for (const Task& task : list) {
        std::string title = task.title;
        std::string subject = task.subject;
        if (title.size() > 25) title = title.substr(0, 22) + "...";
        if (subject.size() > 14) subject = subject.substr(0, 11) + "...";

        std::cout << std::left
                  << std::setw(5) << task.id
                  << std::setw(27) << title
                  << std::setw(16) << subject
                  << std::setw(13) << task.deadline
                  << std::setw(10) << priorityName(task.priority)
                  << (task.completed ? "COMPLETED" : "PENDING") << '\n';
    }

    int completedCount = 0;
    for (const Task& task : list) {
        if (task.completed) ++completedCount;
    }
    std::cout << "\nTasks shown: " << list.size()
              << " | Completed: " << completedCount
              << " | Pending: " << (list.size() - completedCount) << '\n';
}

void TaskManager::viewAllTasks() const {
    // Build a BST by deadline, then use in-order traversal for chronological order.
    DeadlineNode* root = nullptr;
    for (const Task& task : tasks)
        root = insertDeadline(root, task);

    std::vector<Task> sortedTasks;
    collectInOrder(root, sortedTasks);
    deleteDeadlineTree(root);

    std::cout << "\n--- All Tasks (BST: sorted by deadline) ---\n";
    printTaskTable(sortedTasks);
}

void TaskManager::viewUrgentTasks() const {
    std::vector<Task> urgent;
    for (const Task& task : tasks) {
        if (!task.completed) urgent.push_back(task);
    }

    std::sort(urgent.begin(), urgent.end(),
        [](const Task& a, const Task& b) {
            if (a.priority != b.priority) return priorityRank(a.priority) < priorityRank(b.priority);
            if (a.deadline != b.deadline) return a.deadline < b.deadline;
            return a.id < b.id;
        });

    std::cout << "\n--- Urgent Tasks (priority first, then deadline) ---\n";
    printTaskTable(urgent);
}

void TaskManager::markTaskCompleted() {
    if (tasks.empty()) {
        std::cout << "\nNo tasks available.\n";
        return;
    }

    std::string input;
    int id = 0;
    std::cout << "\nEnter task ID to mark completed: ";
    std::getline(std::cin, input);
    if (!parseInt(trim(input), id)) {
        std::cout << "[ERROR] Enter a valid numeric task ID.\n";
        return;
    }

    std::unordered_map<int, std::size_t>::iterator found = taskIndex.find(id);
    if (found == taskIndex.end()) {
        std::cout << "[ERROR] No task found with that ID.\n";
        return;
    }

    Task& task = tasks[found->second];
    if (task.completed) {
        std::cout << "This task is already completed.\n";
        return;
    }

    task.completed = true;
    saveToFile();
    std::cout << "[SUCCESS] Task marked as completed.\n";
}

void TaskManager::deleteTask() {
    if (tasks.empty()) {
        std::cout << "\nNo tasks available.\n";
        return;
    }

    std::string input;
    int id = 0;
    std::cout << "\nEnter task ID to delete: ";
    std::getline(std::cin, input);
    if (!parseInt(trim(input), id)) {
        std::cout << "[ERROR] Enter a valid numeric task ID.\n";
        return;
    }

    std::unordered_map<int, std::size_t>::iterator found = taskIndex.find(id);
    if (found == taskIndex.end()) {
        std::cout << "[ERROR] No task found with that ID.\n";
        return;
    }

    tasks.erase(tasks.begin() + static_cast<std::ptrdiff_t>(found->second));
    rebuildTaskIndex();
    saveToFile();
    std::cout << "[SUCCESS] Task deleted.\n";
}
