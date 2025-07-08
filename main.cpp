
#include <iostream>
#include <string>
#include <cmath> // For cos and sin

// Airplane class definition and implementation
class Airplane {
private:
    double x, y, z;
    double vx, vy, vz;
    double pitch, yaw, roll;
    double throttle;

public:
    Airplane()
        : x(0), y(0), z(0),
          vx(0), vy(0), vz(0),
          pitch(0), yaw(0), roll(0),
          throttle(0) {}

    void update(double dt) {
        double speed = throttle * 100.0;
        vx = speed * cos(pitch);
        vz = speed * sin(pitch);

        x += vx * dt;
        z += vz * dt;
    }

    void increaseThrottle() {
        throttle += 0.1;
        if (throttle > 1.0) throttle = 1.0;
        displayMessage("Throttle increased.");
    }

    void decreaseThrottle() {
        throttle -= 0.1;
        if (throttle < 0.0) throttle = 0.0;
        displayMessage("Throttle decreased.");
    }

    void pitchUp() {
        pitch += 0.05;
        displayMessage("Pitch up.");
    }

    void pitchDown() {
        pitch -= 0.05;
        displayMessage("Pitch down.");
    }

    void printStatus() const {
        std::cout << "Position: (" << x << ", " << y << ", " << z << ")\n";
        std::cout << "Velocity: (" << vx << ", " << vy << ", " << vz << ")\n";
        std::cout << "Pitch: " << pitch << "\n";
        std::cout << "Throttle: " << throttle << "\n";
    }

private:
    static void displayMessage(const std::string& message) {
        std::cout << message << std::endl;
    }
};

int main() {
    Airplane plane;
    std::string command;

    std::cout << "=== Simple Flight Simulator ===\n";
    std::cout << "Commands:\n";
    std::cout << "  1. throttle_up\n";
    std::cout << "  2. throttle_down\n";
    std::cout << "  3. pitch_up\n";
    std::cout << "  4. pitch_down\n";
    std::cout << "  5. status\n";
    std::cout << "  6. quit\n";

    std::cout << "\n--- Current Airplane Status ---\n";
    plane.printStatus();
    std::cout << "------------------------------\n";
    std::cout << "Enter a number (1-6) to perform a command from the list above.\n";

    while (true) {
        std::cout << "\n> ";
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            plane.increaseThrottle();
            plane.printStatus();
        }
        else if (choice == 2) {
            plane.decreaseThrottle();
            plane.printStatus();
        }
        else if (choice == 3) {
            plane.pitchUp();
            plane.printStatus();
        }
        else if (choice == 4) {
            plane.pitchDown();
            plane.printStatus();
        }
        else if (choice == 5) {
            plane.printStatus();
        }
        else if (choice == 6) {
            std::cout << "Exiting simulator.\n";
            break;
        }
        else {
            std::cout << "Unknown command.\n";
        }

        plane.update(0.1);  // Advance physics by 0.1 seconds
    }

    return 0;
}
