#include<iostream>
#include<thread> 
#include<mutex>
#include<condition_variable>

//Recordatorio del la explicacion con dibujitos
using namespace std;

class mesa
{
public:
    int papeles;
    mutex mtx;
    condition_variable cv;
public:
    mesa()
    {
        pilaPapeles = 0;
    }
    void agregarHoja()
    {
        unique_lock<mutex> lock(mtx);
        while (pilaPapeles == 3) {
            cv.wait(lock);
        }
        mesa++;
        if (pilaPapeles == 3) {
            cv.notify_all();
        }
        lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(100 + rand() % 200));
    }
    void vaciarHojas()
    {
        unique_lock<mutex> lock(mtx);

        // Espera hasta que haya 3 papeles
        while (mesa < 3) {
            cv.wait(lock);
        }
        mesa = 0;
        cv.notify_all();
        lock.unlock();

        this_thread::sleep_for(chrono::milliseconds(150));
    }
};

int main()
{
    return 0; 
}