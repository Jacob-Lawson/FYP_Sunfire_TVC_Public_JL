import requests
import csv
import time
from datetime import datetime
import sys
import os  # Add this line to import the os module

def run_script(ip_address, csv_save_location):
    # URLs of the ESP32 server with the provided IP address
    thumbstick_url = f'http://{ip_address}/thumbstick'
    toggle_url = f'http://{ip_address}/toggle'
    temperature_url = f'http://{ip_address}/temperature'  # Add URL for temperature data
    mpu6050_url = f'http://{ip_address}/mpu6050'  # Add URL for mpu6050 data


    # Get Datetime
    now = datetime.now()
    DataTime = now.strftime("%Y-%m-%d, %H-%M")

    # File path to save the CSV file
    csv_file = os.path.join(csv_save_location, f"data_{DataTime}.csv")

    # Number of data points to collect
    data_duration = 5               # duration of test to record (mins)
    data_freq = 100                 # frequency of data recording (Hz)
    num_data_points = int((data_duration * 60) * data_freq)  # duration of test data saved as number of data points
    # multiply num_data_points *(1/data_freq) for time duration of test
    # this limit exists to ensure the data is not saved endless which could exceed the file limit and cause data loss

    def fetch_data(url):
        try:
            response = requests.get(url)
            if response.status_code == 200:
                return response.text
            else:
                print("Failed to fetch data. Status code:", response.status_code)
                return None
        except Exception as e:
            print("Error fetching data:", str(e))
            return None

    def save_to_csv(timestamp, thumbstick_data, toggle_data, temperature_data, mpu6050_data, filename):
        tvc_x, tvc_y, command_x, command_y = thumbstick_data.split(',')
        ax, ay, az, gx, gy, gz, pitch, roll, yaw = mpu6050_data.split(',')
        led_state = "on" if toggle_data.strip() == "LED state toggled" else "off"
        temperature_value = temperature_data.split()[0]  # Split temperature data and take the first part
        try:
            with open(filename, 'a', newline='') as csvfile:
                writer = csv.writer(csvfile)
                writer.writerow([timestamp, tvc_x, tvc_y, command_x, command_y, temperature_value, led_state, ax, ay, az, gx, gy, gz, pitch, roll, yaw])  # Use temperature_value without unit
            #print("Data saved to", filename)
        except Exception as e:
            print("Error saving data to CSV:", str(e))

    print("Pulling data from ESP32 server and saving to CSV...")
    print("Press Ctrl+C to stop.")

    try:
        with open(csv_file, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(["Timestamp", "TVC_X_Pos_(steps)", "TVC_Y_Pos_(steps)", "TVC_Command_Xvalue_(deg)", "TVC_Command_Yvalue(deg)", "Temperature (Celius)", "LED state", "Xacl", "Yacl", "Zacl", "Xgrav", "Ygrav", "Zgrav", "roll", "pitch", "yaw"])  # Update header

        end_time = time.time() + (data_duration * 60)  # Calculate end time based on data duration
        while time.time() < end_time:
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
            thumbstick_data = fetch_data(thumbstick_url)
            toggle_data = fetch_data(toggle_url)
            temperature_data = fetch_data(temperature_url)  # Fetch temperature data
            mpu6050_data = fetch_data(mpu6050_url)  # Fetch mpu6050 data
            if thumbstick_data and toggle_data and temperature_data and mpu6050_data:  # Check if all data points are fetched
                save_to_csv(timestamp, thumbstick_data, toggle_data, temperature_data, mpu6050_data, csv_file)
            else:
                print("Data point not available. Skipping...")
            time.sleep(1 / data_freq)  # Adjust the interval as needed
    except KeyboardInterrupt:
        print("Data collection stopped.")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python slave_record.py <ESP32_IP_Address> <CSV_Save_Location>")
        sys.exit(1)
    ip_address = sys.argv[1]
    csv_save_location = sys.argv[2]
    run_script(ip_address, csv_save_location)
