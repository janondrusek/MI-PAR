/* 
 * File:   main.cpp
 * Author: magnusekm
 *
 * Created on October 11, 2010, 10:33 PM
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

static bool **malloc2(int s1, int s2) {
    bool ** matrix = (bool **)malloc(s1 * sizeof (bool*));
    for (int i = 0; i < s1; i++)
        matrix[i] = (bool *)malloc(s2 * sizeof (bool));
    return matrix;
}

int main(int argc, char** argv) {
    string str;
    ifstream inFile;
    inFile.open("graf.out");
    if (!inFile) {
        cerr << "Unable to open file datafile.txt";
        exit(1); // call system to stop
    }
    getline(inFile, str);
    istringstream is(str);
    int num;
    is >> num;

    // declaration
    bool** matrix;
    bool* temp;

    matrix = (bool**) malloc(num * sizeof (bool*));
    temp = (bool*) malloc(num * num * sizeof (bool));
    for (int i = 0; i < num; i++) {
        matrix[i] = temp + (i * num);
    }


    int line = 0;
    while (getline(inFile, str)) {
        for (int pos = 0; pos < num; ++pos) {
            char value = str.at(pos);
            matrix[line][pos] = value == '1';
        }
        line++;
    }

    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            cout << matrix[i][j];
        }
        cout << "\n";
    }


    inFile.close();



    return 0;
}

