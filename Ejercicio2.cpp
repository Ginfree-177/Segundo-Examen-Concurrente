#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <chrono>

using namespace std;

// Mutex global solo para evitar mezclas de texto en consola
mutex candado;

class Mesa {
private:
    int pilaPapeles;
    mutex mtx;
    condition_variable cv;

public:
    Mesa() : pilaPapeles(0) {}

    void agregarHoja(int id) {
        unique_lock<mutex> lock(mtx);
        while (pilaPapeles == 3) {
            cv.wait(lock);
        }
        pilaPapeles++;
        {
            lock_guard<mutex> guard(candado);
            cout << "Persona " << id << " colocó una hoja. Papeles en la mesa: " << pilaPapeles << endl;
        }
        if (pilaPapeles == 3) {
            cv.notify_all(); // avisar al engrapador
        }
        lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(100 + rand() % 200));
    }

    void vaciarHojas() {
        unique_lock<mutex> lock(mtx);
        while (pilaPapeles < 3) {
            cv.wait(lock);
        }
        {
            lock_guard<mutex> guard(candado);
            cout << "\n>>> Engrapador: hay 3 papeles, engrapando...\n";
        }
        this_thread::sleep_for(chrono::milliseconds(200));
        pilaPapeles = 0;
        {
            lock_guard<mutex> guard(candado);
            cout << ">>> Engrapador: mesa vacía, continuamos <<<\n\n";
        }
        cv.notify_all(); // despertar a las personas
        lock.unlock();
        this_thread::sleep_for(chrono::milliseconds(150));
    }
};

class Persona {
private:
    int id;
    int repeticiones;
    Mesa* mesa;

public:
    Persona(int identificador, Mesa* m, int rep)
        : id(identificador), mesa(m), repeticiones(rep) {}

    void operator()() {
        for (int i = 0; i < repeticiones; i++) {
            mesa->agregarHoja(id);
        }
    }
};

class Engrapador {
private:
    Mesa* mesa;
    int repeticiones;

public:
    Engrapador(Mesa* m, int rep)
        : mesa(m), repeticiones(rep) {}

    void operator()() {
        for (int i = 0; i < repeticiones; i++) {
            mesa->vaciarHojas();
        }
    }
};

int main() {
    srand(time(nullptr));
    Mesa mesaCompartida;

    int repeticiones = 5;

    // Crear personas y engrapador
    Persona p1(1, &mesaCompartida, repeticiones);
    Persona p2(2, &mesaCompartida, repeticiones);
    Persona p3(3, &mesaCompartida, repeticiones);
    Engrapador engr(&mesaCompartida, repeticiones);

    // Lanzar los hilos
    thread t1(ref(p1));
    thread t2(ref(p2));
    thread t3(ref(p3));
    thread t4(ref(engr));

    // Esperar que terminen
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    {
        lock_guard<mutex> guard(candado);
        cout << "Simulación finalizada.\n";
    }

    return 0;
}
