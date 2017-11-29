#include <iostream>
#include "powerMod_incomplete.cpp"

using namespace std;

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
