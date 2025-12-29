#include "thermocouplecalculator.h"
#include <cmath>
#include <stdexcept>

ThermocoupleCalculator::ThermocoupleCalculator()
    : m_type(ThermocoupleType::TYPE_K)
{
    initializeCoefficients();
}

void ThermocoupleCalculator::setType(ThermocoupleType type)
{
    m_type = type;
}

void ThermocoupleCalculator::initializeCoefficients()
{
    // Type K (Chromel-Alumel) - NIST ITS-90
    // Forward: Temperature to Voltage
    m_forwardCoefficients[ThermocoupleType::TYPE_K] = {
        // Range 1: -270°C to 0°C
        {-270.0, 0.0, {
            0.0,
            0.394501280250E-01,
            0.236223735980E-04,
            -0.328589067840E-06,
            -0.499048287770E-08,
            -0.675090591730E-10,
            -0.574103274280E-12,
            -0.310888728940E-14,
            -0.104516093650E-16,
            -0.198892668780E-19,
            -0.163226974860E-22
        }, false, {}},
        // Range 2: 0°C to 1372°C
        {0.0, 1372.0, {
            -0.176004136860E-01,
            0.389212049750E-01,
            0.185587700320E-04,
            -0.994575928740E-07,
            0.318409457190E-09,
            -0.560728448890E-12,
            0.560750590590E-15,
            -0.320207200030E-18,
            0.971511471520E-22,
            -0.121047212750E-25
        }, true, {
            0.118597600000E+00,
            -0.118343200000E-03,
            0.126968600000E+03
        }}
    };

    // Type J (Iron-Constantan) - NIST ITS-90
    m_forwardCoefficients[ThermocoupleType::TYPE_J] = {
        // Range 1: -210°C to 760°C
        {-210.0, 760.0, {
            0.0,
            0.503811878150E-01,
            0.304758369300E-04,
            -0.856810657200E-07,
            0.132281952950E-09,
            -0.170529583370E-12,
            0.209480906970E-15,
            -0.125383953360E-18,
            0.156317256970E-22
        }, false, {}},
        // Range 2: 760°C to 1200°C
        {760.0, 1200.0, {
            0.296456256810E+03,
            -0.149761277860E+01,
            0.317871039240E-02,
            -0.318476867010E-05,
            0.157208190040E-08,
            -0.306913690560E-12
        }, false, {}}
    };

    // Type T (Copper-Constantan) - NIST ITS-90
    m_forwardCoefficients[ThermocoupleType::TYPE_T] = {
        // Range 1: -270°C to 0°C
        {-270.0, 0.0, {
            0.0,
            0.387481063640E-01,
            0.441944343470E-04,
            0.118443231050E-06,
            0.200329735540E-07,
            0.901380195590E-09,
            0.226511565930E-10,
            0.360711542050E-12,
            0.384939398830E-14,
            0.282135219250E-16,
            0.142515947790E-18,
            0.487686622860E-21,
            0.107955392700E-23,
            0.139450270620E-26,
            0.797951539270E-30
        }, false, {}},
        // Range 2: 0°C to 400°C
        {0.0, 400.0, {
            0.0,
            0.387481063640E-01,
            0.332922278800E-04,
            0.206182434040E-06,
            -0.218822568460E-08,
            0.109968809280E-10,
            -0.308157587720E-13,
            0.454791352900E-16,
            -0.275129016730E-19
        }, false, {}}
    };

    // Type E (Chromel-Constantan) - NIST ITS-90
    m_forwardCoefficients[ThermocoupleType::TYPE_E] = {
        // Range 1: -270°C to 0°C
        {-270.0, 0.0, {
            0.0,
            0.586655087080E-01,
            0.454109771240E-04,
            -0.779980486860E-06,
            -0.258001608430E-07,
            -0.594525830570E-09,
            -0.932140586670E-11,
            -0.102876055340E-12,
            -0.803701236210E-15,
            -0.439794973910E-17,
            -0.164147763550E-19,
            -0.396736195160E-22,
            -0.558273287210E-25,
            -0.346578420130E-28
        }, false, {}},
        // Range 2: 0°C to 1000°C
        {0.0, 1000.0, {
            0.0,
            0.586655087080E-01,
            0.450322755820E-04,
            0.289084072120E-07,
            -0.330568966520E-09,
            0.650244032700E-12,
            -0.191974955040E-15,
            -0.125366004970E-17,
            0.214892175690E-20,
            -0.143880417820E-23,
            0.359608994810E-27
        }, false, {}}
    };

    // Type N (Nicrosil-Nisil) - NIST ITS-90
    m_forwardCoefficients[ThermocoupleType::TYPE_N] = {
        // Range 1: -270°C to 0°C
        {-270.0, 0.0, {
            0.0,
            0.261591059620E-01,
            0.109574842280E-04,
            -0.938411115540E-07,
            -0.464120397590E-10,
            -0.263033577160E-11,
            -0.226534380030E-13,
            -0.760893007910E-16,
            -0.934196678350E-19
        }, false, {}},
        // Range 2: 0°C to 1300°C
        {0.0, 1300.0, {
            0.0,
            0.259293946010E-01,
            0.157101418800E-04,
            0.438256272370E-07,
            -0.252611697940E-09,
            0.643118193390E-12,
            -0.100634715190E-14,
            0.997453389920E-18,
            -0.608632456070E-21,
            0.208492293390E-24,
            -0.306821961510E-28
        }, false, {}}
    };

    // Simplified coefficients for noble metal thermocouples (can be expanded with full NIST data)

    // Type S (Pt-Pt/Rh 10%)
    m_forwardCoefficients[ThermocoupleType::TYPE_S] = {
        {0.0, 1768.0, {
            0.0,
            0.540313308631E-02,
            0.125934289740E-04,
            -0.232477968689E-07,
            0.322028823036E-10,
            -0.331465196389E-13,
            0.255744251786E-16,
            -0.125068871393E-19,
            0.271443176145E-23
        }, false, {}}
    };

    // Type R (Pt-Pt/Rh 13%)
    m_forwardCoefficients[ThermocoupleType::TYPE_R] = {
        {0.0, 1768.0, {
            0.0,
            0.528961729765E-02,
            0.139166589782E-04,
            -0.238855693017E-07,
            0.356916001063E-10,
            -0.462347666298E-13,
            0.500777441034E-16,
            -0.373105886191E-19,
            0.157716482367E-22,
            -0.281038625251E-26
        }, false, {}}
    };

    // Type B (Pt/Rh 30%-Pt/Rh 6%)
    m_forwardCoefficients[ThermocoupleType::TYPE_B] = {
        {200.0, 1820.0, {
            -0.389381686210E+01,
            0.285717474700E-01,
            -0.848851047850E-04,
            0.157852801640E-06,
            -0.168353448640E-09,
            0.111097940130E-12,
            -0.445154310330E-16,
            0.989756408210E-20,
            -0.937913302890E-24
        }, false, {}}
    };

    // Temperature ranges
    m_temperatureRanges[ThermocoupleType::TYPE_K] = {-270.0, 1372.0};
    m_temperatureRanges[ThermocoupleType::TYPE_J] = {-210.0, 1200.0};
    m_temperatureRanges[ThermocoupleType::TYPE_T] = {-270.0, 400.0};
    m_temperatureRanges[ThermocoupleType::TYPE_E] = {-270.0, 1000.0};
    m_temperatureRanges[ThermocoupleType::TYPE_N] = {-270.0, 1300.0};
    m_temperatureRanges[ThermocoupleType::TYPE_S] = {-50.0, 1768.0};
    m_temperatureRanges[ThermocoupleType::TYPE_R] = {-50.0, 1768.0};
    m_temperatureRanges[ThermocoupleType::TYPE_B] = {200.0, 1820.0};
}

double ThermocoupleCalculator::evaluatePolynomial(double temperature, const PolynomialRange& range) const
{
    double voltage = 0.0;
    double tempPower = 1.0;

    // Calculate standard polynomial
    for (size_t i = 0; i < range.coefficients.size(); ++i) {
        voltage += range.coefficients[i] * tempPower;
        tempPower *= temperature;
    }

    // Add exponential correction if needed (Type K above 0°C)
    if (range.isExponential && temperature > 0) {
        double a0 = range.expCoefficients[0];
        double a1 = range.expCoefficients[1];
        double a2 = range.expCoefficients[2];
        voltage += a0 * std::exp(a1 * (temperature - a2) * (temperature - a2));
    }

    return voltage;
}

double ThermocoupleCalculator::temperatureToVoltage(double temperature) const
{
    // Check if temperature is in valid range
    double minT = getMinTemperature();
    double maxT = getMaxTemperature();

    if (temperature < minT || temperature > maxT) {
        throw std::out_of_range("Temperature out of range for this thermocouple type");
    }

    // Find appropriate range
    const auto& ranges = m_forwardCoefficients[m_type];
    for (const auto& range : ranges) {
        if (temperature >= range.minTemp && temperature <= range.maxTemp) {
            return evaluatePolynomial(temperature, range);
        }
    }

    throw std::runtime_error("No valid polynomial range found");
}

double ThermocoupleCalculator::inversePolynomial(double voltage, double coldJunctionTemp) const
{
    // Account for cold junction compensation
    double coldJunctionVoltage = 0.0;
    if (coldJunctionTemp != 0.0) {
        coldJunctionVoltage = temperatureToVoltage(coldJunctionTemp);
        voltage += coldJunctionVoltage;
    }

    // Use Newton-Raphson method to solve inverse
    // Start with midpoint of valid temperature range as initial guess
    double minTemp = getMinTemperature();
    double maxTemp = getMaxTemperature();
    double temperature = (minTemp + maxTemp) / 2.0;  // Initial guess at midpoint
    const int maxIterations = 50;
    const double tolerance = 1e-6;

    for (int i = 0; i < maxIterations; ++i) {
        double f = temperatureToVoltage(temperature) - voltage;

        // Numerical derivative (dV/dT)
        double h = 0.1;
        // Ensure derivative points stay within valid range
        double tempPlus = std::min(temperature + h, maxTemp);
        double tempMinus = std::max(temperature - h, minTemp);
        double df = (temperatureToVoltage(tempPlus) - temperatureToVoltage(tempMinus)) / (tempPlus - tempMinus);

        if (std::abs(df) < 1e-12) break;

        double newTemp = temperature - f / df;

        if (std::abs(newTemp - temperature) < tolerance) {
            return newTemp;
        }

        temperature = newTemp;

        // Clamp to valid range
        temperature = std::max(minTemp, std::min(temperature, maxTemp));
    }

    return temperature;
}

double ThermocoupleCalculator::voltageToTemperature(double voltage, double coldJunctionTemp) const
{
    return inversePolynomial(voltage, coldJunctionTemp);
}

double ThermocoupleCalculator::getMinTemperature() const
{
    return m_temperatureRanges[m_type].first;
}

double ThermocoupleCalculator::getMaxTemperature() const
{
    return m_temperatureRanges[m_type].second;
}

double ThermocoupleCalculator::getMinVoltage() const
{
    return temperatureToVoltage(getMinTemperature());
}

double ThermocoupleCalculator::getMaxVoltage() const
{
    return temperatureToVoltage(getMaxTemperature());
}

QString ThermocoupleCalculator::getTypeName() const
{
    switch (m_type) {
        case ThermocoupleType::TYPE_K: return "Type K (Chromel-Alumel)";
        case ThermocoupleType::TYPE_J: return "Type J (Iron-Constantan)";
        case ThermocoupleType::TYPE_T: return "Type T (Copper-Constantan)";
        case ThermocoupleType::TYPE_E: return "Type E (Chromel-Constantan)";
        case ThermocoupleType::TYPE_N: return "Type N (Nicrosil-Nisil)";
        case ThermocoupleType::TYPE_S: return "Type S (Pt-Pt/Rh 10%)";
        case ThermocoupleType::TYPE_R: return "Type R (Pt-Pt/Rh 13%)";
        case ThermocoupleType::TYPE_B: return "Type B (Pt/Rh 30%-Pt/Rh 6%)";
        default: return "Unknown";
    }
}

QString ThermocoupleCalculator::getColorCode() const
{
    switch (m_type) {
        case ThermocoupleType::TYPE_K: return "Yellow";
        case ThermocoupleType::TYPE_J: return "Black";
        case ThermocoupleType::TYPE_T: return "Blue";
        case ThermocoupleType::TYPE_E: return "Purple";
        case ThermocoupleType::TYPE_N: return "Orange";
        case ThermocoupleType::TYPE_S: return "Green";
        case ThermocoupleType::TYPE_R: return "Green";
        case ThermocoupleType::TYPE_B: return "Gray";
        default: return "Unknown";
    }
}

double ThermocoupleCalculator::getSensitivity() const
{
    // Approximate sensitivity at 0°C in µV/°C
    switch (m_type) {
        case ThermocoupleType::TYPE_K: return 41.0;
        case ThermocoupleType::TYPE_J: return 52.0;
        case ThermocoupleType::TYPE_T: return 41.0;
        case ThermocoupleType::TYPE_E: return 61.0;
        case ThermocoupleType::TYPE_N: return 27.0;
        case ThermocoupleType::TYPE_S: return 6.0;
        case ThermocoupleType::TYPE_R: return 6.0;
        case ThermocoupleType::TYPE_B: return 1.0;
        default: return 0.0;
    }
}

QString ThermocoupleCalculator::getApplications() const
{
    switch (m_type) {
        case ThermocoupleType::TYPE_K:
            return "General purpose, industrial, HVAC";
        case ThermocoupleType::TYPE_J:
            return "Iron/steel industry, reducing atmospheres";
        case ThermocoupleType::TYPE_T:
            return "Cryogenics, food industry, laboratory";
        case ThermocoupleType::TYPE_E:
            return "High output, cryogenics, power generation";
        case ThermocoupleType::TYPE_N:
            return "High temperature stability, oxidizing atmospheres";
        case ThermocoupleType::TYPE_S:
            return "High accuracy, pharmaceutical, biotech";
        case ThermocoupleType::TYPE_R:
            return "High temperature, laboratory standards";
        case ThermocoupleType::TYPE_B:
            return "Very high temperature, glass manufacturing";
        default:
            return "Unknown";
    }
}
