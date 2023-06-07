#pragma once

#include <GL/gl.h>
#include <climits>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <iostream>
#include <fstream>
#include <array>
#include "classes.h"

using namespace std;

string text = "magnitude > 3";
vector<GeoPoints> geoPoints;
array<int, 4> canvasSize{INT_MAX, -1, INT_MAX, -1};
set<Earthquake> earthquakes;
int W_SIZE_X = 750;
int W_SIZE_Y = 750;
bool timeGoesBack;
bool timeGoesForward;
bool timeIsStopped;
bool prevF;
bool prevB;
int curMag = 3;
double zoomTop = 1;
double zoomBottom = 1;
double zoomRight = 1;
double zoomLeft = 1;
double zoomStep = 0.5;
uint64_t timeChange = 0;
double markerTransform = 0.03;
uint64_t startTimer = 0;
uint64_t endTimer = ULLONG_MAX;

void zoom(bool in) {
    int mult = in ? 1 : -1;
    zoomTop += zoomStep * mult;
    zoomLeft += zoomStep * mult;
    zoomBottom += zoomStep * mult;
    zoomRight += zoomStep * mult;
}

void animSet(bool forward, bool back, bool pause) {
    timeGoesForward = forward;
    timeGoesBack = back;
    timeIsStopped = pause;
}

string trim(string &&s) {
    auto it = s.begin();
    while (*it == ' ') {
        ++it;
    }
    auto it2 = s.end();
    while (*it2 == ' ') {
        --it2;
    }
    return string(it, it2);
}

vector<string> parse(const string &s, vector<string> &v) {
    auto it = find(s.begin(), s.end(), ',');
    if (it != s.end()) {
        string tmp = trim(string(s.begin(), it));
        v.push_back(tmp);
        parse(string(it + 1, s.end()), v);
    } else {
        s.begin() = s.end();
    }
    return v;
}

bool containsAlpha(const string &s) {
    return any_of(s.begin(), s.end(), ::isalpha);
}

void readJapanMap() {
    fstream japanMap;
    japanMap.open(R"(/home/ejenliya/unik/CG/RGR/RGR/japanMap.txt)", ios::in);
    if (japanMap.is_open()) {
        string tp;
        while (getline(japanMap, tp)) {
            if (!containsAlpha(tp)) {
                string lat = tp.substr(0, tp.find(','));
                string lon = tp.substr(tp.find(' ') + 1, tp.size());
                geoPoints.emplace_back(stoi(lat), stoi(lon));
            }
        }
        japanMap.close();
    } else cerr << "Cannot open file!\n";
}

void setUpWindow() {
    for (auto &e: geoPoints) {
        if (e.lat < canvasSize[0]) canvasSize[0] = e.lat;
        if (e.lat > canvasSize[1]) canvasSize[1] = e.lat;
        if (e.lon < canvasSize[2]) canvasSize[2] = e.lon;
        if (e.lon > canvasSize[3]) canvasSize[3] = e.lon;
    }
}

void setDefault() {
    startTimer = 0;
    endTimer = ULLONG_MAX;
    animSet(false, false, false);
}

void showEarthQuake(const Earthquake &e) {
    double markerSize = markerTransform * e.mag;
    glColor3f(1, float (8 - e.mag) / 5, 0);
    glVertex2f(e.lat, e.lon + markerSize);
    glVertex2f(e.lat, e.lon - markerSize);
    glVertex2f(e.lat - markerSize, e.lon);
    glVertex2f(e.lat + markerSize, e.lon);
}

void readQuakesMap() {
    fstream quakesMap;
    quakesMap.open(R"(/home/ejenliya/unik/CG/RGR/RGR/quakesMap.txt)", ios::in);
    if (quakesMap.is_open()) {
        string tp;
        while (getline(quakesMap, tp)) {
            if (!containsAlpha(tp)) {
                vector<string> v;
                v = parse(tp, v);
                if (!v[6].empty() && stod(v[6]) >= 3) {
                    earthquakes.emplace(v[0], v[1], v[2], v[3], stod(v[4]), stod(v[5]), stod(v[6]));
                }
            }
        }
        quakesMap.close();
        timeChange = (((--earthquakes.end())->dateTime + secInMonth) - earthquakes.begin()->dateTime) / 54391.6;
    } else cerr << "Cannot open file!\n";
}
