import tkinter as tk
import math
from PIL import Image, ImageTk

# Initialize Tkinter and create main window
root = tk.Tk()
root.title("RPM Gauge")

# Constants for gauge drawing
canvas_width, canvas_height = 800, 800
center_x, center_y = canvas_width // 2, canvas_height // 2
needle_length = 300

# Create canvas widget
canvas = tk.Canvas(root, width=canvas_width, height=canvas_height, bg='black')
canvas.pack()

# Open the image with Pillow
image = Image.open("rpmgauge.jpg")
image = image.resize((canvas_width, canvas_height), Image.LANCZOS)

# Convert the Image object into a TkPhoto object
photo = ImageTk.PhotoImage(image)

canvas.create_image(0, 0, anchor=tk.NW, image=photo)

# Initial needle (at 0 RPM)
needle = canvas.create_line(center_x, center_y, center_x, center_y - needle_length, fill='red', width=4)

# Function to update the needle based on RPM
def update_needle(rpm):
    max_rpm = 7000  # Maximum RPM for scale
    angle_degrees = 90 + (rpm / max_rpm) * 180  # Convert RPM to angle (simplified)
    angle_radians = math.radians(angle_degrees)
    
    # Calculate new needle end position
    end_x = center_x + needle_length * math.cos(angle_radians)
    end_y = center_y - needle_length * math.sin(angle_radians)
    
    # Update the needle
    canvas.coords(needle, center_x, center_y, end_x, end_y)

# Slider to simulate changing RPM
rpm_slider = tk.Scale(root, from_=0, to=7000, orient='horizontal', command=lambda value: update_needle(int(value)))
rpm_slider.pack()

# Start the Tkinter event loop
root.mainloop()
