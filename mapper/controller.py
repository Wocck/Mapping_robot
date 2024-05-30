import serial
import time

SERIAL_PORT = "COM12"

class RobotController:
    def __init__(self, port='COM12', baud_rate=9600, response_timeout=2):
        self.port = port
        self.baud_rate = baud_rate
        self.response_timeout = response_timeout
        self.ser = None

    def connect(self):
        try:
            self.ser = serial.Serial(self.port, self.baud_rate, timeout=1)
            print(f"Connected to {self.port}")
            return True
        except Exception as e:
            print(f"Failed to connect: {e}")
            return False

    def send_command(self, command):
        try:
            self.ser.write(f"{command}\r\n".encode())
            print(f"Sent: {command}")
        except Exception as e:
            print(f"Error sending command {command}: {e}")

    def receive_response(self):
        end_time = time.time() + self.response_timeout
        response = ""
        while time.time() < end_time:
            if self.ser.in_waiting > 0:
                response += self.ser.read(self.ser.in_waiting).decode()
                if "OK" in response:
                    print(f"Received: {response}")
                    return True
                if "ERR" in response:
                    print(f"Error parsing response: {response}")
                    return False
        print("No response received.")
        return False

    def flush_until_sequence(self, sequence):
        """ Flush the serial buffer until the specified sequence is found. """
        buffer = bytearray()
        seq_len = len(sequence)
        try:
            while True:
                if self.ser.in_waiting > 0:
                    buffer.extend(self.ser.read(self.ser.in_waiting))
                    if buffer[-seq_len:] == bytearray(sequence):
                        print("Sequence found, flushing complete.")
                        break
        except Exception as e:
            print(f"Error while flushing serial buffer: {e}")

    def clear_serial_buffer(self):
        """ Clear all data from the serial buffer. """
        if self.ser:
            self.ser.flushInput()

    def check_connection(self):
        self.send_command("CHECK_CONNECTION")
        return self.receive_response()

    def robot_run(self, power, direction):
        self.send_command(f"RUN {power} {direction}")
        return self.receive_response()

    def robot_stop(self):
        self.send_command("STOP")
        return self.receive_response()

    def robot_move(self, power, direction, time_ms):
        if self.robot_run(power, direction):
            time.sleep(time_ms / 1000)
            return self.robot_stop()
        return False

    def robot_start_lidar(self):
        self.send_command("LIDAR_START")
        return self.receive_response()

    def capture_lidar_data(self, duration=12):
        """Capture LIDAR data continuously for a given duration and store in a list."""
        start_time = time.time()
        captured_data = []
        
        while time.time() - start_time < duration:
            if self.ser.in_waiting > 0:
                data = self.ser.read(self.ser.in_waiting)
                captured_data.extend(data)
        
        return captured_data

    def process_lidar_packets(self, captured_data):
        """Process raw LIDAR data to extract valid packets with a specific format."""
        starting_sequence = [0x54, 0x2C]
        packet_length = 26
        packets = []
        index = 0
        while index < len(captured_data) - len(starting_sequence) + 1:
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
    robot = RobotController(SERIAL_PORT)
    if robot.connect() and robot.check_connection():
        robot.robot_start_lidar()
        data = robot.capture_lidar_data()
        print(f"Captured {len(data)} bytes of LIDAR data.")
        packets = robot.process_lidar_packets(data)
        print(f"Captured {len(packets)} packets.")
    robot.ser.close()

if __name__ == "__main__":
    main()
