/**
 * Preform Matrix Multiplication and other matrix opreations, Multithreding.
 * Given random matrix or identity matrixes
 * 
 * @author
 *   Abdalrahman Ibrahim <a2ibrahim@edu.aau.at>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <valgrind/callgrind.h>

#include <iostream>
#include <bits/stdc++.h>
#include <random>

#include <omp.h>

#define M 2
#define N (1 << M)

typedef double datatype;
#define DATATYPE_FORMAT "%4.1f"

typedef datatype mat[N][N]; // mat[2**M,2**M]  for divide and conquer mult.

typedef struct
{
    int ra, rb, ca, cb;
} corners; // for tracking rows and columns.
// A[ra..rb]
//  [ca..cb] .. the 4 corners of a matrix.

class MatrixOpreations
{

private:
    typedef double datatype;

#define DATATYPE_FORMAT "%4.1f"
    typedef datatype mat[N][N]; // mat[2**M,2**M]  for divide and conquer mult.

    int i, j, m, n, k;

public:
    mat _A;
    mat _B;
    mat _C;

    corners _ai;
    corners _bi;
    corners _ci;

    MatrixOpreations() = default;
    MatrixOpreations(MatrixOpreations &&) = default;
    MatrixOpreations(const MatrixOpreations &) = default;
    MatrixOpreations &operator=(MatrixOpreations &&) = default;
    MatrixOpreations &operator=(const MatrixOpreations &) = default;
    ~MatrixOpreations() = default;

    void identity(mat _A, corners _a);

    void rand_mat(mat _A, corners _a, double _l, double _h);

    void set(mat _A, corners _a, datatype k);

    void add(mat _A, mat _B, mat _C, corners _a, corners _b, corners _c);

    void mult_normal(mat _A, mat _B, mat _C, corners _a, corners _b, corners _c);
    void mult_parallel(mat _A, mat _B, mat _C, corners _a, corners _b, corners _c);

    void print(mat _A, corners _a, char *name);
};

void MatrixOpreations::print(mat _A, corners _a, char *name)
{
    int i, j;
    printf("%s = {\n", name);
    for (i = _a.ra; i < _a.rb; i++)
    {
        for (j = _a.ca; j < _a.cb; j++)
            printf(DATATYPE_FORMAT ", ", _A[i][j]);
        printf("\n");
    }
    printf("}\n");
};

void MatrixOpreations::identity(mat _A, corners _a)
{
    for (i = _a.ra; i < _a.rb; i++)
        for (j = _a.ca; j < _a.cb; j++)
            _A[i][j] = (datatype)(i == j);
};

// set A[a] = [random(l..h)].
void MatrixOpreations::rand_mat(mat _A, corners _a, double _l, double _h)
{
    // create a random number generator for the given limits
    const int range_from = _l;
    const int range_to = _h;
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int> distr(range_from, range_to);

    // Initialise the matrix with uniformly distributed random numbers
    for (i = _a.ra; i < _a.rb; i++)
        for (j = _a.ca; j < _a.cb; j++)
            // _A[i][j] = (datatype)(_l + (_h - _l) * (rand() / (double)RAND_MAX));
            _A[i][j] = (datatype)(distr(generator));
    
}

void MatrixOpreations::mult_normal(mat _A, mat _B, mat _C, corners _a, corners _b, corners _c)
{

    // Check: A[m n] * B[n k] = C[m k]
    m = _a.rb - _a.ra;
    assert(m == (_c.rb - _c.ra));
    n = _a.cb - _a.ca;
    assert(n == (_b.rb - _b.ra));
    k = _b.cb - _b.ca;
    assert(k == (_c.cb - _c.ca));
    assert(m > 0);

    if (n == 1)
    {
        _C[_c.ra][_c.ca] += _A[_a.ra][_a.ca] * _B[_b.ra][_b.ca];
        return;
    }

    // A[ra..rb]
    //  [ca..cb] .. the 4 corners of A matrix.
    // B[ra..rb]
    //  [ca..cb] .. the 4 corners of B matrix.
    // A[r1][c1],B[r2][c2]

    for (i = 0; i < _a.rb; i++)
    {
        for (j = 0; j < _b.cb; j++)
        {
            _C[i][j] = 0;
            for (k = 0; k < _b.rb; k++)
            {
                _C[i][j] += _A[i][k] * _B[k][j];
            }
        }
    }
};

void MatrixOpreations::mult_parallel(mat _A, mat _B, mat _C, corners _a, corners _b, corners _c)
{

    // Check: A[m n] * B[n k] = C[m k]
    m = _a.rb - _a.ra;
    assert(m == (_c.rb - _c.ra));
    n = _a.cb - _a.ca;
    assert(n == (_b.rb - _b.ra));
    k = _b.cb - _b.ca;
    assert(k == (_c.cb - _c.ca));
    assert(m > 0);

    if (n == 1)
    {
        _C[_c.ra][_c.ca] += _A[_a.ra][_a.ca] * _B[_b.ra][_b.ca];
        return;
    }

    // A[ra..rb]
    //  [ca..cb] .. the 4 corners of A matrix.
    // B[ra..rb]
    //  [ca..cb] .. the 4 corners of B matrix.
    // A[r1][c1],B[r2][c2]

    // #pragma omp parallel for shared(_A,_B) private(i, k, j) lastprivate(_C) schedule(dynamic,1)
    // #pragma omp parallel for shared(_A,_B,_a,_b,_C) private(i, k, j) schedule(dynamic,1) //working
    // #pragma omp parallel for shared(_A,_B,_a,_b) private(i,j,k) schedule(dynamic,1) reduction(+:_C[i][j])
    
    int procs = omp_get_num_procs(); // get number of process
    int nthreads = procs * 2;

    #pragma omp parallel for shared(_A,_B,_a,_b,_C) private(i,j,k) schedule(dynamic,1) num_threads(nthreads) 
    for (i = 0; i < _a.rb; i++)
    {
        // #pragma omp parallel for private(j)
        for (j = 0; j < _b.cb; j++)
        {
            _C[i][j] = 0;

            // #pragma omp parallel for nowait
            for (k = 0; k < _b.rb; k++)
            {
                // #pragma omp atomic
                _C[i][j] += _A[i][k] * _B[k][j];
            }
        }
    }
    
};

// C[c] = A[a] + B[b]
void MatrixOpreations::add(mat _A, mat _B, mat _C, corners _a, corners _b, corners _c)
{
    int rd = _a.rb - _a.ra;
    int cd = _a.cb - _a.ca;
    int i, j;
    for (i = 0; i < rd; i++)
    {
        for (j = 0; j < cd; j++)
        {
            _C[i + _c.ra][j + _c.ca] = _A[i + _a.ra][j + _a.ca] + _B[i + _b.ra][j + _b.ca];
        }
    }
}

// set A[a] = k
void MatrixOpreations::set(mat _A, corners _a, datatype k)
{

    for (i = _a.ra; i < _a.rb; i++)
        for (j = _a.ca; j < _a.cb; j++)
            _A[i][j] = k;
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

// int main(int argc, const char **argv)
// {

//     MatrixOpreations op;

//     // for a given custom matrix
//     // corners ai = {0, 3, 0, 5};
//     // corners bi = {0, 5, 0, 4};
//     // corners ci = {0, 3, 0, 4};

//     // for a given square matrix 8x8 random
//     corners ai = {0, N, 0, N};
//     corners bi = {0, N, 0, N};
//     corners ci = {0, N, 0, N};

//     mat A;
//     mat B;
//     mat C;

//     // op.identity(A,ai);
//     // op.identity(B,bi);

//     //op.set(A,ai,33);
//     //op.set(B,bi,22);

//     op.rand_mat(A, ai, 0, 55555);
//     op.rand_mat(B, bi, 0, 55555);

//     //op.print(C, ci, "C");
//     // op.print(A, ai, "A");
//     //op.print(B, bi, "B");
//     //op.set(C, ci, 0);

//     //CALLGRIND_START_INSTRUMENTATION;
//     // function to measure
//     // CALLGRIND_STOP_INSTRUMENTATION;
//     // CALLGRIND_DUMP_STATS;

//     // Using the Profiler
//     {

//         PROFILE_BLOCK("Multiplication ");
//         op.mult_normal(A, B, C, ai, bi, ci);
//     }

//     // op.print(C, ci, "C");

//     {

//         PROFILE_BLOCK("Multiplication ");
//         op.mult_parallel(A, B, C, ai, bi, ci);
//     }

//     // op.print(C, ci, "C");

//     return 0;
// }
