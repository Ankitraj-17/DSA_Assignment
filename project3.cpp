#include <iostream>
#include <vector>
#include <string>
#include <limits>

using namespace std;

const string RESET   = "\033[0m";
const string BOLD    = "\033[1m";
const string RED     = "\033[1;31m";
const string GREEN   = "\033[1;32m";
const string YELLOW  = "\033[1;33m";
const string BLUE    = "\033[1;34m";
const string MAGENTA = "\033[1;35m";
const string CYAN    = "\033[1;36m";

// Incident Structure representing a disaster report
struct Incident {
    int id;
    string type;      // e.g., Flood, Fire, Earthquake, Landslide
    string location;  // e.g., Sector A, Sector B
    int severity;     // 1 to 10
    string status;    // "Pending", "Dispatched", "Resolved"

    void input() {
        cout << "\nEnter Incident ID: ";
        while (!(cin >> id)) {
            cout << RED << "Invalid ID. Enter a numerical Incident ID: " << RESET;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cout << "Enter Disaster Type (e.g. Fire, Flood): ";
        cin >> type;

        cout << "Enter Location Area (e.g. Sector-A): ";
        cin >> location;

        cout << "Enter Severity Level (1-10): ";
        while (!(cin >> severity) || severity < 1 || severity > 10) {
            cout << RED << "Invalid Level. Enter a level between 1 and 10: " << RESET;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        status = "Pending";
    }

    void display() const {
        cout << "\n-----------------------------------";
        cout << "\nIncident ID    : " << id;
        cout << "\nDisaster Type  : " << type;
        cout << "\nLocation Area  : " << location;
        cout << "\nSeverity Level : " << severity << "/10";
        cout << "\nStatus         : " << status;
        cout << "\n-----------------------------------";
    }
};

// -------------------------------------------------------------
// CUSTOM DATA STRUCTURE: Stack Implemented via Singly Linked List
// -------------------------------------------------------------
template <typename T>
class CustomStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };
    Node* topNode;

public:
    CustomStack() : topNode(nullptr) {}

    ~CustomStack() {
        clear();
    }

    void push(const T& val) {
        Node* newNode = new Node(val);
        newNode->next = topNode;
        topNode = newNode;
    }

    void pop() {
        if (isEmpty()) return;
        Node* temp = topNode;
        topNode = topNode->next;
        delete temp;
    }

    T top() const {
        if (isEmpty()) {
            throw runtime_error("Stack is empty");
        }
        return topNode->data;
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }

    void clear() {
        while (!isEmpty()) {
            pop();
        }
    }
};

// Global disaster reports database
vector<Incident> incidents;

// Graph representing connectivity between incident areas (resizes dynamically)
vector<vector<int>> graph;

void resizeGraph(int newSize) {
    graph.resize(newSize);
    for (int i = 0; i < newSize; i++) {
        graph[i].resize(newSize, 0);
    }
}

// -------------------------------------------------------------
// CUSTOM QUICK SORT (Time Complexity: O(N log N))
// -------------------------------------------------------------
int partitionBySeverity(vector<Incident>& arr, int low, int high) {
    int pivot = arr[high].severity;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        // Sorting in descending order of severity (highest first)
        if (arr[j].severity > pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSortBySeverity(vector<Incident>& arr, int low, int high) {
    if (low < high) {
        int pi = partitionBySeverity(arr, low, high);
        quickSortBySeverity(arr, low, pi - 1);
        quickSortBySeverity(arr, pi + 1, high);
    }
}

// Custom Quick Sort sorting by ID ascending (for Binary Search)
int partitionByID(vector<Incident>& arr, int low, int high) {
    int pivot = arr[high].id;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j].id < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSortByID(vector<Incident>& arr, int low, int high) {
    if (low < high) {
        int pi = partitionByID(arr, low, high);
        quickSortByID(arr, low, pi - 1);
        quickSortByID(arr, pi + 1, high);
    }
}

// -------------------------------------------------------------
// CUSTOM BINARY SEARCH (Time Complexity: O(log N))
// -------------------------------------------------------------
int binarySearchByID(const vector<Incident>& arr, int searchId) {
    int left = 0;
    int right = arr.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid].id == searchId) {
            return mid;
        } else if (arr[mid].id < searchId) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

// -------------------------------------------------------------
// DISPLAY DATABASE RECORDS
// -------------------------------------------------------------
void displayAll() {
    if (incidents.empty()) {
        cout << YELLOW << "\nNo Incident Records Registered Yet." << RESET << "\n";
        return;
    }

    cout << "\n" << CYAN << "=== DISASTER RESPONSE INCIDENTS REGISTER ===" << RESET << "\n";
    for (size_t i = 0; i < incidents.size(); i++) {
        cout << "\nIndex [" << i << "] Details:";
        incidents[i].display();
    }
}

// -------------------------------------------------------------
// GRAPH NETWORK OPERATIONS
// -------------------------------------------------------------
void addRoute() {
    if (incidents.size() < 2) {
        cout << YELLOW << "\nYou need at least two registered incidents to establish a rescue road route." << RESET << "\n";
        return;
    }

    int a, b;
    cout << "\nEnter Incident Index 1 (0 to " << incidents.size() - 1 << "): ";
    while (!(cin >> a) || a < 0 || a >= (int)incidents.size()) {
        cout << RED << "Invalid selection. Enter a valid index within range: " << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Enter Incident Index 2 (0 to " << incidents.size() - 1 << "): ";
    while (!(cin >> b) || b < 0 || b >= (int)incidents.size() || b == a) {
        cout << RED << "Invalid selection. Enter a unique, valid index within range: " << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Establish symmetric undirected connection representing a route/road
    graph[a][b] = 1;
    graph[b][a] = 1;

    cout << GREEN << "\nRescue Road Route Connection Established Successfully!" << RESET << "\n";
}

void showDisasterNetwork() {
    if (incidents.empty()) {
        cout << YELLOW << "\nNo connections exist in an empty disaster network." << RESET << "\n";
        return;
    }

    cout << "\n" << CYAN << "=== DISASTER RESPONSE AREA TRANSIT MAP ===" << RESET << "\n";

    for (size_t i = 0; i < incidents.size(); i++) {
        cout << BOLD << incidents[i].type << " at " << incidents[i].location << RESET 
             << " (Index " << i << ") -> Linked Road to: ";
        
        bool hasConnections = false;
        for (size_t j = 0; j < incidents.size(); j++) {
            if (graph[i][j] == 1) {
                cout << "[" << incidents[j].type << " at " << incidents[j].location << " (Index " << j << ")] ";
                hasConnections = true;
            }
        }
        if (!hasConnections) {
            cout << "None";
        }
        cout << endl;
    }
    cout << CYAN << "==========================================" << RESET << "\n";
}

// -------------------------------------------------------------
// GRAPH TRAVERSAL: Breadth-First Search (BFS)
// -------------------------------------------------------------
void traceRescueAccessCluster() {
    if (incidents.empty()) {
        cout << YELLOW << "\nNo incident data available to perform analysis." << RESET << "\n";
        return;
    }

    int startIdx;
    cout << "\nEnter starting Incident Index (0 to " << incidents.size() - 1 << ") to trace rescue accessibility: ";
    while (!(cin >> startIdx) || startIdx < 0 || startIdx >= (int)incidents.size()) {
        cout << RED << "Invalid selection. Enter a valid starting index: " << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Breadth-First Search queue
    vector<int> q;
    vector<bool> visited(incidents.size(), false);

    q.push_back(startIdx);
    visited[startIdx] = true;

    cout << "\n" << MAGENTA << "=== RESCUE TRANSIT PATH TRACING ===" << RESET << "\n";
    
    int head = 0;
    int visitCount = 0;
    
    while (head < (int)q.size()) {
        int current = q[head++];
        visitCount++;
        
        cout << visitCount << ". " << BOLD << incidents[current].type << RESET 
             << " [ID: " << incidents[current].id 
             << " | Area: " << incidents[current].location 
             << " | Severity: " << incidents[current].severity << "/10]\n";

        for (size_t neighbor = 0; neighbor < incidents.size(); neighbor++) {
            if (graph[current][neighbor] == 1 && !visited[neighbor]) {
                visited[neighbor] = true;
                q.push_back(neighbor);
            }
        }
    }
    
    cout << MAGENTA << "=======================================" << RESET << "\n";
}

// -------------------------------------------------------------
// EXTRACT MOST URGENT DISASTER REPORT
// -------------------------------------------------------------
void mostUrgentDisaster() {
    if (incidents.empty()) {
        cout << YELLOW << "\nNo incident records available to analyze." << RESET << "\n";
        return;
    }

    size_t maxIndex = 0;

    for (size_t i = 1; i < incidents.size(); i++) {
        if (incidents[i].severity > incidents[maxIndex].severity) {
            maxIndex = i;
        }
    }

    cout << "\n" << RED << "=== MOST CRITICAL DISASTER RESPONSE AREA ===" << RESET;
    incidents[maxIndex].display();
}

// -------------------------------------------------------------
// MAIN PROGRAM CONTROLLER
// -------------------------------------------------------------
int main() {
    int choice;
    CustomStack<Incident> undoStack; // Custom Stack for Undo operation

    // Add some pre-populated incident reports for standard database startup
    incidents.push_back({101, "Flood", "Sector-A", 8, "Pending"});
    incidents.push_back({102, "Fire", "Sector-D", 9, "Pending"});
    incidents.push_back({103, "Landslide", "Sector-C", 6, "Pending"});
    incidents.push_back({104, "Storm", "Sector-B", 5, "Pending"});
    resizeGraph(incidents.size());

    // Pre-populate some connection links between sectors
    graph[0][1] = 1; graph[1][0] = 1;
    graph[1][2] = 1; graph[2][1] = 1;
    graph[2][3] = 1; graph[3][2] = 1;

    do {
        cout << "\n" << BLUE << "+-----------------------------------------+" << RESET << "\n";
        cout << BLUE << "|  " << BOLD << CYAN << "SMART DISASTER RESPONSE MANAGEMENT" << BLUE << "     |" << RESET << "\n";
        cout << BLUE << "+-----------------------------------------+" << RESET << "\n";
        cout << BLUE << "| " << RESET << "1. Report New Disaster Incident         " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "| " << RESET << "2. Display Incident Register            " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "| " << RESET << "3. Search Incident Record               " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "| " << RESET << "4. Sort Incidents by Severity           " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "| " << RESET << "5. Establish Link between Sectors       " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "| " << RESET << "6. Display Sector Connection Network    " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "| " << RESET << "7. Trace Connected Regions              " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "| " << RESET << "8. Extract Most Urgent Disaster Area    " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "| " << RESET << "9. Undo Last Reported Incident          " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "| " << RESET << "10. Exit Application                    " << BLUE << "|" << RESET << "\n";
        cout << BLUE << "+-----------------------------------------+" << RESET << "\n";

        cout << "\nEnter Choice [1-10]: ";
        
        if (!(cin >> choice)) {
            cout << RED << "\nInvalid numerical menu entry!" << RESET << "\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0; // Trigger default loop cycle
            continue;
        }

        switch (choice) {
        case 1: {
            Incident inc;
            inc.input();
            
            // Check duplicates
            bool duplicate = false;
            for (const auto& existing : incidents) {
                if (existing.id == inc.id) {
                    duplicate = true;
                    break;
                }
            }

            if (duplicate) {
                cout << RED << "\nError: Disaster incident with ID " << inc.id << " already exists." << RESET << "\n";
            } else {
                incidents.push_back(inc);
                undoStack.push(inc); // push onto Undo stack
                resizeGraph(incidents.size());
                cout << GREEN << "\nDisaster Incident Reported and Pushed Successfully!" << RESET << "\n";
            }
            break;
        }

        case 2:
            displayAll();
            break;

        case 3: {
            if (incidents.empty()) {
                cout << YELLOW << "\nNo Incident records available to search." << RESET << "\n";
                break;
            }

            int searchId;
            cout << "\nEnter Incident ID to search: ";
            while (!(cin >> searchId)) {
                cout << RED << "Invalid ID entry. Enter a numerical ID: " << RESET;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            // Binary search requires sorted collection by ID
            vector<Incident> sortedByID = incidents;
            quickSortByID(sortedByID, 0, sortedByID.size() - 1);

            int index = binarySearchByID(sortedByID, searchId);

            if (index != -1) {
                cout << GREEN << "\nIncident Record Found!" << RESET;
                sortedByID[index].display();
            } else {
                cout << RED << "\nIncident Record Not Found!" << RESET << "\n";
            }
            break;
        }

        case 4:
            if (!incidents.empty()) {
                // Sort a copy of incidents by severity descending to prevent adjacency index corruption
                vector<Incident> tempIncidents = incidents;
                quickSortBySeverity(tempIncidents, 0, tempIncidents.size() - 1);
                
                cout << "\n" << GREEN << "=== INCIDENTS SORTED BY SEVERITY ===" << RESET << "\n";
                for (const auto& item : tempIncidents) {
                    item.display();
                }
            } else {
                cout << YELLOW << "\nNo database records to sort." << RESET << "\n";
            }
            break;

        case 5:
            addRoute();
            break;

        case 6:
            showDisasterNetwork();
            break;

        case 7:
            traceRescueAccessCluster();
            break;

        case 8:
            mostUrgentDisaster();
            break;

        case 9:
            if (undoStack.isEmpty()) {
                cout << YELLOW << "\nNothing to undo. No recent incidents reported in this session." << RESET << "\n";
            } else {
                Incident lastAdded = undoStack.top();
                undoStack.pop();

                bool removed = false;
                for (int i = incidents.size() - 1; i >= 0; i--) {
                    if (incidents[i].id == lastAdded.id) {
                        cout << GREEN << "Successfully removed report: " << RESET 
                             << lastAdded.type << " at " << lastAdded.location << " (ID: " << lastAdded.id << ")" << "\n";
                        incidents.erase(incidents.begin() + i);
                        removed = true;
                        break;
                    }
                }
                if (removed) {
                    resizeGraph(incidents.size());
                } else {
                    cout << RED << "Error: Incident was modified; target not found in active database." << RESET << "\n";
                }
            }
            break;

        case 10:
            cout << "\n" << MAGENTA << "Exiting Smart Disaster Response Management..." << RESET << "\n\n";
            break;

        default:
            cout << RED << "\nInvalid choice selection!" << RESET << "\n";
        }

    } while (choice != 10);

    return 0;
}