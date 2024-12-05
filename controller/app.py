import streamlit as st
import time
import random

# Function to simulate checking connection status for each device
def check_connection(device_id):
    time.sleep(1)  # Simulate a delay for checking each device
    return random.choice([True, False])  # Randomly return connected or disconnected status

# Set up the Streamlit app
st.title("Multi-Device Connection Status Checker")

# Define the device names
devices = ["Device 1", "Device 2", "Device 3"]

# Layout the devices in columns
status_columns = st.columns(len(devices))

# Function to update the status with a loading bar
def update_status():
    for i, device in enumerate(devices):
        with status_columns[i]:
            st.subheader(device)
            st.spinner("Connecting...")
            # Show a progress bar while checking connection
            # with st.spinner("Checking..."):
            #     connection_status = check_connection(device)
                # time.sleep(1)  # Simulate loading time

            # Display the connection status
            # if connection_status:
            #     st.success("Connected")
            # else:
            #     st.error("Disconnected")

# Button to manually refresh connection statuses
if st.button("Refresh Connection Status"):
    update_status()

# Auto-refresh every 10 seconds (optional)
while True:
    update_status()
    time.sleep(10)  # Adjust the refresh interval as needed
