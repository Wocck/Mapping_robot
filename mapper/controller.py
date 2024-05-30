import subprocess
import serial
import time
import os

# Constants
SERIAL_PORT = 'COM12'
BAUD_RATE = 9600
MOVEMENT_PARAMETERS = {'power': 500, 'direction': -100}
DATA_FILE = "robot_data.txt"
RESPONSE_TIMEOUT = 2

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

def receive_response(ser, timeout=RESPONSE_TIMEOUT):
    end_time = time.time() + timeout
    response = ""
    while time.time() < end_time:
        if ser.in_waiting > 0:
            response += ser.read(ser.in_waiting).decode()
            if "OK" in response:
                print("Received:", response)
                return True
            if "ERR" in response:
                print("Error parsing response", response)
                return False
    print("No response received.")
    return None

def check_connection(ser):
    send_command(ser, "CHECK_CONNECTION")
    return receive_response(ser)

def robot_run(ser, power, direction):
    send_command(ser, f"RUN {power} {direction}")
    return receive_response(ser)

def robot_stop(ser):
    send_command(ser, "STOP")
    return receive_response(ser)

def robot_move(ser, power, direction, time_ms):
    res = robot_run(ser, power, direction)
    if res:
        time.sleep(time_ms / 1000)
        robot_stop(ser)
        return True
    return False

def robot_start_lidar(ser):
    send_command(ser, "LIDAR_START")
    return receive_response(ser)

def capture_lidar_data(ser, duration=6):
    """Capture LIDAR data continuously for a given duration and store in a list."""
    start_time = time.time()
    captured_data = []
    
    while time.time() - start_time < duration:
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting)
            captured_data.extend(data)
    
    return captured_data

def process_lidar_packets(captured_data):
    """Process raw LIDAR data to extract valid packets with a specific format."""
    starting_sequence = b'\x54\x2C'  
    packet_length = 26  
    packets = []

    index = 0
    while index < len(captured_data) - packet_length - len(starting_sequence) + 1:
        if captured_data[index:index+2] == starting_sequence:
            if index + 2 + packet_length <= len(captured_data):
                packet = captured_data[index+2:index+2+packet_length]
                if len(packet) == packet_length:
                    packets.append(packet)
                index += 2 + packet_length
                continue
        index += 1

    return packets

def main():
    ser = connect_robot(SERIAL_PORT)
    if ser is None:
        return

    if not check_connection(ser):
        return

    robot_start_lidar(ser)
    data = capture_lidar_data(ser)
    packets = process_lidar_packets(data)
    print(f"Processed {len(packets)} packets.")
    ser.close()

if __name__ == "__main__":
    main()