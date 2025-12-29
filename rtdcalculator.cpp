#include "rtdcalculator.h"

RTDCalculator::RTDCalculator()
    : m_standard(RTDStandard::IEC_60751)
    , m_material(RTDMaterial::PLATINUM)
    , m_alpha(AlphaCoefficient::PT_385)
    , m_tolerance(ToleranceClass::CLASS_B)
    , m_r0(100.0)
{
}

void RTDCalculator::setStandard(RTDStandard standard)
{
    m_standard = standard;
}

void RTDCalculator::setMaterial(RTDMaterial material)
{
    m_material = material;
}

void RTDCalculator::setNominalResistance(double r0)
{
    m_r0 = r0;
}

void RTDCalculator::setAlphaCoefficient(AlphaCoefficient alpha)
{
    m_alpha = alpha;
}

void RTDCalculator::setToleranceClass(ToleranceClass tolerance)
{
    m_tolerance = tolerance;
}

double RTDCalculator::alphaToValue(AlphaCoefficient alpha) const
{
    switch (alpha) {
        case AlphaCoefficient::PT_385: return 0.00385;
        case AlphaCoefficient::PT_391: return 0.00391;
        case AlphaCoefficient::PT_392: return 0.00392;
        case AlphaCoefficient::CU_426: return 0.00426;
        case AlphaCoefficient::CU_427: return 0.00427;
        case AlphaCoefficient::CU_428: return 0.00428;
        case AlphaCoefficient::NI_617: return 0.00617;
        case AlphaCoefficient::NI_618: return 0.00618;
        case AlphaCoefficient::NI_672: return 0.00672;
        default: return 0.00385;
    }
}

double RTDCalculator::getAlphaValue() const
{
    return alphaToValue(m_alpha);
}

QString RTDCalculator::getMaterialName() const
{
    switch (m_material) {
        case RTDMaterial::PLATINUM: return "Platinum";
        case RTDMaterial::COPPER: return "Copper";
        case RTDMaterial::NICKEL: return "Nickel";
        default: return "Unknown";
    }
}

QString RTDCalculator::getStandardName() const
{
    switch (m_standard) {
        case RTDStandard::IEC_60751: return "IEC 60751";
        case RTDStandard::GOST_6651: return "GOST 6651";
        case RTDStandard::ASTM_E1137: return "ASTM E1137";
        case RTDStandard::LEGACY_US: return "Legacy US";
        default: return "Unknown";
    }
}

RTDCalculator::CVDCoefficients RTDCalculator::getCVDCoefficients() const
{
    CVDCoefficients coeff;
    
    // Callendar-Van Dusen equation coefficients for IEC 60751
    if (m_alpha == AlphaCoefficient::PT_385) {
        coeff.A = 3.9083e-3;
        coeff.B = -5.775e-7;
        coeff.C = -4.183e-12;  // Only used below 0°C
    }
    else if (m_alpha == AlphaCoefficient::PT_391) {
        // GOST coefficients
        coeff.A = 3.96847e-3;
        coeff.B = -5.847e-7;
        coeff.C = -4.22e-12;
    }
    else {
        // Default to IEC
        coeff.A = 3.9083e-3;
        coeff.B = -5.775e-7;
        coeff.C = -4.183e-12;
    }
    
    return coeff;
}

double RTDCalculator::calculateResistancePlatinum(double temperature)
{
    // Callendar-Van Dusen equation for platinum
    CVDCoefficients coeff = getCVDCoefficients();
    
    if (temperature >= 0) {
        // R(T) = R0 * (1 + A*T + B*T²)
        return m_r0 * (1.0 + coeff.A * temperature + coeff.B * temperature * temperature);
    } else {
        // R(T) = R0 * (1 + A*T + B*T² + C*(T-100)*T³)
        double t2 = temperature * temperature;
        double t3 = t2 * temperature;
        return m_r0 * (1.0 + coeff.A * temperature + coeff.B * t2 + 
                       coeff.C * (temperature - 100.0) * t3);
    }
}

double RTDCalculator::calculateResistanceLinear(double temperature)
{
    // Linear approximation: R(T) = R0 * (1 + α * T)
    double alpha = alphaToValue(m_alpha);
    return m_r0 * (1.0 + alpha * temperature);
}

double RTDCalculator::temperatureToResistance(double temperature)
{
    if (m_material == RTDMaterial::PLATINUM) {
        return calculateResistancePlatinum(temperature);
    } else {
        // For copper and nickel, use linear approximation
        return calculateResistanceLinear(temperature);
    }
}

double RTDCalculator::calculateTemperaturePlatinum(double resistance)
{
    // Solve Callendar-Van Dusen equation for temperature
    // This uses an iterative approach (Newton-Raphson)
    
    CVDCoefficients coeff = getCVDCoefficients();
    double temperature = 0.0;
    
    // Initial guess using linear approximation
    temperature = (resistance / m_r0 - 1.0) / coeff.A;
    
    // Newton-Raphson iteration
    for (int i = 0; i < 10; i++) {
        double r_calc = temperatureToResistance(temperature);
        double error = r_calc - resistance;
        
        if (std::abs(error) < 0.0001) break;
        
        // Calculate derivative dR/dT
        double delta = 0.01;
        double r_plus = temperatureToResistance(temperature + delta);
        double derivative = (r_plus - r_calc) / delta;
        
        if (derivative != 0) {
            temperature -= error / derivative;
        }
    }
    
    return temperature;
}

double RTDCalculator::calculateTemperatureLinear(double resistance)
{
    // T = (R - R0) / (R0 * α)
    double alpha = alphaToValue(m_alpha);
    return (resistance - m_r0) / (m_r0 * alpha);
}

double RTDCalculator::resistanceToTemperature(double resistance)
{
    if (m_material == RTDMaterial::PLATINUM) {
        return calculateTemperaturePlatinum(resistance);
    } else {
        return calculateTemperatureLinear(resistance);
    }
}

double RTDCalculator::calculateTolerance(double temperature)
{
    // IEC 60751 tolerance classes
    double t_abs = std::abs(temperature);
    
    switch (m_tolerance) {
        case ToleranceClass::CLASS_AA:
            return 0.1 + 0.0017 * t_abs;
        case ToleranceClass::CLASS_A:
            return 0.15 + 0.002 * t_abs;
        case ToleranceClass::CLASS_B:
            return 0.3 + 0.005 * t_abs;
        case ToleranceClass::CLASS_C:
            return 0.6 + 0.01 * t_abs;
        default:
            return 0.3 + 0.005 * t_abs;
    }
}
