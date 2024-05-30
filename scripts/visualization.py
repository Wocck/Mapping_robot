import matplotlib.pyplot as plt
import struct
import numpy as np


def create_lidar_map_from_file(file_path):
    lidar_map = [0] * 360
    with open(file_path, 'rb') as file:
        while True:
            chunk = file.readline()
            chunk = chunk.rstrip(b'\r\n')
            if len(chunk) == 0:
                break
            elif len(chunk) != 129:
                continue

            hex_numbers = chunk.decode().split()
            output_bytes = b''.join(bytes.fromhex(s[2:]) for s in hex_numbers)

            angle_start = struct.unpack('<H', output_bytes[0:2])[0] / 100
            distances = struct.unpack('<12H', output_bytes[2:26])
            angle_increment = 10 / 12
            current_angle = angle_start

            for distance in distances:
                if round(int(current_angle)) >= 360:
                    current_angle -= 360
                index = round(int(current_angle))
                if index == 360:
                    index = 0
                lidar_map[index] = distance
                current_angle += angle_increment
    return lidar_map


def plot_lidar_distances(distances):
    angles = np.deg2rad(np.arange(0, 360)[::-1])
    x = distances * np.sin(angles)
    y = distances * np.cos(angles)
    plt.figure(figsize=(15, 10))
    plt.plot(-x, y, 'bo')
    plt.gca().set_aspect('equal', adjustable='box')
    plt.title('Mapa pomieszczenia na podstawie danych lidaru')
    plt.xlabel('Współrzędna X')
    plt.ylabel('Współrzędna Y')
    plt.grid(True)
    plt.show()


lidar_map = create_lidar_map_from_file("received_data.txt")
print(lidar_map)
plot_lidar_distances(lidar_map)
