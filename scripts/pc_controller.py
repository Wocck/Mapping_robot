import subprocess
import serial
import time
import os

# Constants
SERIAL_PORT = 'COM12'
BAUD_RATE = 9600
LIDAR_COLLECTION_TIME = 7  # in seconds
MOVEMENT_PARAMETERS = {'power': 500, 'direction': -100, 'time': 2500}  # power, direction, duration in ms
DATA_FILE = "robot_data.txt"

def connect_robot(port):
    try:
        ser = serial.Serial(port, BAUD_RATE, timeout=1)
        print("Connected to", port)
        return ser
    except Exception as e:
        print(f"Failed to connect: {e}")
        return None

def send_command(ser, command):
    command += "\r\n"
    ser.write(command.encode())
    print(f"Sent: {command}")

def receive_response(ser, timeout=6):
    send_command(ser, "CHECK_CONNECTION")
    end_time = time.time() + timeout
    response = ""
    while time.time() < end_time:
        if ser.in_waiting > 0:
            response += ser.read(ser.in_waiting).decode()
            if "OK" in response:
                print("Received:", response)
                return response
            if "ERR" in response:
                print("Error parsing response", response)
                return response
    print("No response received.")
    return None

def connect_serial(port):
    ser = serial.Serial(port, 9600, timeout=None)
    return ser

def find_packet(ser, end_time):
    packet = bytearray()
    while time.time() < end_time:
        byte = ser.read(1)
        if not byte:
            continue

        if byte == b'\x54':
            next_byte = ser.read(1)
            if next_byte == b'\x2C':
                if len(packet) > 0:
                    return packet
                packet = bytearray(b'\x54\x2C')
            else:
                packet += byte + next_byte
        else:
            packet += byte
    return packet if len(packet) > 0 else None

def collect_lidar_data(ser, duration):
    send_command(ser, "LIDAR_START")
    start_time = time.time()
    end_time = start_time + duration
    packets = []
    while time.time() < end_time:
        packet = find_packet(ser, end_time)
        if packet:
            packets.append(packet)
    send_command(ser, "LIDAR_STOP")
    return packets

def save_data_to_file(data):
    with open(DATA_FILE, "a") as file:
        for packet in data:
            hex_data = ' '.join(f"{byte:02X}" for byte in packet)
            file.write(hex_data + "\n")
        file.write("\n\n")

def process_data(data):
    # Placeholder for data processing logic
    print("Processing data: {} packets collected".format(len(data)))
    save_data_to_file(data)

def move_robot(ser, power, direction, time_ms):
    send_command(ser, f"RUN {power} {direction}")
    time.sleep(time_ms / 1000)
    send_command(ser, "STOP")

def main():
    ser = connect_robot(SERIAL_PORT)
    if ser and receive_response(ser):
        # Collect initial LIDAR data
        data = collect_lidar_data(ser, LIDAR_COLLECTION_TIME)
        process_data(data)



        # Command the robot to move
        move_robot(ser, MOVEMENT_PARAMETERS['power'], MOVEMENT_PARAMETERS['direction'], MOVEMENT_PARAMETERS['time'])

        # Collect and process LIDAR data after movement
        time.sleep(3)
        data = collect_lidar_data(ser, LIDAR_COLLECTION_TIME)
        process_data(data)

        ser.close()
    else:
        print("Could not establish connection with the robot.")

if __name__ == "__main__":
    main()
