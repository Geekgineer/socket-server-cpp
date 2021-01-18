/**
 * Calculating the Sieve Eratosthenes to a given limit, Multithreding.
 * 
 * @author
 *   Abdalrahman Ibrahim <a2ibrahim@edu.aau.at>
 */

#include <bits/stdc++.h>

#include <omp.h>

using namespace std;

class SieveEratosthenes
{
private:
public:
    int _limit;
    int p;
    int i;
    int x;
    int _prim_list[];

    SieveEratosthenes() = default;
    ~SieveEratosthenes() = default;

    void SieveEratosthenes_original(int _limit);
    void SieveEratosthenes_parallel(int _limit);
    void PrintSieve(int _limit);
};

void SieveEratosthenes::SieveEratosthenes_original(int _limit)
{
    // Create a boolean array "prime[0..n]" and initialize
    // all entries it as true. A value in prime[i] will
    // finally be false if i is Not a prime, else true.
    bool _prime[_limit + 1];

    // Converts the value ch to unsigned char 'true' and copies it into each of
    // the first n characters of the object pointed to by _prime[]
    memset(_prime, true, sizeof(_prime));

    for (int p = 2; p * p <= _limit; p++)
    {
        // If prime[p] is not changed, then it is a prime
        if (_prime[p] == true)
        {
            // Update all multiples of p greater than or
            // equal to the square of it
            // numbers which are multiple of p and are
            // less than p^2 are already been marked.
            for (int i = p * p; i <= _limit; i += p)
                _prime[i] = false;
        }
    }
};

void SieveEratosthenes::SieveEratosthenes_parallel(int _limit)
{
    // Create a boolean array "prime[0..n]" and initialize
    // all entries it as true. A value in prime[i] will
    // finally be false if i is Not a prime, else true.
    bool _prime[_limit + 1];

    // Converts the value ch to unsigned char 'true' and copies it into each of
    // the first n characters of the object pointed to by _prime[]
    memset(_prime, true, sizeof(_prime));


    int sqrt_limit = int(sqrt(_limit)) +1;

    int procs = omp_get_num_procs(); // get number of process
    int nthreads = procs * 2;

    #pragma omp parallel for shared(p,sqrt_limit,_prime) private(i) schedule(dynamic,1) num_threads(nthreads)
    for (int p = 2; p <= sqrt_limit; p++)
    {
        // If prime[p] is not changed, then it   is a prime
        if (_prime[p] == true)
        {
            // Update all multiples of p greater than or
            // equal to the square of it
            // numbers which are multiple of p and are
            // less than p^2 are already been marked.
            
            for (int i = p; i <= sqrt_limit; i += p)
                _prime[i] = false;
        }

    }

};

void SieveEratosthenes::PrintSieve(int _limit)
{
    bool _prime[_limit + 1];
    // Print all prime numbers
    for (int p = 2; p <= _limit; p++)
        if (_prime[p])
            cout << p << " ";
            // _prim_list[p] = p;
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

// int main()
// {
//     // Select limit
//     // int const limit = 50; //Uncomment this and line 150, 159 see results
//     int const limit = 999999; // max number

//     // Class instance of the Sieve Eratosthenes
//     SieveEratosthenes SE;

//     // Find all prime number using original sieve eratosthenes

//     {

//         PROFILE_BLOCK("Sieve Eratosthenes Original ");
//         SE.SieveEratosthenes_original(limit);
//     }

//     // SE.PrintSieve(limit);

//     {

//         PROFILE_BLOCK("Sieve Eratosthenes Original ");
//         SE.SieveEratosthenes_parallel(limit);
//     }

//     // Print the those primes
//     // SE.PrintSieve(limit);
// }