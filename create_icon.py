#!/usr/bin/env python3
"""
Convert SVG icon to ICO and PNG formats for the RTD Master application
"""

try:
    from PIL import Image
    import io

    # Try to import cairosvg for SVG conversion
    try:
        import cairosvg
        HAS_CAIROSVG = True
    except ImportError:
        HAS_CAIROSVG = False
        print("Warning: cairosvg not found. Trying alternative method...")

    if HAS_CAIROSVG:
        # Convert SVG to PNG using cairosvg
        print("Converting icon.svg to PNG...")

        # Create multiple sizes for ICO file
        sizes = [16, 24, 32, 48, 64, 128, 256]
        images = []

        for size in sizes:
            png_data = cairosvg.svg2png(url='icon.svg', output_width=size, output_height=size)
            img = Image.open(io.BytesIO(png_data))
            images.append(img)
            print(f"  Created {size}x{size} image")

        # Save as ICO with multiple sizes
        print("Saving icon.ico...")
        images[0].save('icon.ico', format='ICO', sizes=[(s, s) for s in sizes], append_images=images[1:])

        # Save largest as PNG
        print("Saving icon.png...")
        png_data = cairosvg.svg2png(url='icon.svg', output_width=256, output_height=256)
        with open('icon.png', 'wb') as f:
            f.write(png_data)

        print("\n✓ Successfully created icon.ico and icon.png")

    else:
        print("\nAlternative method: Using online converter or manual conversion")
        print("\nPlease install cairosvg:")
        print("  pip install cairosvg pillow")
        print("\nOr use an online converter:")
        print("  1. Go to https://convertio.co/svg-ico/")
        print("  2. Upload icon.svg")
        print("  3. Convert to ICO and download")
        print("  4. Save as icon.ico in the RTD folder")

except ImportError as e:
    print(f"Error: {e}")
    print("\nPlease install required packages:")
    print("  pip install pillow cairosvg")
    print("\nOr use an online converter:")
    print("  https://convertio.co/svg-ico/")
