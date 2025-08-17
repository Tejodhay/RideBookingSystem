#include <bits/stdc++.h>
using namespace std;

// ---------- Location Class ----------
class Location {
    string name;
public:
    Location(string name = "") : name(name) {}
    string getName() const { return name; }
};

// ---------- Vehicle Base Class ----------
class Vehicle {
protected:
    string model;
    string numberPlate;
public:
    Vehicle(string model, string numberPlate) : model(model), numberPlate(numberPlate) {}
    virtual double calculateFare(double distance) const = 0; // Abstract
    virtual string getType() const = 0;
    virtual ~Vehicle() {}
};

class Car : public Vehicle {
public:
    Car(string model, string numberPlate) : Vehicle(model, numberPlate) {}
    double calculateFare(double distance) const override {
        double baseFare = 50; // ₹50 base
        double perKm = 15;    // ₹15 per km
        return baseFare + distance * perKm;
    }
    string getType() const override { return "Car"; }
};

class Bike : public Vehicle {
public:
    Bike(string model, string numberPlate) : Vehicle(model, numberPlate) {}
    double calculateFare(double distance) const override {
        double baseFare = 20;
        double perKm = 8;
        return baseFare + distance * perKm;
    }
    string getType() const override { return "Bike"; }
};

class Auto : public Vehicle {
public:
    Auto(string model, string numberPlate) : Vehicle(model, numberPlate) {}
    double calculateFare(double distance) const override {
        double baseFare = 30;
        double perKm = 10;
        return baseFare + distance * perKm;
    }
    string getType() const override { return "Auto"; }
};

// ---------- Driver Class ----------
class Driver {
    string name;
    Vehicle* vehicle; // Aggregation
    Location location;
    bool available;
public:
    Driver(string name, Vehicle* v, Location loc)
        : name(name), vehicle(v), location(loc), available(true) {}

    string getName() const { return name; }
    Vehicle* getVehicle() const { return vehicle; }
    Location getLocation() const { return location; }
    bool isAvailable() const { return available; }

    void setAvailable(bool status) { available = status; }
};

// ---------- User Class ----------
class User {
    string name;
    vector<string> rideHistory;
public:
    User(string name) : name(name) {
        loadRideHistoryFromFile();
    }

    string getName() const { return name; }

    void addRideHistory(string ride) {
        rideHistory.push_back(ride);
        saveRideToFile(ride);
    }

    void showRideHistory() const {
        if (rideHistory.empty()) {
            cout << "No rides yet.\n";
            return;
        }
        cout << "\n--- Ride History ---\n";
        for (auto &r : rideHistory) {
            cout << r << "\n";
        }
    }

private:
    void loadRideHistoryFromFile() {
        ifstream file("RideHistory.txt");
        string line;
        while (getline(file, line)) {
            if (!line.empty()) rideHistory.push_back(line);
        }
        file.close();
    }

    void saveRideToFile(const string &ride) {
        ofstream file("RideHistory.txt", ios::app); // append mode
        file << ride << "\n";
        file.close();
    }
};

// ---------- Ride Class ----------
class Ride {
    User &user;
    Driver &driver;
    Location pickup, drop;
    double fare;
public:
    Ride(User &u, Driver &d, Location p, Location q, double distance)
        : user(u), driver(d), pickup(p), drop(q) {
        fare = driver.getVehicle()->calculateFare(distance);
        driver.setAvailable(false); // Driver busy
    }

    double getFare() const { return fare; }
    string getDriverName() const { return driver.getName(); }
    string getVehicleType() const { return driver.getVehicle()->getType(); }
    Driver& getDriver() { return driver; }
};

// ---------- Ride Booking App ----------
class RideBookingApp {
    vector<Driver> drivers;
    User *currentUser;
    map<pair<string,string>, double> distanceMap; // Stores distances between places
public:
    RideBookingApp(User *u) : currentUser(u) {
        // Initialize distances (in km)
        distanceMap[{"Bangalore", "Mysore"}] = 150;
        distanceMap[{"Mysore", "Bangalore"}] = 150;
        distanceMap[{"Bangalore", "Chennai"}] = 350;
        distanceMap[{"Chennai", "Bangalore"}] = 350;
        distanceMap[{"Mysore", "Chennai"}] = 500;
        distanceMap[{"Chennai", "Mysore"}] = 500;
    }

    void addDriver(const Driver &d) { drivers.push_back(d); }

    void bookRide() {
        vector<string> places = {"Bangalore", "Mysore", "Chennai"};
        cout << "\nAvailable Locations:\n";
        for (int i = 0; i < places.size(); i++)
            cout << i+1 << ". " << places[i] << "\n";

        int pIndex, dIndex;
        cout << "Select pickup location: ";
        cin >> pIndex;
        cout << "Select drop location: ";
        cin >> dIndex;

        if (pIndex == dIndex || pIndex < 1 || dIndex < 1 || pIndex > places.size() || dIndex > places.size()) {
            cout << "Invalid locations.\n";
            return;
        }

        Location pickup(places[pIndex-1]), drop(places[dIndex-1]);
        double distance = distanceMap[{pickup.getName(), drop.getName()}];

        cout << "Select vehicle type: 1.Car  2.Bike  3.Auto\nChoice: ";
        int choice;
        cin >> choice;

        string vehicleType;
        if (choice == 1) vehicleType = "Car";
        else if (choice == 2) vehicleType = "Bike";
        else vehicleType = "Auto";

        Driver *nearestDriver = nullptr;
        for (auto &d : drivers) {
            if (d.isAvailable() && d.getVehicle()->getType() == vehicleType) {
                nearestDriver = &d;
                break;
            }
        }

        if (!nearestDriver) {
            cout << "No available driver for " << vehicleType << ".\n";
            return;
        }

        Ride ride(*currentUser, *nearestDriver, pickup, drop, distance);
        cout << "\nNearest driver found: " << ride.getDriverName()
             << " (" << ride.getVehicleType() << ")\n";
        cout << "Distance: " << distance << " km\n";
        cout << "Fare: ₹" << ride.getFare() << "\nRide booked successfully!\n";

        // Save to history + file
        currentUser->addRideHistory(
            "Ride with " + ride.getDriverName() + " (" + ride.getVehicleType() +
            ") - " + pickup.getName() + " to " + drop.getName() +
            " - Fare: ₹" + to_string((int)ride.getFare())
        );
    }

    void completeRide() {
        string driverName;
        cout << "Enter driver name to complete ride: ";
        cin >> driverName;

        for (auto &d : drivers) {
            if (d.getName() == driverName && !d.isAvailable()) {
                d.setAvailable(true);
                cout << "Ride completed. " << d.getName() << " is now available.\n";
                return;
            }
        }
        cout << "No such driver is currently busy.\n";
    }
};

// ---------- Main ----------
int main() {
    Driver d1("John", new Car("Hyundai i20", "KA-01-1234"), Location("Bangalore"));
    Driver d2("Raj", new Bike("Hero Splendor", "KA-02-5678"), Location("Mysore"));
    Driver d3("Ali", new Auto("Bajaj RE", "KA-03-2468"), Location("Bangalore"));

    User u1("Tejodhay");

    RideBookingApp app(&u1);
    app.addDriver(d1);
    app.addDriver(d2);
    app.addDriver(d3);

    int option;
    do {
        cout << "\n--- Ride Booking App ---\n";
        cout << "1. Book a Ride\n2. View Ride History\n3. Complete a Ride\n0. Exit\nChoice: ";
        cin >> option;

        switch (option) {
            case 1: app.bookRide(); break;
            case 2: u1.showRideHistory(); break;
            case 3: app.completeRide(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (option != 0);

    return 0;
}
