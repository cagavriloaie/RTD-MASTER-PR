# RTD Master - Professional Temperature Calculator

A Qt-based professional application for RTD (Resistance Temperature Detector) calculations supporting multiple international standards.

## Features

- **Multiple Standards Support:**
  - IEC 60751 (International/European)
  - GOST 6651 (Russian/Soviet)
  - ASTM E1137 (American)
  - Legacy US standard

- **Multiple Materials:**
  - Platinum (Pt100, Pt1000, etc.)
  - Copper (Cu50, Cu100)
  - Nickel (Ni100, Ni120, Ni1000)

- **Temperature Coefficients:**
  - Pt385 (0.00385) - IEC/ASTM/DIN
  - Pt391 (0.00391) - GOST Platinum
  - Pt392 (0.00392) - Legacy US
  - Cu426, Cu427, Cu428 - Various Copper standards
  - Ni617, Ni618, Ni672 - Various Nickel types

- **Tolerance Classes (IEC 60751):**
  - Class AA: ±(0.1 + 0.0017|t|)°C
  - Class A: ±(0.15 + 0.002|t|)°C
  - Class B: ±(0.3 + 0.005|t|)°C
  - Class C: ±(0.6 + 0.01|t|)°C

- **Bi-directional Conversion:**
  - Temperature → Resistance
  - Resistance → Temperature

## Building the Project

### Prerequisites
- Qt 5.x or Qt 6.x
- Qt Creator (recommended) or qmake
- C++17 compatible compiler

### Using Qt Creator
1. Copy all files to: C:\Users\Constantin\Desktop\RTD
2. Open Qt Creator
3. File → Open File or Project
4. Select RTD.pro
5. Configure the project with your Qt kit
6. Build → Build Project "RTD"
7. Run → Run

### Using qmake (Command Line)
```bash
cd C:\Users\Constantin\Desktop\RTD
qmake RTD.pro
make (or nmake on Windows with MSVC)
```

## Usage

1. **Select RTD Configuration:**
   - Choose the standard (IEC, GOST, ASTM, etc.)
   - Select material (Platinum, Copper, Nickel)
   - Pick the appropriate alpha coefficient
   - Set R0 (nominal resistance at 0°C)
   - Choose tolerance class

2. **Perform Conversion:**
   - Select conversion mode (Temp→Resistance or Resistance→Temp)
   - Enter the input value
   - Click "Calculate"
   - View the result and tolerance

## Implementation Details

### Platinum RTDs
Uses the Callendar-Van Dusen equation:
- For T ≥ 0°C: R(T) = R0(1 + AT + BT²)
- For T < 0°C: R(T) = R0[1 + AT + BT² + C(T-100)T³]

### Copper and Nickel RTDs
Uses linear approximation:
- R(T) = R0(1 + αT)

## Files

- `RTD.pro` - Qt project file
- `main.cpp` - Application entry point
- `mainwindow.h/cpp` - Main window GUI
- `mainwindow.ui` - Qt Designer UI file
- `rtdcalculator.h/cpp` - RTD calculation engine
- `README.md` - This file

## Future Enhancements

Possible additions:
- Export results to CSV
- Batch conversion
- Calibration data import
- Graphical resistance-temperature curve
- Wire compensation (2-wire, 3-wire, 4-wire)
- Multi-language support

## License

Free to use for educational and commercial purposes.

## Author

Created for RTD temperature measurement applications.
