#CDD Script
import serial
import csv

# Change COM3 to your Arduino's port
ser = serial.Serial('COM3', 115200, timeout=1)

with open('data.csv', 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(['heart_rate', 'confidence', 'oxygen', 'status', 'temperature'])

    try:
        while True:
            line = ser.readline().decode('utf-8', errors='ignore').strip()

            if not line:
                continue

            print(line)

            data = line.split(',')

            # Make sure the line has exactly 5 values
            if len(data) == 5:
                writer.writerow(data)
                file.flush()  # saves immediately
            else:
                print("Skipped malformed line:", line)

    except KeyboardInterrupt:
        print("\nStopped logging.")

    finally:
        ser.close()
