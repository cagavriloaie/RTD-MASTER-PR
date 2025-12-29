#ifndef RTDCALCULATOR_H
#define RTDCALCULATOR_H

#include <QString>
#include <cmath>

// RTD Standards
enum class RTDStandard {
    IEC_60751,      // International/European standard
    GOST_6651,      // Russian/Soviet standard
    ASTM_E1137,     // American standard (same as IEC for Pt385)
    LEGACY_US       // Old American Pt392
};

// RTD Materials
enum class RTDMaterial {
    PLATINUM,
    COPPER,
    NICKEL
};

// Temperature coefficient types
enum class AlphaCoefficient {
    PT_385,     // 0.00385 (IEC/ASTM/DIN)
    PT_391,     // 0.00391 (GOST Platinum)
    PT_392,     // 0.00392 (Legacy US)
    CU_426,     // 0.00426 (GOST Copper)
    CU_427,     // 0.00427 (IEC Copper)
    CU_428,     // 0.00428 (Some Copper variants)
    NI_617,     // 0.00617 (Nickel 100)
    NI_618,     // 0.00618 (Nickel 1000)
    NI_672      // 0.00672 (Nickel 120)
};

// Tolerance classes (IEC 60751)
enum class ToleranceClass {
    CLASS_AA,
    CLASS_A,
    CLASS_B,
    CLASS_C
};

class RTDCalculator
{
public:
    RTDCalculator();
    
    // Set RTD configuration
    void setStandard(RTDStandard standard);
    void setMaterial(RTDMaterial material);
    void setNominalResistance(double r0); // Resistance at 0°C
    void setAlphaCoefficient(AlphaCoefficient alpha);
    void setToleranceClass(ToleranceClass tolerance);
    
    // Temperature to Resistance conversion
    double temperatureToResistance(double temperature);
    
    // Resistance to Temperature conversion
    double resistanceToTemperature(double resistance);
    
    // Tolerance calculation
    double calculateTolerance(double temperature);
    
    // Get alpha value
    double getAlphaValue() const;
    
    // Get material name
    QString getMaterialName() const;
    
    // Get standard name
    QString getStandardName() const;
    
    // Callendar-Van Dusen coefficients (for platinum)
    struct CVDCoefficients {
        double A;
        double B;
        double C;
    };
    
    CVDCoefficients getCVDCoefficients() const;
    
private:
    RTDStandard m_standard;
    RTDMaterial m_material;
    AlphaCoefficient m_alpha;
    ToleranceClass m_tolerance;
    double m_r0;  // Nominal resistance at 0°C
    
    // Helper functions
    double alphaToValue(AlphaCoefficient alpha) const;
    double calculateResistancePlatinum(double temperature);
    double calculateResistanceLinear(double temperature);
    double calculateTemperaturePlatinum(double resistance);
    double calculateTemperatureLinear(double resistance);
};

#endif // RTDCALCULATOR_H
