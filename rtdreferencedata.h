#ifndef RTDREFERENCEDATA_H
#define RTDREFERENCEDATA_H

#include <QMap>
#include <QPair>

// IEC 60751 Reference Table for Pt100 (α = 0.00385)
// Temperature in °C, Resistance in Ω
namespace RTDReferenceData {

struct TablePoint {
    double temperature;
    double resistance;
};

// IEC 60751 Reference Table for Pt100 (R0 = 100Ω, α = 0.00385)
// Values at 10°C intervals
static const TablePoint PT100_IEC60751[] = {
    {-200, 18.52}, {-190, 22.83}, {-180, 27.10}, {-170, 31.34}, {-160, 35.54},
    {-150, 39.72}, {-140, 43.88}, {-130, 48.00}, {-120, 52.11}, {-110, 56.19},
    {-100, 60.26}, {-90, 64.30}, {-80, 68.33}, {-70, 72.33}, {-60, 76.33},
    {-50, 80.31}, {-40, 84.27}, {-30, 88.22}, {-20, 92.16}, {-10, 96.09},
    {0, 100.00}, {10, 103.90}, {20, 107.79}, {30, 111.67}, {40, 115.54},
    {50, 119.40}, {60, 123.24}, {70, 127.08}, {80, 130.90}, {90, 134.71},
    {100, 138.51}, {110, 142.29}, {120, 146.07}, {130, 149.83}, {140, 153.58},
    {150, 157.33}, {160, 161.05}, {170, 164.77}, {180, 168.48}, {190, 172.17},
    {200, 175.86}, {210, 179.53}, {220, 183.19}, {230, 186.84}, {240, 190.47},
    {250, 194.10}, {260, 197.71}, {270, 201.31}, {280, 204.90}, {290, 208.48},
    {300, 212.05}, {310, 215.61}, {320, 219.15}, {330, 222.68}, {340, 226.21},
    {350, 229.72}, {360, 233.21}, {370, 236.70}, {380, 240.18}, {390, 243.64},
    {400, 247.09}, {410, 250.53}, {420, 253.96}, {430, 257.38}, {440, 260.78},
    {450, 264.18}, {460, 267.56}, {470, 270.93}, {480, 274.29}, {490, 277.64},
    {500, 280.98}, {510, 284.30}, {520, 287.62}, {530, 290.92}, {540, 294.21},
    {550, 297.49}, {560, 300.76}, {570, 304.02}, {580, 307.27}, {590, 310.51},
    {600, 313.73}, {610, 316.95}, {620, 320.16}, {630, 323.35}, {640, 326.54},
    {650, 329.71}, {660, 332.87}, {670, 336.02}, {680, 339.16}, {690, 342.29},
    {700, 345.41}, {710, 348.51}, {720, 351.61}, {730, 354.70}, {740, 357.77},
    {750, 360.84}, {760, 363.89}, {770, 366.94}, {780, 369.97}, {790, 373.00},
    {800, 376.01}, {810, 379.02}, {820, 382.01}, {830, 385.00}, {840, 387.97},
    {850, 390.94}
};

static const int PT100_IEC60751_SIZE = sizeof(PT100_IEC60751) / sizeof(TablePoint);

// Helper function to get interpolated resistance from table
inline double getTableResistance(double temperature, double r0Factor = 1.0) {
    if (temperature < PT100_IEC60751[0].temperature ||
        temperature > PT100_IEC60751[PT100_IEC60751_SIZE - 1].temperature) {
        return -1.0; // Out of range
    }

    // Find bracketing points
    for (int i = 0; i < PT100_IEC60751_SIZE - 1; i++) {
        if (temperature >= PT100_IEC60751[i].temperature &&
            temperature <= PT100_IEC60751[i + 1].temperature) {
            // Linear interpolation
            double t1 = PT100_IEC60751[i].temperature;
            double t2 = PT100_IEC60751[i + 1].temperature;
            double r1 = PT100_IEC60751[i].resistance;
            double r2 = PT100_IEC60751[i + 1].resistance;

            double ratio = (temperature - t1) / (t2 - t1);
            double resistance = r1 + ratio * (r2 - r1);

            return resistance * r0Factor; // Scale for Pt500, Pt1000, etc.
        }
    }

    return -1.0;
}

// Get table points for plotting (every 10°C or custom step)
inline QVector<QPair<double, double>> getTablePoints(double minTemp, double maxTemp,
                                                      double r0Factor = 1.0, double step = 10.0) {
    QVector<QPair<double, double>> points;

    for (double t = minTemp; t <= maxTemp; t += step) {
        double r = getTableResistance(t, r0Factor);
        if (r > 0) {
            points.append(qMakePair(t, r));
        }
    }

    return points;
}

} // namespace RTDReferenceData

#endif // RTDREFERENCEDATA_H
