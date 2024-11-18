#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

using namespace std;

// Patient class
class Patient {
public:
    string name;
    int age;
    string sex;
    time_t arrival_time;
    int esi_priority;

    // Constructor
    Patient(string name, int age, string sex)
        : name(name), age(age), sex(sex), arrival_time(time(nullptr)), esi_priority(-1) {}

    // Method to calculate ESI priority
    void calculate_esi_priority() {
        int life_saving_needed, high_risk, severe_pain, resources_required;

        cout << "Does the patient require immediate life-saving intervention? (1 = Yes, 0 = No): ";
        cin >> life_saving_needed;

        if (life_saving_needed) {
            esi_priority = 1;
            return;
        }

        cout << "Is the patient high-risk, confused, lethargic, or severely distressed? (1 = Yes, 0 = No): ";
        cin >> high_risk;

        if (high_risk) {
            esi_priority = 2;
            return;
        }

        cout << "Does the patient have severe pain or distress? (1 = Yes, 0 = No): ";
        cin >> severe_pain;

        if (severe_pain) {
            cout << "How many resources does the patient require? (Enter a number): ";
            cin >> resources_required;

            if (resources_required >= 2) {
                esi_priority = 2;
            } else {
                esi_priority = 3;
            }
            return;
        }

        cout << "How many resources does the patient require? (Enter a number): ";
        cin >> resources_required;

        if (resources_required >= 2) {
            esi_priority = 3;
        } else {
            esi_priority = 4;
        }
    }
};

// Comparison function for priority queue
struct ComparePriority {
    bool operator()(const Patient* p1, const Patient* p2) {
        return p1->esi_priority > p2->esi_priority; // Lower priority number = higher priority
    }
};

// Patient Manager class
class PatientManager {
private:
    priority_queue<Patient*, vector<Patient*>, ComparePriority> queue;
    mutex mtx;
    condition_variable cv;
    bool exit_flag;

public:
    PatientManager() : exit_flag(false) {}

    ~PatientManager() {
        while (!queue.empty()) {
            delete queue.top();
            queue.pop();
        }
    }

    void push(Patient* patient) {
        lock_guard<mutex> lock(mtx);
        queue.push(patient);
        cv.notify_one();
    }

    void pop() {
        lock_guard<mutex> lock(mtx);
        if (queue.empty()) {
            cout << "No patients in the queue.\n";
            return;
        }
        Patient* top_patient = queue.top();
        queue.pop();

        cout << "Removed patient: " << top_patient->name << " (Priority: " << top_patient->esi_priority << ")\n";
        delete top_patient; // Free memory
    }

    void list_patients() {
        lock_guard<mutex> lock(mtx);
        if (queue.empty()) {
            cout << "No patients in the queue.\n";
            return;
        }

        cout << "\nCurrent Patients:\n";
        cout << "Name\tAge\tSex\tPriority\n";
        cout << "---------------------------------\n";

        vector<Patient*> temp;
        while (!queue.empty()) {
            Patient* p = queue.top();
            queue.pop();
            temp.push_back(p);

            cout << p->name << "\t" << p->age << "\t" << p->sex << "\t" << p->esi_priority << "\n";
        }

        for (Patient* p : temp) {
            queue.push(p); // Rebuild the queue
        }
    }

    void start_icu_consumer() {
        thread([this]() {
            while (true) {
                Patient* top_patient = nullptr;

                {
                    unique_lock<mutex> lock(mtx);
                    cv.wait(lock, [this]() { return !queue.empty() || exit_flag; });

                    if (exit_flag && queue.empty())
                        break;

                    top_patient = queue.top();
                    queue.pop();
                }

                cout << "\n[ICU Consumer] Treating patient: " << top_patient->name
                     << " (Priority: " << top_patient->esi_priority << ")\n";

                this_thread::sleep_for(chrono::seconds(5)); // Simulate ICU treatment
                delete top_patient; // Free memory
            }
        }).detach();
    }

    void stop_icu_consumer() {
        lock_guard<mutex> lock(mtx);
        exit_flag = true;
        cv.notify_all();
    }
};

// Main function
int main() {
    PatientManager manager;
    manager.start_icu_consumer();

    while (true) {
        cout << "\nOptions:\n";
        cout << "1. Add a new patient\n";
        cout << "2. View all patients\n";
        cout << "3. Remove highest-priority patient\n";
        cout << "4. Exit\n";
        cout << "Enter your choice (1-4): ";

        string choice;
        cin >> choice;

        if (choice == "1") {
            string name, sex;
            int age;

            cout << "Enter the patient's name: ";
            cin >> name;
            cout << "Enter the patient's age: ";
            cin >> age;
            cout << "Enter the patient's sex (Male/Female): ";
            cin >> sex;

            Patient* new_patient = new Patient(name, age, sex);
            new_patient->calculate_esi_priority();
            manager.push(new_patient);
        } else if (choice == "2") {
            manager.list_patients();
        } else if (choice == "3") {
            manager.pop();
        } else if (choice == "4") {
            manager.stop_icu_consumer();
            cout << "Exiting...\n";
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
