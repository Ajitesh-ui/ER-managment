#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <iomanip>

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
    Patient(string name, int age, string sex) {
        this->name = name;
        this->age = age;
        this->sex = sex;
        this->arrival_time = time(nullptr); // Current time
        this->esi_priority = -1; // Default priority
    }

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
    bool operator()(const Patient& p1, const Patient& p2) {
        return p1.esi_priority > p2.esi_priority; // Lower priority number = higher priority
    }
};

// Priority queue for patients
class PatientQueue {
private:
    priority_queue<Patient, vector<Patient>, ComparePriority> queue;

public:
    // Add a patient to the queue
    void add_patient(Patient patient) {
        queue.push(patient);
    }

    // Serve the highest-priority patient
    void serve_patient() {
        if (queue.empty()) {
            cout << "No patients in the queue.\n";
            return;
        }
        Patient next_patient = queue.top();
        queue.pop();

        cout << "Serving patient: " << next_patient.name << endl;
        cout << "Age: " << next_patient.age << ", Sex: " << next_patient.sex << endl;
        cout << "ESI Priority: " << next_patient.esi_priority << endl;
        cout << "Arrival Time: " << ctime(&next_patient.arrival_time);
    }

    // Display all patients
    void display_queue() {
        if (queue.empty()) {
            cout << "No patients in the queue.\n";
            return;
        }

        cout << left << setw(15) << "Name" << setw(10) << "Age" << setw(10) << "Sex" 
             << setw(20) << "Arrival Time" << setw(15) << "Priority" << endl;
        cout << string(70, '-') << endl;

        vector<Patient> temp;
        while (!queue.empty()) {
            Patient p = queue.top();
            queue.pop();
            temp.push_back(p);

            cout << left << setw(15) << p.name << setw(10) << p.age << setw(10) << p.sex
                 << setw(20) << ctime(&p.arrival_time) << setw(15) << p.esi_priority << endl;
        }

        for (Patient p : temp) {
            queue.push(p); // Rebuild the queue
        }
    }
};

// Main function
int main() {
    PatientQueue pq;
    int choice;

    do {
        cout << "\n=== Patient Management System ===\n";
        cout << "1. Add a new patient\n";
        cout << "2. Serve the highest-priority patient\n";
        cout << "3. Display all patients\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string name, sex;
                int age;

                cout << "Enter patient name: ";
                cin >> name;
                cout << "Enter patient age: ";
                cin >> age;
                cout << "Enter patient sex: ";
                cin >> sex;

                Patient new_patient(name, age, sex);
                new_patient.calculate_esi_priority();
                pq.add_patient(new_patient);

                cout << "Patient added successfully.\n";
                break;
            }
            case 2:
                pq.serve_patient();
                break;
            case 3:
                pq.display_queue();
                break;
            case 4:
                cout << "Exiting the system. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);

    return 0;
}


