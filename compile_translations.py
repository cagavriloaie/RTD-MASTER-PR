#!/usr/bin/env python3
"""
Compile Qt translation files (.ts) to compiled format (.qm)
"""

import subprocess
import os
import sys
from pathlib import Path

def find_lrelease():
    """Try to find lrelease executable"""
    # Common Qt installation paths
    common_paths = [
        r"C:\Qt\6.7.0\mingw_64\bin\lrelease.exe",
        r"C:\Qt\6.6.0\mingw_64\bin\lrelease.exe",
        r"C:\Qt\6.5.0\mingw_64\bin\lrelease.exe",
        r"C:\Qt\5.15.2\mingw81_64\bin\lrelease.exe",
        r"C:\Qt\5.15.0\mingw81_64\bin\lrelease.exe",
        r"C:\Qt\Tools\QtCreator\bin\lrelease.exe",
    ]

    # Check if lrelease is in PATH
    try:
        result = subprocess.run(['lrelease', '-version'],
                              capture_output=True,
                              text=True)
        if result.returncode == 0:
            return 'lrelease'
    except:
        pass

    # Check common paths
    for path in common_paths:
        if os.path.exists(path):
            return path

    # Try to find Qt in C:\Qt
    qt_base = Path('C:/Qt')
    if qt_base.exists():
        for exe in qt_base.rglob('lrelease.exe'):
            return str(exe)

    return None

def compile_translations():
    """Compile all .ts files to .qm"""
    lrelease = find_lrelease()

    if not lrelease:
        print("ERROR: Could not find lrelease executable!")
        print("\nPlease compile translations manually:")
        print("1. Open Qt Creator")
        print("2. Open RTD.pro")
        print("3. Go to Tools -> External -> Linguist -> Release Translations")
        print("\nOr add Qt bin directory to your PATH")
        return False

    print(f"Using lrelease: {lrelease}")
    print()

    translations_dir = Path(__file__).parent / 'translations'
    ts_files = list(translations_dir.glob('*.ts'))

    if not ts_files:
        print("No .ts files found in translations directory")
        return False

    success = True
    for ts_file in ts_files:
        qm_file = ts_file.with_suffix('.qm')
        print(f"Compiling {ts_file.name} -> {qm_file.name}...")

        try:
            result = subprocess.run([lrelease, str(ts_file), '-qm', str(qm_file)],
                                  capture_output=True,
                                  text=True)

            if result.returncode == 0:
                print(f"  [OK] {qm_file.name} created successfully")
            else:
                print(f"  [ERROR] Failed to compile {ts_file.name}")
                print(f"  {result.stderr}")
                success = False
        except Exception as e:
            print(f"  [ERROR] {e}")
            success = False

    print()
    if success:
        print("SUCCESS! All translations compiled.")
        print("You can now build and run the project.")
    else:
        print("Some translations failed to compile.")

    return success

if __name__ == '__main__':
    compile_translations()
    input("\nPress Enter to close...")
