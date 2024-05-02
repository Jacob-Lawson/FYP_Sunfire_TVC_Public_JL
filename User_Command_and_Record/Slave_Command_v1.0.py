import csv
import requests
import time
import sys

def send_values(csv_file, url):
    try:
        with open(csv_file, 'r', encoding='utf-8-sig') as file:
            reader = csv.DictReader(file, delimiter=',')
            
            # Create a session for reusing TCP connection
            session = requests.Session()
            session_timeout = 10  # Set timeout to 10 seconds
            
            # Batch data for sending in chunks
            batch_size = 1 # set 1 to use time interval
            data_batch = []
            
            for row in reader:
                #print(row)  # Debugging: Print row to verify data
                time_interval = int(row['time(ms)']) / 1000
                x_deg = float(row['TVC_X_deg'])
                y_deg = float(row['TVC_Y_deg'])
                payload = {'esp32_data': f"{x_deg},{y_deg}"}
                data_batch.append(payload)
                time.sleep(time_interval)
                
                # Send batch of data
                if len(data_batch) >= batch_size:
                    for data in data_batch:
                        try:
                            response = session.get(url, params=data, timeout=session_timeout)
                            response.raise_for_status()  # Raise exception for non-200 status codes
                        except Exception as e:
                            print(f"Failed to send data: {e}")
                    data_batch = []  # Reset batch
                    # Sleep after sending a batch
                    time.sleep(0.01)  # Adjust sleep time as needed
                    
            # Send remaining data
            if data_batch:
                for data in data_batch:
                    try:
                        response = session.get(url, params=data, timeout=session_timeout)
                        response.raise_for_status()  # Raise exception for non-200 status codes
                    except Exception as e:
                        print(f"Failed to send data: {e}")
                    
    except Exception as e:
        print(f"Error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python Command_Profile.py <ESP32_IP_Address> <CSV_File_Location>")
        sys.exit(1)
    ip_address = sys.argv[1]
    csv_file = sys.argv[2]
    url = f'http://{ip_address}/thumbstick'
    send_values(csv_file, url)
