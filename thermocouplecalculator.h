#ifndef THERMOCOUPLECALCULATOR_H
#define THERMOCOUPLECALCULATOR_H

#include <QString>
#include <QMap>
#include <vector>

// Thermocouple types based on IEC 60584 and NIST ITS-90
enum class ThermocoupleType {
    TYPE_K = 0,  // Chromel-Alumel (most common)
    TYPE_J,      // Iron-Constantan
    TYPE_T,      // Copper-Constantan
    TYPE_E,      // Chromel-Constantan
    TYPE_N,      // Nicrosil-Nisil
    TYPE_S,      // Platinum-Platinum/Rhodium 10%
    TYPE_R,      // Platinum-Platinum/Rhodium 13%
    TYPE_B       // Platinum/Rhodium 30%-Platinum/Rhodium 6%
};

// Polynomial coefficients structure
struct PolynomialRange {
    double minTemp;
    double maxTemp;
    std::vector<double> coefficients;
    bool isExponential; // For reverse calculation with exponential terms
    std::vector<double> expCoefficients; // Exponential correction coefficients
};

class ThermocoupleCalculator
{
public:
    ThermocoupleCalculator();

    // Set thermocouple type
    void setType(ThermocoupleType type);
    ThermocoupleType getType() const { return m_type; }

    // Temperature to voltage conversion (°C to mV)
    double temperatureToVoltage(double temperature) const;

    // Voltage to temperature conversion (mV to °C)
    // coldJunctionTemp: temperature of the cold junction in °C (default 0°C)
    double voltageToTemperature(double voltage, double coldJunctionTemp = 0.0) const;

    // Get temperature range for current type
    double getMinTemperature() const;
    double getMaxTemperature() const;

    // Get voltage range for current type
    double getMinVoltage() const;
    double getMaxVoltage() const;

    // Get thermocouple name
    QString getTypeName() const;

    // Get color code
    QString getColorCode() const;

    // Get sensitivity (approximate µV/°C at 0°C)
    double getSensitivity() const;

    // Get common applications
    QString getApplications() const;

private:
    ThermocoupleType m_type;

    // Initialize polynomial coefficients for all types
    void initializeCoefficients();

    // Polynomial evaluation for temperature to voltage
    double evaluatePolynomial(double temperature, const PolynomialRange& range) const;

    // Inverse polynomial for voltage to temperature (Newton-Raphson method)
    double inversePolynomial(double voltage, double coldJunctionTemp) const;

    // Storage for polynomial coefficients (forward: T->V)
    QMap<ThermocoupleType, std::vector<PolynomialRange>> m_forwardCoefficients;

    // Storage for polynomial coefficients (reverse: V->T)
    QMap<ThermocoupleType, std::vector<PolynomialRange>> m_reverseCoefficients;

    // Temperature ranges for each type
    QMap<ThermocoupleType, QPair<double, double>> m_temperatureRanges;
};

#endif // THERMOCOUPLECALCULATOR_H
