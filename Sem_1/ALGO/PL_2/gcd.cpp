#include <cstdlib>
#include <iostream>
#include <fstream>
#include <time.h>

#include "../BigInteger.h"

using namespace std;

// ==========================================================================
// Helper classes
// ==========================================================================

/**
 * Allows to measure CPU time between calling start() and stop() method.
 * See http://www.cplusplus.com/reference/ctime/clock/.
 */
class Timer {
private:
    clock_t s;
    long time;
public:

    void start() {
        s = clock();
    }

    long stop() {
        time = (clock() - s) * 1000 / CLOCKS_PER_SEC;
        return time;
    }

    long getTime() {
        return time;
    }

};

/**
 * Create random test numbers with the amount of digits specified by the constants.
 */
class TestData {
public:
    static const int N = 4096; // amount of numbers
    static const int MIN = 1;
    static const int STEP_DIGITS = 1; // number of binary digits to add each step
private:
    BigInteger numbers[N];
public:

    TestData() {
        generate();
    }

    void generate() {
        size_t digits = MIN;
        for (int i = 0; i < N; i++) {
            numbers[i] = BigInteger::randomBitL(digits, false, false);
            digits += STEP_DIGITS;
        }
    }

    /**
     * Get the number at the specified index.
     * @param i
     * @return
     */
    BigInteger get(int i) {
        return numbers[i];
    }

    /**
     * Get the amount of bits of the number at index i.
     * @param i
     * @return
     */
    int bits(int i) {
        return MIN + i*STEP_DIGITS;
    }
};


// ==========================================================================
// ALGORITHMS
// ==========================================================================

/**
 * Fast modular exponentiation, recursive.
 * @param x
 * @param y
 * @param m
 * @return x^y mod m
 */
BigInteger powerModRecursive(BigInteger x, BigInteger y, const BigInteger& m) {
    if (y == 0) {
        return 1;
    }
    if (y == 1) {
    	return x % m;
    }
    BigInteger z = powerModRecursive(x * x % m, y / 2, m);
    if (y.isEven()) {
        return z;
    } else {
        return (z * x % m);
    }
}

/**
 * Computes the GCD of the numbers.
 */
BigInteger gcdNaive(BigInteger a, BigInteger b) {
    if (a < b) {
        return gcdNaive(b, a);
    }
    if(b == 0) {
        return a;
    }
    // It is a naive method to compute the gcd.
    for (BigInteger d = b; d > 0; d--) {
        if (a % d == 0 && b % d == 0) {
            return d;
        }
    }

   /*// This another naive method to compute the gcd.
    BigInteger t = 0;
    for (BigInteger i = 1; (i * i < a && i < b ); ++i) {
        if (((a % i) ==  0) && ((b % i) ==  0)) {
            t = i;
        }
    }

    return t;*/

}

/**
 * Euclid's algorithm.
 * Computes the GCD of the numbers.
 */
BigInteger gcd(BigInteger a, BigInteger b) {
    // It is a more efficient implementation.
    if (b == 0) return a;
    if (a == 0) return b;

    return gcd(b, a % b);
}

/**
 * Result of the extended Euclid's algorithm.
 * GCD(a, b) = gcd = ax + by
 */
struct EuclidResult {
    BigInteger x;
    BigInteger y;
    BigInteger gcd;
};

/**
 * Extended Euclid's algorithm
 * Computes the GCD of the numbers and stores it into the result.
 */
void extendedEuclid(const BigInteger& a, const BigInteger& b, EuclidResult& result) {
    if (a < 0) {
        throw std::runtime_error("Extended Euclid's Algorithm works only for non-negative values of a.");
    }
    if (b < 0) {
        throw std::runtime_error("Extended Euclid's Algorithm works only for non-negative values of b.");
    }

    if (b == 0) {
        result.gcd = a;
        result.x = 1;
        result.y = 0;
    } else
    {
        extendedEuclid(b, a%b, result);

        BigInteger x = result.x;
        result.x = result.y;
        result.y = x - (a/b)*result.y;
    }
}

/**
 * (Extended) Euclid's algorithms
 * Computes the GCD of the given numbers.
 */
EuclidResult extendedEuclid(const BigInteger& a, const BigInteger& b) {
    EuclidResult result;
    extendedEuclid(a, b, result);
    return result;
}

/**
 * Computes the multiplicative inverse of x modulo m.
 */
BigInteger modularInverse(const BigInteger& x, const BigInteger& m) {
    EuclidResult r = extendedEuclid(x, m);
    if(r.gcd == 1) {
        return r.x % m; // In case of negative r.x.
    } else {
        throw std::runtime_error(x.toString() + " has no modular inverse modulo " + m.toString() + "!");
    }
}


// ==========================================================================
// Tests / Time measuring
// ==========================================================================

void testGcd() {
    ofstream fileNaive, fileEuclid;
    fileNaive.open("gcdNaive.data");
    fileEuclid.open("gcdEuclid.data");

    TestData a;
    TestData b;

    Timer t;

    /*
     * Test naive and Euclid implementation "separately", because difference is so big that comparison for same numbers does not make sense.
     * Max bit length of a few thousand is suitable for Euclid, only up to 32 for naive.
     */

    // test gcdNaive
    cout << "gcdNaive:" << endl;
    for (int i = 0; i < min((int)TestData::N, 22); i++) {
        t.start();
        BigInteger d = gcdNaive(a.get(i), b.get(i));
        t.stop();
        fileNaive << a.bits(i) << " " << t.getTime() << endl;
        cout << a.bits(i) << "bit  t=" << t.getTime() << "ms  gcdNaive(" << a.get(i) << ", " << b.get(i) << ") = " << d << endl;
    }
    fileNaive.close();

    // test gcdEuclid
    cout << "gcdEuclid:" << endl;
    for (int i = 15; i < min((int)TestData::N, /*2256*/1600); i+=16) {
        t.start();
        BigInteger d = gcd(a.get(i), b.get(i));
        t.stop();
        fileEuclid << a.bits(i) << " " << t.getTime() << endl;
        cout << a.bits(i) << "bit  t=" << t.getTime() << "ms  gcd(" << a.get(i) << ", " << b.get(i) << ") = " << d << endl;
    }
    fileEuclid.close();
}

void testExtendedEuclid() {
    ofstream fileExtended;
    fileExtended.open("gcdExtended.data");

    TestData a;
    TestData b;

    Timer t;

    cout << "extendedEuclid:" << endl;
    for (int i = 15; i < min((int)TestData::N, /*2256*/1600); i+=16) {
        t.start();
        EuclidResult res = extendedEuclid(a.get(i), b.get(i));
        t.stop();
        fileExtended << a.bits(i) << " " << t.getTime() << endl;
        cout << a.bits(i) << "bit  t=" << t.getTime() << "ms  extendedEuclid("
             << a.get(i) << ", " << b.get(i) << ").gcd = " << res.gcd << endl;
    }
    fileExtended.close();
}

int main(int argc, char** argv) {

    // Step 1
    cout << "##### Step 1 ### test gcdNaive #####" << endl;
    cout << "gcdNaive(12,9) = " << gcdNaive(12,9) << endl;
    cout << "gcdNaive(17,13) = " << gcdNaive(17,13) << endl;
    cout << "gcdNaive(0,0) = " << gcdNaive(0,0) << endl;
    cout << "### Step 1 done ###" << endl;
    cin.get();

    // Step 2
    cout << "##### Step 2 ### test gcd #####" << endl;
    cout << "gcd(12,9) = " << gcd(12,9) << endl;
    cout << "gcd(17,13) = " << gcd(17,13) << endl;
    cout << "gcd(0,0) = " << gcd(0,0) << endl;
    cout << "### Step 2 done ###" << endl;
    cin.get();

    // Step 3
    cout << "##### Step 3 ### testGcd #####" << endl;
    testGcd();
    cout << "### Step 3 done ###" << endl;
    cin.get();

    // Step 4
    cout << "##### Step 4 ### test gcd and extendedEuclid #####" << endl;
    EuclidResult res;
    cout << "gcd(12,9) = " << gcd(12,9) << " = " << (res = extendedEuclid(12,9)).gcd << " = extendedEuclid(12,9).gcd" << endl;
    cout << "12 * " << res.x << " + 9 * " << res.y << " = " << res.gcd << endl;
    cout << "gcd(17,13) = " << gcd(17,13) << " = " << (res = extendedEuclid(17,13)).gcd << " = extendedEuclid(17,13).gcd" << endl;
    cout << "17 * " << res.x << " + 13 * " << res.y << " = " << res.gcd << endl;
    cout << "gcd(0,0) = " << gcd(0,0) << " = " << (res = extendedEuclid(0,0)).gcd << " = extendedEuclid(0,0).gcd" << endl;
    cout << "0 * " << res.x << " + 0 * " << res.y << " = " << res.gcd << endl;
    cout << "### Step 4 done ###" << endl;
    cin.get();

    // Step 5
    cout << "##### Step 5 ### testExtendedEuclid #####" << endl;
    testExtendedEuclid();
    cout << "### Step 5 done ###" << endl;
    cin.get();

    // Step 6
    cout << "##### Step 6 ### test modularInverse #####" << endl;
    try {
        cout << "12 ^ (-1) mod 9 = " << modularInverse(12,9) << endl;
    } catch(std::runtime_error e) {
        cout << e.what() << endl;
    }
    try {
        cout << "9 ^ (-1) mod 12 = " << modularInverse(9,12) << endl;
    } catch(std::runtime_error e) {
        cout << e.what() << endl;
    }
    cout << "17 ^ (-1) mod 13 = " << modularInverse(17,13) << endl;
    cout << "13 ^ (-1) mod 17 = " << modularInverse(13,17) << endl;
    try {
        cout << "0 ^ (-1) mod 0 = " << modularInverse(0,0) << endl;
    } catch(std::runtime_error e) {
        cout << e.what() << endl;
    }
    cout << "### Step 6 done ###" << endl;
    cin.get();

    return 0;
}
