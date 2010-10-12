/* 
 * File:   main.cpp
 * Author: magnusekm
 *
 * Created on October 11, 2010, 10:33 PM
 */
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <stack>
#include "GraphStructure.h"
#include <queue>

using namespace std;

void printMatrix(bool** matrix, int num) {
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            cout << matrix[i][j];
        }
        cout << "\n";
    }
}

vector<int> getNeighbours(bool * line, int num) {
    vector<int> neighbours;
    for (int i = 0; i < num; ++i) {
        if (line[i]) {
            neighbours.push_back(i);
        }
    }
    return neighbours;
}

bool isBipartial(bool ** matrix, int num) {
    int i;
    int u;
    int v;
    int start;
    queue< int > q;
    start = 0; // nodes labeled from 1
    q.push(start);
    int partition[num];
    int visited[num];
    partition[start] = 1; // 1 left, 2 right
    visited[start] = 1; // set gray

    while (!q.empty()) {
        u = q.front();
        q.pop();
        vector<int> neighbours = getNeighbours(matrix[u], num);
        for (i = 0; i < neighbours.size(); i++) {
            v = neighbours[i];
            if (partition[u] == partition[v]) return false;
            if (visited[v] == 0) {
                visited[v] = 1;
                partition[v] = 3 - partition[u]; // alter 1 and 2
                q.push(v);
            }
        }
    }
    return true;
}

bool isOne(char value) {
    return value == '1';
}

bool ** getMatrixCopy(bool ** original, int num) {
    bool ** matrix = (bool**) malloc(num * sizeof (bool*));
    bool * temp = (bool*) malloc(num * num * sizeof (bool));
    for (int i = 0; i < num; i++) {
        matrix[i] = temp + (i * num);
        for (int j = 0; j < num; ++j) {
            matrix[i][j] = original[i][j];
        }
    }
    return matrix;
}

bool **removeEdge(bool**original, int edgePosition, int num) {
    bool ** matrix = getMatrixCopy(original, num);
    int edgesCount = 0;
    for (int i = 0; i < num; ++i) {
        for (int j = i + 1; j < num; ++j) {
            if (matrix[i][j]) {
                ++edgesCount;
                if (edgePosition == edgesCount) {
                    matrix[i][j] = 0;
                    matrix[j][i] = 0;
                    return matrix;
                }
            }
        }
    }
    cout << "How the fuck did you get here ???";
}

int main(int argc, char** argv) {
    string str;
    ifstream inFile;

    inFile.open("graf.out");
    if (!inFile) {
        cerr << "Unable to open file graf.out";
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


    int edgesCount = 0;
    int line = 0;
    while (getline(inFile, str)) {
        for (int pos = 0; pos < num; ++pos) {
            char value = str.at(pos);
            bool one = isOne(value);
            matrix[line][pos] = one;

            if ((pos > line) && one) {
                ++edgesCount;
            }
        }
        line++;
    }
    inFile.close();

    stack <GraphStructure *> matrices;
    GraphStructure *winner;
    matrices.push(new GraphStructure(matrix, edgesCount));

    bool winnerSet = false;
    while (!matrices.empty()) {
        GraphStructure * gs;
        gs = matrices.top();
        matrices.pop();
        if (!isBipartial(gs->getMatrix(), num)) {
            if (!winnerSet || gs->getEdgesCount() > winner->getEdgesCount() + 1) {
                for (int i = 0; i < gs->getEdgesCount(); ++i) {
                    matrices.push(new GraphStructure(
                            removeEdge(gs->getMatrix(), i + 1, num), gs->getEdgesCount() - 1));
                }
                delete gs;
            }
        } else {
            if (!winnerSet) {
                winnerSet = true;
                winner = gs;
            } else if (winner->getEdgesCount() < gs->getEdgesCount()) {
                winner = gs;
            }
            printf("bipartial with edges: %d\n", gs->getEdgesCount());
            delete gs;
        }
    }
    printf("all edges count: %d\n", edgesCount);

    return 0;
}

