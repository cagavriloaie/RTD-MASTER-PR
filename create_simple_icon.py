#!/usr/bin/env python3
"""
Create a simple icon for RTD Master application using PIL/Pillow
"""

try:
    from PIL import Image, ImageDraw, ImageFont

    # Create a 256x256 image with a blue background
    size = 256
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Draw background circle (blue)
    margin = 8
    draw.ellipse([margin, margin, size-margin, size-margin],
                 fill='#3498db', outline='#2980b9', width=8)

    # Draw thermometer body (white rectangle with rounded corners)
    therm_width = 56
    therm_left = (size - therm_width) // 2
    therm_top = 60
    therm_height = 120
    draw.rounded_rectangle([therm_left, therm_top, therm_left + therm_width, therm_top + therm_height],
                          radius=28, fill='white', outline='#2c3e50', width=4)

    # Draw temperature scale lines
    for y in [80, 100, 120, 140, 160]:
        draw.line([therm_left - 15, y, therm_left - 5, y], fill='#2c3e50', width=2)

    # Draw thermometer bulb (red circle)
    bulb_radius = 30
    bulb_y = 170
    bulb_x = size // 2
    draw.ellipse([bulb_x - bulb_radius, bulb_y - bulb_radius,
                 bulb_x + bulb_radius, bulb_y + bulb_radius],
                fill='#e74c3c', outline='#c0392b', width=4)

    # Draw mercury column (red rectangle)
    mercury_width = 20
    mercury_left = (size - mercury_width) // 2
    draw.rounded_rectangle([mercury_left, 90, mercury_left + mercury_width, 165],
                          radius=10, fill='#e74c3c')

    # Draw inner bulb detail
    inner_radius = 18
    draw.ellipse([bulb_x - inner_radius, bulb_y - inner_radius,
                 bulb_x + inner_radius, bulb_y + inner_radius],
                fill='#c0392b')

    # Draw RTD text
    try:
        # Try to use a nice font
        font = ImageFont.truetype("arial.ttf", 32)
    except:
        # Fallback to default font
        font = ImageFont.load_default()

    text = "RTD"
    # Get text bbox for centering
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    text_x = (size - text_width) // 2
    draw.text((text_x, 215), text, fill='white', font=font)

    # Save as PNG
    img.save('icon.png', 'PNG')
    print("Created icon.png (256x256)")

    # Create smaller sizes for ICO
    sizes = [(16, 16), (24, 24), (32, 32), (48, 48), (64, 64), (128, 128), (256, 256)]
    images = []

    for size_tuple in sizes:
        resized = img.resize(size_tuple, Image.Resampling.LANCZOS)
        images.append(resized)
        print(f"  Created {size_tuple[0]}x{size_tuple[1]} image")

    # Save as ICO
    images[0].save('icon.ico', format='ICO', sizes=sizes, append_images=images[1:])
    print("Created icon.ico (multi-size)")

    print("\nSuccess! Icon files created:")
    print("  - icon.png (for Qt)")
    print("  - icon.ico (for Windows executable)")

except ImportError:
    print("Error: PIL/Pillow not installed")
    print("\nPlease install Pillow:")
    print("  pip install pillow")
    print("\nThen run this script again.")
except Exception as e:
    print(f"Error creating icon: {e}")
    print("\nAlternative: Convert icon.svg manually")
    print("  Visit: https://convertio.co/svg-ico/")
