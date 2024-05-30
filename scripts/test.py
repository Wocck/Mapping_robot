import serial
import time

# Constants
SERIAL_PORT = 'COM12'
BAUD_RATE = 115200

def connect_serial(port):
    try:
        ser = serial.Serial(port, BAUD_RATE, timeout=None)
        print("Connected to serial port", port)
        return ser
    except Exception as e:
        print(f"Failed to connect to {port}: {e}")
        return None

def find_packet(ser):
    packet = bytearray()
    while True:
        byte = ser.read(1)
        if not byte:
            continue
        else:
            packet += byte
        if len(packet) >= 26:
            return packet

def print_packet(packet):
    hex_data = ' '.join(f"{byte:02X}" for byte in packet)
    print("Received Packet:", hex_data)

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

def main():
    ser = connect_serial(SERIAL_PORT)
    if ser:
        try:
            receive_response(ser)
            send_command(ser, "LIDAR_START")
            send_command(ser, "LIDAR_START")
            while True:
                byte = ser.read(1)
                if not byte:
                    continue
                else:
                    print(byte)
        finally:
            ser.close()
            print("Serial connection closed.")
    else:
        print("Unable to open serial connection.")

if __name__ == "__main__":
    main()
