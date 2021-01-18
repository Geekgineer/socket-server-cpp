/**
 * Preform Sorting using Bubble Sort Algorithm Multithreding.
 * Given/generated random arrays
 *
 * @author
 *   Abdalrahman Ibrahim <a2ibrahim@edu.aau.at>
 */

#include <bits/stdc++.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <valgrind/callgrind.h>

#include <iostream>
#include <random>

#include <omp.h>

#define M 12
#define N (1 << M)

typedef double datatype;
#define DATATYPE_FORMAT "%"
typedef datatype arr[N];
#define RANGE 255
typedef struct
{
    // int ra, rb, ca, cb;
    //arr[ra .... rb]
    int ra, rb;
} corners; // for tracking the first and last element of an arr.

class SortingAlgo
{
private:
    int i, j, m, n, k, temp;

public:
    SortingAlgo() {}
    ~SortingAlgo() {}

    // Random Generator
    void rand_arr(arr _A, corners _a, double _l, double _h);

    // Sorting Algos
    void BubbleSort(arr _A, corners _a);

    // Sorting (Multithreding)
    void BubbleSort_parallel(arr _A, corners _a);
    
    // Print Array
    void disply_arr(arr _A, corners _a, char *name);

};

void SortingAlgo::BubbleSort(arr _A, corners _a)
{

    // sliding window for _A[ra,,,i,j,,,rb]
    for (i = 0; i < _a.rb; i++)
    {
        for (j = i + 1; j <= _a.rb; j++)
        {
            // flip poses
            if (_A[i] < _A[j])
            {
                temp = _A[i];
                _A[i] = _A[j];
                _A[j] = temp;
            }
        }
    }
};


void SortingAlgo::BubbleSort_parallel(arr _A, corners _a)
{

    // sliding window for _A[ra,,,i,j,,,rb]

    int procs = omp_get_num_procs(); // get number of process
    int nthreads = procs * 2;

    #pragma omp parallel for shared(_A,_a) private(i,j) schedule(dynamic,1) num_threads(nthreads) 
    for (i = 0; i < _a.rb; i++)
    {
        for (j = i + 1; j <= _a.rb; j++)
        {
            // flip poses
            if (_A[i] < _A[j])
            {
                temp = _A[i];
                _A[i] = _A[j];
                _A[j] = temp;
            }
        }
    }
};

void SortingAlgo::rand_arr(arr _A, corners _a, double _l, double _h)
{
    // create a random number generator for the given limits
    const int range_from = _l;
    const int range_to = _h;
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int> distr(range_from, range_to);

    // Initialise the arr with uniformly distributed random numbers from the given ranges
    for (i = _a.ra; i < _a.rb; i++)
        _A[i] = (datatype)(distr(generator));
}

void SortingAlgo::disply_arr(arr _A, corners _a, char *name)
{
    printf("%s = {\n", name);
    for (i = _a.ra; i < _a.rb; i++)
    {
        printf(DATATYPE_FORMAT ", ", _A[i]);
        printf("\n");
    }
    printf("}\n");
};

//********************PROFILER Using Chrono********************//
struct profiler
{
    std::string name;
    std::chrono::high_resolution_clock::time_point p;
    profiler(std::string const &n) : name(n), p(std::chrono::high_resolution_clock::now()) {}
    ~profiler()
    {
        //using dura = std::chrono::duration<double>;
        //using dura = std::chrono::microseconds;

        using elapsednas = std::chrono::nanoseconds;
        using elapsedmic = std::chrono::microseconds;
        using elapsedmis = std::chrono::milliseconds;

        auto d = std::chrono::high_resolution_clock::now() - p;

        std::cout << name << "Time Measured: " << std::chrono::duration_cast<elapsednas>(d).count() << "[µs] ,"
                  << std::chrono::duration_cast<elapsedmic>(d).count() << "[ns] ,"
                  << std::chrono::duration_cast<elapsedmis>(d).count() << "[ms] "
                  << std::endl;
    }
};

#define PROFILE_BLOCK(pbn) profiler _pfinstance(pbn)

//********************PROFILER Using Chrono********************//

int main(int argc, char const *argv[])
{
    arr A;

    // Array limit
    corners ai = {0, 3999};

    SortingAlgo SA;

    // Generate Random Array[3999] each element from rang 1 to 15000
    SA.rand_arr(A, ai, 1, 15000);

    //SA.disply_arr(A,ai,"A");

    // Call the sorting algorithm
    //SA.BubbleSort(A, ai);

    // BubbleSort one thread
    {
        PROFILE_BLOCK("Bubble Sort ");
        SA.BubbleSort(A, ai);
    }

    // SA.disply_arr(A,ai,"A");

    // BubbleSort Multi-theads
    {
        PROFILE_BLOCK("Bubble Sort ");
        SA.BubbleSort_parallel(A, ai);
    }

    // SA.disply_arr(A,ai,"A");

}
