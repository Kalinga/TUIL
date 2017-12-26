#include <cstdlib>
#include <iostream>
#include <fstream>

#include <ctime>  // clock: cpu time (on linux)
#include <chrono> // system_clock/high_resolution_clock: wall time
#include <unistd.h> // sleep()

#include <iomanip> // setw()
#include <climits> // ..._MAX

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
    static const int N = 64; // amount of numbers
    static const int MIN = 16;
    static const int STEP_DIGITS = 16; // number of binary digits to add each step
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
    // TODO
    if (y == 0)
        return 1;
    if (y == 1)
        return  x % m;
    // This naive implementation is just a placeholder so that the test program works properly.
    // It should be replaced by a more efficient implementation.
    BigInteger z = BigInteger(1);
    z = (x % m * x % m) % m;
    if (!y.isEven())
        z = z * x;
    y  = y / 2;

    return powerModRecursive(z, y, m);
}


// ==========================================================================
// Tests / Time measuring
// ==========================================================================

void testBuiltInDatatypes(void) {
    cout << "type (unsigned ...) | bytes | bits |           max. value" << endl;
    cout << "               char | " << setw(5) << sizeof(unsigned char) << " | "
         << setw(4) << 8 * sizeof(unsigned char) << " | " << setw(20) << UCHAR_MAX << endl;
    cout << "          short int | " << setw(5) << sizeof(unsigned short int) << " | "
         << setw(4) << 8 * sizeof(unsigned short int) << " | " << setw(20) << SHRT_MAX << endl;
    cout << "                int | " << setw(5) << sizeof(unsigned int) << " | "
         << setw(4) << 8 * sizeof(unsigned int) << " | " << setw(20) << UINT_MAX << endl;
    cout << "           long int | " << setw(5) << sizeof(unsigned long int) << " | "
         << setw(4) << 8 * sizeof(unsigned long int) << " | " << setw(20) << ULONG_MAX << endl;
    cout << "      long long int | " << setw(5) << sizeof(unsigned long long int) << " | "
         << setw(4) << 8 * sizeof(unsigned long long int) << " | " << setw(20) << ULLONG_MAX << endl;
}

void testBigInteger(void) {
    // Create a small (at most 64 bits) and a large (more than 64 bits) integer and print it on the console
    BigInteger smallNum = BigInteger(1023);
    BigInteger bigNum("47823478352374829374"); // use strings for large numbers
    cout << "smallNum = " << smallNum << " (decimal) = " << smallNum.toString(2) << " (binary)" << endl;
    cout << "bigNum   = " << bigNum << " (decimal) = " << bigNum.toString(2) << " (binary)" << endl;
    cout << "smallNum * bigNum = " << smallNum * bigNum << endl;
    smallNum = BigInteger("101010", 2);
    cout << "smallNum = " << smallNum << " (decimal) = " << smallNum.toString(2) << " (binary)" << endl;
    bigNum = 47823478352374829374; // do not use integer constants for large numbers
    cout << "bigNum   = " << bigNum << " (decimal) = " << bigNum.toString(2) << " (binary)" << endl;
    smallNum = 7; // smallNum = "7"; is not possible, only smallNum = BigInteger("7");
    bigNum = smallNum + 6;
    cout << "smallNum = " << smallNum << " (decimal) = " << smallNum.toString(2) << " (binary)" << endl;
    cout << "bigNum   = " << bigNum << " (decimal) = " << bigNum.toString(2) << " (binary)" << endl;

    // Do some basic arithmetic operations
    cout << "bigNum + smallNum = " << bigNum + smallNum << endl;
    cout << "bigNum - smallNum = " << bigNum - smallNum << endl;
    cout << "bigNum * smallNum = " << bigNum * smallNum << endl;
    cout << "bigNum / smallNum = " << bigNum / smallNum << endl;
    cout << "bigNum % smallNum = " << bigNum % smallNum << endl;
    cout << "smallNum + bigNum * bigNum     = " << smallNum + bigNum * bigNum << endl;
    cout << "(smallNum + bigNum) * bigNum   = " << (smallNum + bigNum) * bigNum << endl;
    cout << "smallNum * bigNum % smallNum   = " << smallNum * bigNum % smallNum << endl;
    cout << "smallNum * (bigNum % smallNum) = " << smallNum * (bigNum % smallNum) << endl;

    // Use negative integers
    cout << "- bigNum        = " << - bigNum << endl;
    cout << "bigNum.negate() = " << bigNum.negate() << endl;
    cout << "bigNum.abs()    = " << bigNum.abs() << endl;
    cout << "bigNum.abs()    = " << bigNum.abs() << endl;

    // Use special operators
    BigInteger num(10);
    cout << "num    = " << num << endl;
    cout << "num++  = " << num++ << endl;
    cout << "num    = " << num << endl;
    cout << "++num  = " << ++num << endl;
    cout << "num    = " << num << endl;
    cout << "num+=2 = " << (num+=2) << endl;
    cout << "num%=3 = " << (num%=3) << endl;

    // exponentiation and modular exponentiation
    BigInteger base(2), exp(125), mod(127);
    cout << base << "^" << exp << " = ";
    base.power(exp);
    cout << base << endl;
    base = BigInteger(2);
    cout << base << "^" << exp << " mod " << mod << " = ";
    base.powerMod(exp,mod);
    cout << base << endl;

    // shift left and shift right, << and >>
    num = BigInteger::power2(4);
    cout << "initialize n    => " << num << endl;
    cout << "n.shiftLeft(3)  => " << num.shiftLeft(3) << endl;
    cout << "n.shiftRight(2) => " << num.shiftRight(2) << endl;
    cout << "n << 5          => " << (num << 5) << endl;
    cout << "n >> 7          => " << (num >> 7) << endl;

    // cascading operations
    BigInteger a, b, c;
    cout << "a = " << (a = BigInteger(2)) << ", b = " << (b = BigInteger(3)) << ", c = " << (c = BigInteger(5)) << endl;
    for(int i = 0; i < 12; i++, a = 2, b = 3, c = 5) {
        if(i == 0) {
            a = b = c;
            cout << "a = b = c";
        } else if(i == 1) {
            (a = b) = c;
            cout << "(a = b) = c";
        } else if(i == 2) {
            a.swap(b).swap(c);
            cout << "a.swap(b).swap(c)";
        } else if(i == 3) {
            a.add(b).add(c);
            cout << "a.add(b).add(c)";
        } else if(i == 4) {
            a.add(a).add(a);
            cout << "a.add(a).add(a)";
        } else if(i == 5) {
            a.negate();
            cout << "a.negate()";
        } else if(i == 6) {
            a.negate().negate();
            cout << "a.negate().negate()";
        } else if(i == 7) {
            a.power(b).power(c);
            cout << "a.power(b).power(c)";
        } else if(i == 8) {
            a.power(b.power(c));
            cout << "a.power(b.power(c))";
        } else if(i == 9) {
            a.shiftLeft(4).shiftRight(2);
            cout << "a.shiftLeft(4).shiftRight(2)";
        } else if(i == 10) {
            a << 4 >> 2;
            cout << "a << 4 >> 2";
        } else if(i == 11) {
            (a *= b + c) += b;
            cout << "(a *= b + c) += b";
        }
        cout << "\t=>   a = " << a << ", b = " << b << ", c = " << c << endl;
    }
    cout << "(++--++a)++\t=>   a = " << (++--++a)++ << ", b = " << b << ", c = " << c
         << "\t=>   a = " << a << ", b = " << b << ", c = " << c << endl;
    // It is not possible to use multiple postfix operators like (a++)--;

    // comparisons
    smallNum = BigInteger(16);
    bigNum = BigInteger(32);
    if(smallNum < bigNum) {
        cout << smallNum << " < " << bigNum << endl;
    }
    if(smallNum > bigNum) {
        cout << smallNum << " > " << bigNum << endl;
    }
    if(bigNum - smallNum >= 0) {
        cout << bigNum << " - " << smallNum << " >= 0" << endl;
    }

    // swapping numbers
    cout << "a = " << smallNum << endl;
    cout << "b = " << bigNum << endl;
    smallNum.swap(bigNum);
    cout << "swapped" << endl;
    cout << "a = " << smallNum << endl;
    cout << "b = " << bigNum << endl;

    // creating random numbers
    cout << "Random numbers from 0 to 2: ";
    for(int i = 0; i < 64; i++) {
        cout << BigInteger::random(2);
    }
    cout << "\nRandom numbers from 1 to 2: ";
    for(int i = 0; i < 64; i++) {
        cout << BigInteger::random(2,false);
    }
    cout << "\nRandom numbers from 3 to 5: ";
    for(int i = 0; i < 64; i++) {
        cout << BigInteger::random(3,BigInteger(5));
    }
    cout << "\nRandom (at most) 3-bit numbers (from 0 to 7): ";
    for(int i = 0; i < 64; i++) {
        cout << BigInteger::randomBitL(3);
    }
    cout << "\nRandom (exactly) 3-bit numbers (from 4 to 7): ";
    for(int i = 0; i < 64; i++) {
        cout << BigInteger::randomBitL(3,true,false);
    }
    cout << endl;
}

void testTestData(void) {
    TestData data;
    cout << "TestData::N           = " << TestData::N << endl;
    cout << "TestData::MIN         = " << TestData::MIN << endl;
    cout << "TestData::STEP_DIGITS = " << TestData::STEP_DIGITS << endl;
    for(int i = 0; i < TestData::N; i++) {
        cout << "[" << i << "]\t= " << data.get(i) << " (" << data.bits(i) << " bits)" << endl;
    }
}

void testPowerModRecursive(void) {
    BigInteger b(0), e(0), m(2);
    cout << b << " ^ " << e << " mod " << m << " = " << powerModRecursive(b, e, m) << endl;
    b = 2;
    e = 10;
    m = 9;
    cout << b << " ^ " << e << " mod " << m << " = " << powerModRecursive(b, e, m) << endl;
    b = 6;
    e = 10;
    m = BigInteger(6).power(10) + 1;
    cout << b << " ^ " << e << " mod " << m << " = " << powerModRecursive(b, e, m) << endl;
    m = 100;
    cout << b << " ^ " << e << " mod " << m << " = " << powerModRecursive(b, e, m) << endl;
    b = BigInteger("47823478352374829374");
    e = 1024;
    m = 3 * BigInteger::power2(100);
    cout << b << " ^ " << e << " mod " << m << " = " << powerModRecursive(b, e, m) << endl;
    m = 3;
    cout << b << " ^ " << e << " mod " << m << " = " << powerModRecursive(b, e, m) << endl;
}

void testTimer(void) {
    Timer cpuTimer;
    cout << "cpu time:        " << flush;
    cpuTimer.start();
    sleep(2);
    cpuTimer.stop();
    cout << cpuTimer.getTime() << " ms" << endl;

    cout << "wall-clock time: " << flush;
    auto wallTimeStart = std::chrono::system_clock::now();
    sleep(2);
    std::chrono::duration<double> wallTimeDuration = (std::chrono::system_clock::now() - wallTimeStart);
    cout << (wallTimeDuration.count() * 1000) << " ms" << endl;
}

/**
 * Time measuring for 'powerModRecursive'.
 * Writes results to "powerMod.data".
 */
void finalTest() {
    ofstream file;
    file.open("powerMod.data");

    // One set of random numbers for each variable.
    TestData x;
    TestData y;
    TestData m;

    Timer t;

    cout << "bit length of x, y and m --- required time for powerModRecursive (in ms)" << endl;

    // Starting with the shortest numbers, in round i do a calculation of
    // "x[i]^y[i] mod m[i]" using 'powerModRecursive'.
    for (int i = 0; i < TestData::N; i++) {
        // The calculation with time measuring
        t.start();
        BigInteger z = powerModRecursive(x.get(i), y.get(i), m.get(i));
        t.stop();

        // write number of bits of the current numbers and the time used to exponentiate to the file
        file << x.bits(i) << " " << t.getTime() << endl;

        cout << setw(4) << x.bits(i) << " --- " << t.getTime() << endl;
    }

    file.close();
}

int main(int argc, char** argv) {
    // Step 1
    cout << "##### Step 1 ### testBuiltInDatatypes #####" << endl;
    testBuiltInDatatypes();
    cout << "### Step 1 done ###" << endl;
    cin.get();

    // Step 2
    cout << "##### Step 2 ### testBigInteger #####" << endl;
    testBigInteger();
    cout << "### Step 2 done ###" << endl;
    cin.get();

    // Step 3
    cout << "##### Step 3 ### testTestData #####" << endl;
    testTestData();
    cout << "### Step 3 done ###" << endl;
    cin.get();

    // Step 4
    cout << "##### Step 4 ### testPowerModRecursive #####" << endl;
    testPowerModRecursive();
    cout << "### Step 4 done ###" << endl;
    cin.get();

    // Step 5
    cout << "##### Step 5 ### testTimer #####" << endl;
    testTimer();
    cout << "### Step 5 done ###" << endl;
    cin.get();

    // Step 6
    cout << "##### Step 6 ### finalTest #####" << endl;
    finalTest();
    cout << "### Step 6 done ###" << endl;
    cin.get();

    return 0;
}
