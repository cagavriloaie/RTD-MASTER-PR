#ifndef THERMOCOUPLEREFERENCEDATA_H
#define THERMOCOUPLEREFERENCEDATA_H

#include <QMap>
#include <QPair>
#include <QVector>
#include "thermocouplecalculator.h"

// NIST ITS-90 Reference Tables for Thermocouples
// Temperature in °C, Voltage in mV
namespace ThermocoupleReferenceData {

struct TablePoint {
    double temperature;
    double voltage;
};

// Type K (Chromel-Alumel) - NIST ITS-90
static const TablePoint TYPE_K_NIST[] = {
    {-200, -5.891}, {-150, -4.913}, {-100, -3.554}, {-50, -1.889},
    {0, 0.000}, {50, 2.023}, {100, 4.096}, {150, 6.138}, {200, 8.138},
    {250, 10.153}, {300, 12.209}, {350, 14.293}, {400, 16.397}, {450, 18.516},
    {500, 20.644}, {550, 22.776}, {600, 24.905}, {650, 27.025}, {700, 29.129},
    {750, 31.213}, {800, 33.275}, {850, 35.313}, {900, 37.326}, {950, 39.314},
    {1000, 41.276}, {1050, 43.211}, {1100, 45.119}, {1150, 46.995}, {1200, 48.838},
    {1250, 50.644}, {1300, 52.410}, {1350, 54.138}, {1372, 54.845}
};

// Type J (Iron-Constantan) - NIST ITS-90
static const TablePoint TYPE_J_NIST[] = {
    {-200, -7.890}, {-150, -6.500}, {-100, -4.633}, {-50, -2.431},
    {0, 0.000}, {50, 2.585}, {100, 5.269}, {150, 8.010}, {200, 10.779},
    {250, 13.555}, {300, 16.327}, {350, 19.090}, {400, 21.846}, {450, 24.610},
    {500, 27.388}, {550, 30.188}, {600, 33.013}, {650, 35.863}, {700, 38.735},
    {750, 41.625}, {800, 44.527}, {850, 47.431}, {900, 50.332}, {950, 53.224},
    {1000, 56.101}, {1050, 58.960}, {1100, 61.796}, {1150, 64.606}, {1200, 67.386}
};

// Type T (Copper-Constantan) - NIST ITS-90
static const TablePoint TYPE_T_NIST[] = {
    {-200, -5.603}, {-150, -4.648}, {-100, -3.379}, {-50, -1.819},
    {0, 0.000}, {50, 2.036}, {100, 4.279}, {150, 6.702}, {200, 9.288},
    {250, 12.013}, {300, 14.862}, {350, 17.819}, {400, 20.869}
};

// Type E (Chromel-Constantan) - NIST ITS-90
static const TablePoint TYPE_E_NIST[] = {
    {-200, -8.825}, {-150, -7.279}, {-100, -5.237}, {-50, -2.787},
    {0, 0.000}, {50, 3.048}, {100, 6.319}, {150, 9.789}, {200, 13.421},
    {250, 17.181}, {300, 21.036}, {350, 24.964}, {400, 28.946}, {450, 32.965},
    {500, 37.005}, {550, 41.053}, {600, 45.093}, {650, 49.116}, {700, 53.112},
    {750, 57.073}, {800, 60.990}, {850, 64.858}, {900, 68.667}, {950, 72.413},
    {1000, 76.092}
};

// Type N (Nicrosil-Nisil) - NIST ITS-90
static const TablePoint TYPE_N_NIST[] = {
    {-200, -3.990}, {-150, -3.171}, {-100, -2.407}, {-50, -1.269},
    {0, 0.000}, {50, 1.325}, {100, 2.774}, {150, 4.345}, {200, 5.913},
    {250, 7.600}, {300, 9.341}, {350, 11.136}, {400, 12.974}, {450, 14.852},
    {500, 16.771}, {550, 18.729}, {600, 20.726}, {650, 22.761}, {700, 24.833},
    {750, 26.941}, {800, 29.083}, {850, 31.258}, {900, 33.465}, {950, 35.703},
    {1000, 37.971}, {1050, 40.267}, {1100, 42.588}, {1150, 44.932}, {1200, 47.298},
    {1250, 49.684}, {1300, 52.089}
};

// Type S (Pt10%Rh-Pt) - NIST ITS-90
static const TablePoint TYPE_S_NIST[] = {
    {0, 0.000}, {50, 0.299}, {100, 0.646}, {150, 1.029}, {200, 1.441},
    {250, 1.874}, {300, 2.323}, {350, 2.786}, {400, 3.259}, {450, 3.742},
    {500, 4.233}, {550, 4.732}, {600, 5.237}, {650, 5.747}, {700, 6.262},
    {750, 6.782}, {800, 7.306}, {850, 7.834}, {900, 8.367}, {950, 8.903},
    {1000, 9.443}, {1050, 9.986}, {1100, 10.532}, {1150, 11.081}, {1200, 11.632},
    {1250, 12.186}, {1300, 12.742}, {1350, 13.300}, {1400, 13.860}, {1450, 14.421},
    {1500, 14.984}, {1550, 15.548}, {1600, 16.113}, {1650, 16.679}, {1700, 17.245},
    {1750, 17.811}, {1768, 17.947}
};

// Type R (Pt13%Rh-Pt) - NIST ITS-90
static const TablePoint TYPE_R_NIST[] = {
    {0, 0.000}, {50, 0.296}, {100, 0.647}, {150, 1.041}, {200, 1.469},
    {250, 1.923}, {300, 2.401}, {350, 2.896}, {400, 3.408}, {450, 3.933},
    {500, 4.471}, {550, 5.021}, {600, 5.583}, {650, 6.155}, {700, 6.736},
    {750, 7.326}, {800, 7.924}, {850, 8.530}, {900, 9.143}, {950, 9.763},
    {1000, 10.389}, {1050, 11.021}, {1100, 11.658}, {1150, 12.299}, {1200, 12.945},
    {1250, 13.595}, {1300, 14.249}, {1350, 14.906}, {1400, 15.567}, {1450, 16.230},
    {1500, 16.896}, {1550, 17.564}, {1600, 18.234}, {1650, 18.906}, {1700, 19.579},
    {1750, 20.253}, {1768, 20.413}
};

// Type B (Pt30%Rh-Pt6%Rh) - NIST ITS-90
static const TablePoint TYPE_B_NIST[] = {
    {200, 0.033}, {250, 0.178}, {300, 0.431}, {350, 0.787}, {400, 1.242},
    {450, 1.792}, {500, 2.431}, {550, 3.154}, {600, 3.957}, {650, 4.834},
    {700, 5.780}, {750, 6.786}, {800, 7.848}, {850, 8.956}, {900, 10.099},
    {950, 11.263}, {1000, 12.433}, {1050, 13.591}, {1100, 14.732}, {1150, 15.847},
    {1200, 16.932}, {1250, 17.981}, {1300, 18.991}, {1350, 19.960}, {1400, 20.886},
    {1450, 21.768}, {1500, 22.607}, {1550, 23.402}, {1600, 24.153}, {1650, 24.861},
    {1700, 25.525}, {1750, 26.147}, {1800, 26.727}, {1820, 26.883}
};

// Helper function to get table data for a specific type
inline const TablePoint* getTableData(ThermocoupleType type, int& size) {
    switch (type) {
        case ThermocoupleType::TYPE_K:
            size = sizeof(TYPE_K_NIST) / sizeof(TablePoint);
            return TYPE_K_NIST;
        case ThermocoupleType::TYPE_J:
            size = sizeof(TYPE_J_NIST) / sizeof(TablePoint);
            return TYPE_J_NIST;
        case ThermocoupleType::TYPE_T:
            size = sizeof(TYPE_T_NIST) / sizeof(TablePoint);
            return TYPE_T_NIST;
        case ThermocoupleType::TYPE_E:
            size = sizeof(TYPE_E_NIST) / sizeof(TablePoint);
            return TYPE_E_NIST;
        case ThermocoupleType::TYPE_N:
            size = sizeof(TYPE_N_NIST) / sizeof(TablePoint);
            return TYPE_N_NIST;
        case ThermocoupleType::TYPE_S:
            size = sizeof(TYPE_S_NIST) / sizeof(TablePoint);
            return TYPE_S_NIST;
        case ThermocoupleType::TYPE_R:
            size = sizeof(TYPE_R_NIST) / sizeof(TablePoint);
            return TYPE_R_NIST;
        case ThermocoupleType::TYPE_B:
            size = sizeof(TYPE_B_NIST) / sizeof(TablePoint);
            return TYPE_B_NIST;
        default:
            size = 0;
            return nullptr;
    }
}

// Get table points for plotting within a temperature range
inline QVector<QPair<double, double>> getTablePoints(ThermocoupleType type,
                                                      double minTemp, double maxTemp,
                                                      double coldJunctionVoltage = 0.0) {
    QVector<QPair<double, double>> points;
    int size;
    const TablePoint* data = getTableData(type, size);

    if (!data) return points;

    for (int i = 0; i < size; i++) {
        if (data[i].temperature >= minTemp && data[i].temperature <= maxTemp) {
            // Apply cold junction compensation
            double voltage = data[i].voltage - coldJunctionVoltage;
            points.append(qMakePair(data[i].temperature, voltage));
        }
    }

    return points;
}

} // namespace ThermocoupleReferenceData

#endif // THERMOCOUPLEREFERENCEDATA_H
