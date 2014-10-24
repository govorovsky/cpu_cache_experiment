#include <iostream>
#include<algorithm>
#include<vector>
#include<random>
#include<chrono>
#include<iomanip>

using namespace std;

#define KB 1024
#define MB 1024 * 1024

#define START_SIZE 8*KB
#define END_SIZE 9*MB
#define CYCLES_CNT 5000
#define REP(a) a a

#define MIDDLE 20

const unsigned seed = chrono::system_clock::now().time_since_epoch().count();


struct cache_el {
    cache_el * next;
    cache_el() : next(NULL) {
    }
};

void fill_consistently(cache_el * el, unsigned long size) {
    for(unsigned long i = 0; i< size - 1; i++) {
        el[i].next = &el[i+1];
    }
    el[size - 1].next = el;
}

void fill_rand(cache_el * el, unsigned long size) {
    vector<int> ixs(size);
    int base = 0;

    generate(ixs.begin(), ixs.end(), [&] () {
        return base++;
    });
    shuffle(ixs.begin(), ixs.end(), default_random_engine(seed));
//    for(auto i : ixs) cout << i << " ";
//    cout << "\n" << flush;

    for(unsigned long i = 0; i < size - 1; i++) {
        el[ixs[i]].next = &el[ixs[i+1]];

    }
    el[ixs[size-1]].next = &el[ixs[0]];
}

double measure_time_single(cache_el * el, unsigned long size) {

    volatile cache_el * _el = el;
    const int times = 256;


    auto begin_t = chrono::steady_clock::now();

    for(int i = 0; i < CYCLES_CNT; i++) {
        REP(REP(REP(REP(REP(REP(REP(REP(_el= _el->next;)))))))) // n times
    }

    auto end_t = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end_t - begin_t);
    auto elapsed =  (duration / (CYCLES_CNT * times)).count();

    return elapsed;
}

double measure_time_middle(cache_el * el, unsigned long size) {
    double mid = 0;
    for(int i = 0; i < MIDDLE; i++) {
        mid += measure_time_single(el,size);
    }
    return mid / MIDDLE;
}


int main() {
    cache_el * elements;
    for(unsigned long i = START_SIZE; i < END_SIZE; i*=1.19) {
        unsigned long elements_fit = i/sizeof(cache_el);
        elements = new cache_el[elements_fit];
        fill_rand(elements, elements_fit);
//       fill_consistently(elements, elements_fit);
//       cout << setprecision(9) <<" ---- " << i << " bytes, time: " << measure_time(elements, elements_fit)  << "ns" << "\n";
        cout << (double)i/KB << " " <<  measure_time_middle(elements, elements_fit) << endl;
        delete[] elements;
    }


    return 0;
}

