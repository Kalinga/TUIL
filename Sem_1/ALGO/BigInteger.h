#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

/**
 * Simple implementation of numbers with arbitrary bit length.
 *
 * @file This file  has been created/modified by former and current assistants and student
 * assistants of Prof. Dietzfelbinger at TU Ilmenau. And it can be used for educational
 * purpose without any guarantee or warranty."
 */

#include <string>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <ostream>
#include <istream>
#include <ios>
#include <stack>
#include <vector>
#include <cstdlib>

/**
 * BigInteger is a class representing arbitrarily large integer numbers.
 *
 * The representation is as follows - we store the bits of the number in the array. Each bit is in a single array cell.
 * Also the least significant bit is the first in the array and so on. The number 10 in decimal equals 1010 in binary
 * and is represented as |0|1|0|1|. The negative numbers are stored using twos complement. The flag positive tells us if
 * the number is positive or zero (stored directly) or negative (and thus stored using the twos complement).
 *
 * The operations are implemented in an update fashion. For example adding means adding the given argument to `this`.
 * A simple advantage is a smaller memory consumption and less memory operations. However conventional operators are
 * provided.
 */
class BigInteger {
public:
    /**
     * Creates the big decimal from an integer value.
     */
    BigInteger(int value = 0):
        digits(new char [sizeof(value) * 8]),
        positive(true),
        length(sizeof(value) * 8) {
        // If the value is negative make it positive, we will change the signs at the end.
        bool valuePositive = value >= 0;
        if (!valuePositive) {
            value = -value;
        }

        // Copy the digits.
        for (size_t i = 0; i < length; ++i) {
            digits[i] = value % 2;
            value /= 2;
        }

        // Change the sign if necessary.
        if (!valuePositive) {
            negate();
        }

        // Normalize the number.
        normalize();
    }

    /**
     * Creates the BigInteger from a given string.
     */
    explicit BigInteger(std::string value, std::size_t radix = 10):
        digits(0),
        positive(true),
        length(0) {
        // Skip the sign if necessary.
        size_t start = (value.size() > 0 && value[0] == '-') ? 1 : 0;

        // Conversion from a given radix.
        BigInteger t(0);
        for (std::size_t i = start; i < value.size(); ++i) {
            t.multiply(radix);
            if (std::isdigit(value[i])) {
                t.add(value[i] - '0');
            } else {
                t.add(value[i] - 'a' + 10);
            }
        }

        // The sign.
        if (start == 1) {
            t.negate();
        }

        // Swap with the created number t.
        swap(t);
    }

    /**
     * Move constructor - technical C++ stuff.
     */
    BigInteger(BigInteger && b):
        digits(0),
        positive(true),
        length(0) {
        swap(b);
    }

    /**
     * Random number less than or equal to max and greater than (or equal to) zero.
     *
     * @param allowZero If false we do not allow zero.
     */
    static BigInteger random(const BigInteger & max, bool allowZero = true) {
        BigInteger t(0);

        // Set the bit length.
        t.setBitLength(max.length);
        do {
            // Generate random bits.
            for (std::size_t i = 0; i < max.length; ++i) {
                t.digits[i] = rand() % 2;
            }
            // Allow only the allowed max => t >(=) 0.
        } while (t.compare(max) > 0 || (!allowZero && t.compare(0) == 0));

        // Normalize.
        t.normalize();
        return t;
    }

    /**
     * Random number from min to max, inclusive.
     */
    static BigInteger random(const BigInteger& min, const BigInteger& max) {
        BigInteger t(0);

        // Set the bit length.
        t.setBitLength(max.length);
        do {
            // Generate random bits.
            for (std::size_t i = 0; i < max.length; ++i) {
                t.digits[i] = rand() % 2;
            }
            // Allow only the allowed min <= t <= max.
        } while (t.compare(min) < 0 || t.compare(max) > 0);

        // Normalize.
        t.normalize();
        return t;
    }

    /**
     * Random number with specified number of bits.
     *
     * @param allowZero If false we do not allow zero.
     * @param allowMsbZero If false we do not allow the most significant bit to be zero.
     */
    static BigInteger randomBitL(const size_t bits, bool allowZero = true, bool allowMsbZero = true) {
        BigInteger t(0);

        // Set the bit length.
        t.setBitLength(bits);
        do {
            if(allowMsbZero) {
                t.digits[bits-1] = rand() % 2;
            } else {
                // Set MSB = 1 to guarantee the bit length for normalized t
                t.digits[bits-1] = 1;
            }
            // Generate random bits.
            for (std::size_t i = 0; i < bits-1; ++i) {
                t.digits[i] = rand() % 2;
            }
            // Allow only the allowed max > t >(=) 0.
        } while (!allowZero && t.compare(0) == 0);

        // Normalize.
        t.normalize();
        return t;
    }

    /**
     * Returns the BigInteger which is a power of two - 2 ^ power.
     */
    static BigInteger power2(std::size_t power) {
        BigInteger p(1);
        p.shiftLeft(power);
        return p;
    }

    /**
     * Destructor.
     */
    ~BigInteger(void) {
        delete [] digits;
        digits = 0;
        length = 0;
        positive = true;
    }

    /**
     * Copy constructor.
     */
    BigInteger(const BigInteger & a):
        digits(new char [a.length]),
        positive(a.positive),
        length(a.length) {
        for (std::size_t i = 0; i < length; ++i) {
            digits[i] = a.digits[i];
        }
    }

    /**
     * Assignment operator.
     */
    BigInteger & operator=(const BigInteger & a) {
        BigInteger b(a);
        b.swap(*this);
        return *this;
    }

    /**
     * Bit length retrieval.
     */
    std::size_t getLength() const {
        return length;
    }

    /**
     * Non negativity flag - just return positive.
     */
    bool isNonNegative() const {
        return positive;
    }

    /**
     * Swaps the two numbers.
     */
    BigInteger& swap(BigInteger & a) {
        using std::swap;

        swap(a.digits, this->digits);
        swap(a.length, this->length);
        swap(a.positive, this->positive);
        return *this;
    }

    // Operations
    /**
     * Adds the value of a to this number.
     */
    BigInteger& add(const BigInteger & a) {
        // Enlarge by one - just in case it is necessary.
        // Sum of two n-bit numbers can have n + 1 bits.
        setBitLength(std::max(this->length, a.length) + 1);
        char carry = 0;
        char sum = 0;

        // Add the shorter part.
        for (std::size_t i = 0; i < a.length; ++i) {

            sum = (carry + digits[i] + a.digits[i]);
            carry = sum / 2;
            digits[i] = sum % 2;
        }

        // Add the remaining part.
        for (std::size_t i = a.length; i < this->length; ++i) {
            sum = (carry + digits[i] + (a.positive ? 0 : 1));
            carry = sum / 2;
            digits[i] = sum % 2;
        }

        // What to do with the carry - use it for sign detection?
        /*
         * CASE 1 positive + positive (no carry possible - the added leading bits make no carry)
         *   11 (= 3)
         * + 11
         * ----
         *  110
         *
         * CASE 2 negative + negative (explicitly written the leading bits - it can be safely ignored), the added
         * leading bits generate it.
         *   111 (=-1)
         * + 100 (=-4)
         * -----
         *   011 =-(100 + 1 = 101 = 5)=-5
         *
         * CASE 3 negative + positive and negative result
         *   100 = -4 (the added bit is 1 therefore negative)
         * + 011 = 3
         * -----
         *   111 = -1 (the first bit is 1) and no carry
         *
         * CASE 4 negative + positive and negative result
         *   1100 = -4
         * + 0111 = 7
         *  -----
         *   0011 = 3 and the first bit is 0 but we have a carry
         *   (notice that the carry would propagate if we had a greater bit length)
         */

        // In case of positive vs. negative number it determines the sign of the result.
        // In case of the same sign it can be safely ignored.
        if (this->positive ^ a.positive) {
            // We deduce the sign of the number using the carry.
            this->positive = (carry == 1);
        }

        // Else we can ignore it and the sign of the number is not changed.
        // Normalization - get the number of bits as small as possible.
        normalize();
        return *this;
    }

    /**
     * Switches the sign of this number - using the twos complement.
     */
    BigInteger& negate(void) {

        // We can not represent 0 in two's complement, we would lose the information about zeroness.
        if (compare(0) == 0) {
            return *this;
        }

        // Change the flag.
        positive = !positive;

        // Do the complementing thing and add the carry.
        char carry = 1, sum;

        for (std::size_t i = 0; i < length; ++i) {
            sum = 1 - digits[i] + carry;
            digits[i] = sum % 2;
            carry = sum / 2;
        }

        // And the last trick ;).
        // If the number becomes positive and the previous number was e.g. -4 -> -100 -> 011 + 1 -> 100 -> 00 after
        // normalization we have to change the bits to 11 and add +1 - we get 00 and carry +1 which can not be ignored.
        // The problem is that we do not have a fixed bit length and we need to be careful since -2 in twos complement
        // is represented as -2 = -10 = 01 + 1 = 10 = 1111110 or just a simple 0 after normalization.

        // For example 5 = 101, -5 = 011 (in twos complement) and switching to +5 is easy 100 + 1 and there is no carry.
        if ((carry == 1) & positive) {
            setBitLength(length + 1);
            digits[length - 1] = 1;
        }

        return *this;
    }

    /**
     * Subtraction.
     */
    BigInteger& subtract(const BigInteger & a) {
        BigInteger b(a);
        b.negate();
        add(b);
        return *this;
    }

    /**
     * Multiplication.
     */
    BigInteger& multiply(const BigInteger & f) {

        BigInteger fAbs;
        if (!f.positive) {
            negate();

            fAbs = f;
            fAbs.negate();
        } else {
            fAbs = f;
        }

        fAbs.normalize();

        BigInteger original(0);
        swap(original);

        for (std::size_t i = fAbs.length - 1; i != (size_t) -1; --i) {
            shiftLeft(1);

            if (fAbs.digits[i] == 1) {
                add(original);
            }
        }

        return *this;
    }

    /**
     * Fast exponentiation - the iterative way.
     */
    BigInteger& power(BigInteger k) {

        BigInteger result = 1;
        if (!k.positive) {
            throw std::runtime_error("Can only power to a non-negative number.");
        }
        k.normalize();

        for (std::size_t i = k.length - 1; i != (size_t) -1; --i) {
            result.multiply(result);

            if (k.digits[i] == 1) {
                result.multiply(*this);
            }
        }

        swap(result);
        return *this;
    }


    /**
     * Fast exponentiation using the modulo operation.
     */
    BigInteger& powerMod(BigInteger k, const BigInteger & m) {
        BigInteger result = 1;
        if (!k.positive) {
            throw std::runtime_error("Can only power to a non-negative number.");
        }
        k.normalize();

        for (std::size_t i = k.length - 1; i != (size_t) -1; --i) {

            result.multiply(result);
            result.mod(m);

            if (k.digits[i] == 1) {
                result.multiply(*this);
                result.mod(m);

            }
        }

        swap(result);
        return *this;
    }

    /**
     * Modulo operation.
     *
     * this becomes this % b.
     */
    BigInteger& mod(const BigInteger & b);

    /**
     * Integer division.
     *
     * this becomes this / b.
     */
    BigInteger& divide(const BigInteger & b);

    /**
     * Computes the absolute value.
     */
    BigInteger& abs() {
        if (!positive) {
            negate();
        }
        return *this;
    }

    /**
     * Shifts the bits to the right.
     */
    BigInteger& shiftRight(std::size_t s) {
        if (s < length) {
            for (std::size_t i = 0; i != length - s; ++i) {
                digits[i] = digits[i + s];
            }
        } else {
            s = length;
        }

        for (std::size_t i = length - s; i != length; ++i) {
            digits[i] = (positive ? 0 : 1);
        }

        normalize();
        return *this;
    }

    /**
     * Shifts the bits to the left.
     */
    BigInteger& shiftLeft(std::size_t s) {
        normalize();

        setBitLength(length + s);
        for (std::size_t i = length - 1; i != s - 1; --i) {
            digits[i] = digits[i - s];
        }

        for (std::size_t i = s - 1; i != (std::size_t) - 1; --i) {
            digits[i] = 0;
        }
        return *this;
    }

    /**
     * Zero test.
     */
    bool isZero(void) const {
        return 0 == compare(BigInteger(0));
    }

    /**
     * Parity test - even numbers.
     */
    bool isEven(void) const {
        return (1 == digits[0]) ^ positive;
    }

    /**
     * Parity test - odd numbers.
     */
    bool isOdd(void) const {
        return !isEven();
    }

    /**
     * Comparison.
     *
     * returns -1 if this < b
     * return 0 if this == b
     * return 1 if this > b
     */
    int compare(const BigInteger & b) const {


        // I am greater than zero and b is not.
        if (positive and !b.positive) {
            return 1;
        }

        // I am less than zero and b is greater.
        if (!positive and b.positive) {
            return -1;
        }

        // Make them both positive.
        if (!positive) {
            BigInteger abs(*this);
            abs.negate();
            BigInteger bAbs(b);
            bAbs.negate();

            int res = -abs.compare(bAbs);

            return res;
        }


        // Compare the positive numbers, without normalization.
        size_t tD;
        size_t bD;

        // Go from the significant bits to the left. If they differ return the appropriate result.
        for (std::size_t i = std::max(length, b.length) - 1; i != (size_t) - 1; --i) {

            // If i-th digit is greater then the bit length fill it with zero. We are positive.
            if (i >= length) {
                tD = 0;
            } else {
                tD = digits[i];
            }

            // The same for b.
            if (i >= b.length) {
                bD = 0;
            } else {
                bD = b.digits[i];
            }


            // Compare the bits.
            if (tD < bD) {
                return -1;
            } else if (tD > bD) {
                return 1;
            }
        }

        // Fine we got here - the numbers are the same.
        return 0;
    }

    /**
     * Conversion to int.
     */
    int intValue(void) const {
        // Set it up.
        int value = positive ? 0 : -1;

        // Just copy it ;) using shifts.
        for (std::size_t i = length - 1; i != (std::size_t) -1; --i) {
            value <<= 1;
            value |= digits[i];
        }

        return value;
    }

    /**
     * Conversion to string.
     */
    std::string toString(char radix = 10) const;

    /**
     * Conversion to an array having the digits of the specified radix. The most significant digit is the first one in
     * the array.
     */
    std::vector<std::size_t> toRadix(std::size_t radix) const;

private:
    /**
     * Normalize the number.
     */
    BigInteger& normalize(void) {
        // Removes the starting zeros (for positive) or ones (for negative) numbers.
        std::size_t newLength = length;
        for (std::size_t i = length - 1; i != (std::size_t) -1; --i) {
            if (digits[i] == (positive ? 0 : 1)) {
                --newLength;
            } else {
                break;
            }
        }

        setBitLength(newLength);
        return *this;
    }

    /**
     * Set the bit length of the number.
     */
    BigInteger& setBitLength(std::size_t newLength) {
        if (newLength == length) {
            return *this;
        }

        // New array.
        char * newDigits = new char [newLength];
        std::copy(digits, digits + std::min(length, newLength), newDigits);
        /*for (std::size_t i = 0; i < std::min(length, newLength); ++i) {
        	newDigits[i] = digits[i];
        }*/

        // Pad according to the positive/negative status.
        for (std::size_t i = length; i < newLength; ++i) {
            newDigits[i] = (positive ? 0 : 1);
        }

        // Release the old array.
        delete [] digits;
        digits = newDigits;

        // Remember the new length.
        length = newLength;

        return *this;
    }

    std::string rawBits(void) const {
        std::stringstream str;
        for (size_t i = length - 1; i != (size_t) -1; --i) {
            str << (std::size_t) digits[i];
        }
        return str.str();
    }

    /**
     * Digit array.
     */
    char * digits;

    /**
     * Positivity flag.
     */
    bool positive;

    /**
     * Length of the number.
     */
    std::size_t length;
};

/**
 * Result of the div and mod operation - we just implement them together.
 */
struct DivisionResult {
    BigInteger quotient;
    BigInteger remainder;
};

/**
 * Division and modulo according to the Algorithms textbook.
 *
 * Just for a >= 0, b > 0.
 */
void divideAndMod(BigInteger & a, const BigInteger & b, DivisionResult & res) {

    // We do not know ho to divide by zero.
    if (b.isZero()) {
        throw std::runtime_error("Division by zero.");
    }

    // This is the basic case - 0/x = 0x + 0.
    if (a.isZero()) {
        res.remainder = BigInteger(0);
        res.quotient = BigInteger(0);

    } else {
        // We divide by two.
        bool odd = a.isOdd();


        // "Divide" by two and compute the result.
        a.shiftRight(1);

        divideAndMod(a, b, res);


        // Back multiply by two.
        res.quotient.shiftLeft(1);
        res.remainder.shiftLeft(1);


        // Do not forget adding the remainder if a was odd.
        if (odd) {
            res.remainder.add(1);
        }

        // If the remainder is large - add one to the quotient.
        if (res.remainder.compare(b) >= 0) {
            res.remainder.subtract(b);
            res.quotient.add(BigInteger(1));
        }

    }
}

/**
 * Modulo operation.
 *
 * this becomes this % b.
 */
BigInteger& BigInteger::mod(const BigInteger & b) {
    // If b is negative, then a % b = a % (-b) + b if a % (-b) > 0, and a % (-b) otherwise
    if (!b.positive) {
        BigInteger bAbs(b);
        bAbs.negate();

        mod(bAbs);
        if(compare(BigInteger(0)) != 0) {
            add(b);
        }
        return *this;
    }

    // If a is negative, then a % b = (b - (-a % b)) % b if -a % b > 0, and -a % b otherwise
    if (!positive) {
        BigInteger abs(*this);
        abs.negate();

        abs.mod(b);
        if(abs.compare(BigInteger(0)) != 0) {
            abs.negate();
            abs.add(b);
        }
        swap(abs);

        return *this;
    }

    DivisionResult res;
    divideAndMod(*this, b, res);
    swap(res.remainder);
    return *this;
}

/**
 * Division operation.
 *
 * this becomes this / b.
 */
BigInteger& BigInteger::divide(const BigInteger & b) {
    // When b is not positive - negate, divide and negate back.
    if (!b.positive) {
        BigInteger bAbs(b);
        bAbs.negate();

        divide(bAbs);
        negate();
        return *this;
    }

    // When this is not positive - negate, divide and negate back.
    if (!positive) {
        BigInteger abs(*this);
        abs.negate();

        abs.divide(b);
        abs.negate();
        swap(abs);
        return *this;
    }

    // Do the division.
    DivisionResult res;
    divideAndMod(*this, b, res);
    swap(res.quotient);
    return *this;
}

std::vector<std::size_t> BigInteger::toRadix(std::size_t radix) const {
    std::vector<std::size_t> res;
    BigInteger d = *this;
    if (!d.positive) {
        d.negate();
    }
    d.normalize();

    if (d.length == 0) {
        res.push_back(0);
    } else {
        std::stack<size_t> digits;
        DivisionResult divRes;
        while (d.compare(0) > 0) {
            divideAndMod(d, radix, divRes);
            d = divRes.quotient;
            digits.push((size_t) divRes.remainder.intValue());
        }

        while (digits.size()) {
            res.push_back(digits.top());
            digits.pop();
        }
    }

    return res;
}

// Technical C++ stuff.

BigInteger operator-(BigInteger & a) {
    BigInteger aC(a);
    aC.negate();
    return aC;
}

BigInteger & operator+=(BigInteger & a, const BigInteger & b) {
    a.add(b);
    return a;
}

BigInteger operator+(const BigInteger & a, const BigInteger & b) {
    BigInteger aCpy = a;
    aCpy += b;
    return aCpy;
}

BigInteger & operator-=(BigInteger & a, const BigInteger & b) {
    a.subtract(b);
    return a;
}

BigInteger operator-(const BigInteger & a, const BigInteger & b) {
    BigInteger aCpy = a;
    aCpy -= b;
    return aCpy;
}

BigInteger & operator*=(BigInteger & a, const BigInteger & b) {
    a.multiply(b);
    return a;
}

BigInteger operator*(const BigInteger & a, const BigInteger & b) {
    BigInteger aCpy = a;
    aCpy *= b;
    return aCpy;
}

BigInteger & operator/=(BigInteger & a, const BigInteger & b) {
    a.divide(b);
    return a;
}

BigInteger operator/(const BigInteger & a, const BigInteger & b) {
    BigInteger aCpy = a;
    aCpy /= b;
    return aCpy;
}

BigInteger & operator%=(BigInteger & a, const BigInteger & b) {
    a.mod(b);
    return a;
}

BigInteger operator%(const BigInteger & a, const BigInteger & b) {
    BigInteger aCpy = a;
    aCpy %= b;
    return aCpy;
}

BigInteger& operator++(BigInteger& a) { // prefix
    a.add(1);
    return a;
}
BigInteger operator++(BigInteger& a, int) { // postfix
    BigInteger result = a;
    ++a; // call operator++(a)
    return result;
}

BigInteger& operator--(BigInteger& a) { // prefix
    a.subtract(1);
    return a;
}
BigInteger operator--(BigInteger& a, int) { // postfix
    BigInteger result = a;
    --a; // call operator--(a)
    return result;
}

bool operator>(const BigInteger & a, const BigInteger & b) {
    return a.compare(b) > 0;
}

bool operator>=(const BigInteger & a, const BigInteger & b) {
    return a.compare(b) >= 0;
}

bool operator==(const BigInteger & a, const BigInteger & b) {
    return a.compare(b) == 0;
}

bool operator!=(const BigInteger & a, const BigInteger & b) {
    return a.compare(b) != 0;
}

bool operator<(const BigInteger & a, const BigInteger & b) {
    return a.compare(b) < 0;
}

bool operator<=(const BigInteger & a, const BigInteger & b) {
    return a.compare(b) <= 0;
}

std::string BigInteger::toString(char radix) const {
    char digitMap [] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    std::stringstream ss;

    BigInteger d = *this;
    if (!d.positive) {
        ss << "-";
    }

    std::vector<std::size_t> digits = toRadix(radix);
    for (std::size_t i = 0; i < digits.size(); ++i) {
        ss << digitMap[digits[i]];
    }

    return ss.str();
}

std::size_t getRadix(std::ios_base & stream) {
    std::ios_base::fmtflags flags = stream.flags();
    size_t radix = 10;
    if (flags & stream.hex) {
        radix = 16;
    } else if (flags & stream.binary) {
        radix = 2;
    }

    return radix;
}

std::ostream & operator<<(std::ostream & out, const BigInteger & a) {
    return out << a.toString(getRadix(out));
}

std::istream & operator>>(std::istream & in, BigInteger & b) {
    std::string str;
    in >> str;
    BigInteger a(str, getRadix(in));
    b.swap(a);
    return in;
}

BigInteger& operator<<(BigInteger& d, std::size_t s) {
    d.shiftLeft(s);
    return d;
}

BigInteger& operator>>(BigInteger& d, std::size_t s) {
    d.shiftRight(s);
    return d;
}

#endif
