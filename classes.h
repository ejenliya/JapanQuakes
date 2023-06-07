#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include "helper.h"

using namespace std;


uint64_t secInHour = 60 * 60;
uint64_t secInDay = secInHour * 24;
uint64_t secInMonth = secInDay * 30;
uint64_t secInYear = secInMonth * 12;

struct GeoPoints {
    int lon;
    int lat;

    GeoPoints(int f, int s) : lon(f), lat(s) {};
};

struct Earthquake {
    string year;
    string month;
    string day;
    string utc;
    uint64_t dateTime;
    float lon;
    float lat;
    int mag;

    Earthquake(string y, string mo, string d, const string& t, float lo, float la, int m)
            : year(y), month(mo), day(d), utc(t), lon(lo), lat(la), mag(m) {
        dateTime = stoi(t) % 100 // seconds
                   + 60 * (((stoi(t) % 10000) / 100))  // minutes
                   + secInHour * ((stoi(t) / 10000))
                   + secInDay * (stoi(day) )
                   + secInMonth * (stoi(month))
                   + secInYear * stoi(year);
    };
};

bool operator<(const Earthquake &a, const Earthquake &b) {
    return a.dateTime < b.dateTime;
}
