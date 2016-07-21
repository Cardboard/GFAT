#include <string>

#ifndef DATE_H
#define DATE_H

typedef struct Date {
    int day;
    int month;
    int year;
    unsigned ymd;

    Date(int y, int m, int d) {
        year = y;
        month = m;
        day = d;
        ymd = stoi(to_string(y) + to_string(m) + to_string(d));
    }
    Date() {
    }
} Date;

#endif // DATE_H
