#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <mutex>

const int w = 800;
const int h = 600;
const int speed = 1;

std::mutex mtx;


class Point {
public:
    int x, y;

    Point() : x(0), y(0) {}

    Point(int x, int y) : x(x), y(y) {}
};

int random(int min, int max) {
    static bool first = true;
    if (first) {
        srand(static_cast<unsigned int>(time(nullptr)));
        first = false;
    }
    return min + rand() % ((max + 1) - min);
}

class Vehicle {
public:
    Point position;
    Point target;  //точка цілі руху
    bool moving;
    std::string type;

    Vehicle(Point pos, Point tgt, std::string t) : position(pos), target(tgt), moving(true), type(t) {}

    void move() {
        while (moving) {
            if (position.x < target.x) position.x += speed;
            if (position.x > target.x) position.x -= speed;
            if (position.y < target.y) position.y += speed;
            if (position.y > target.y) position.y -= speed;

            if (position.x == target.x && position.y == target.y) {
                moving = false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << type << " arrived at (" << position.x << ", " << position.y << ")\n";
    }
};

void createVehicles(int n, bool isTruck) {
    std::vector<std::thread> threads;
    for (int i = 0; i < n; ++i) {
        Point start, end;
        std::string type;
        if (isTruck) {
            start = Point(random(0, w / 2), random(0, h / 2));
            end = Point(random(0, w / 2), random(0, h / 2));
            type = "Truck";
        }
        else {
            start = Point(random(w / 2, w), random(h / 2, h));
            end = Point(random(w / 2, w), random(h / 2, h));
            type = "Car";
        }
        Vehicle* vehicle = new Vehicle(start, end, type);
        threads.emplace_back(&Vehicle::move, vehicle);
    }
    for (auto& th : threads) {
        th.join();
    }
}

int main() {
    int numTrucks = 5;
    int numCars = 5;

    std::thread truckThread(createVehicles, numTrucks, true);
    std::thread carThread(createVehicles, numCars, false);

    truckThread.join();
    carThread.join();

    return 0;
}