import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
import numpy as np
import struct
import math

class LidarProcessor:
    def __init__(self):
        self.distance_to_cut = 200

    def create_lidar_map_from_list(self, data_list):
        lidar_map = [0] * 360
        for chunk in data_list:
            if len(chunk) != 26:
                continue
            output_bytes = bytes(chunk)
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

    def create_coordinates_list(self, distances):
        angles = np.deg2rad(np.arange(0, 360)[::-1])
        x = distances * np.sin(angles)
        y = distances * np.cos(angles)
        for idx in range(len(x)):
            if abs(x[idx]) > 2000 or abs(y[idx]) > 2000 or \
               (-self.distance_to_cut < x[idx] < self.distance_to_cut and \
               -self.distance_to_cut < y[idx] < self.distance_to_cut):
                x[idx] = y[idx] = 0
        non_zero_indices = np.where((x != 0) & (y != 0))
        return x[non_zero_indices], y[non_zero_indices]

    def find_wall_points(self, x, y):
        min_y = np.min(y)
        max_y = np.max(y)
        y_range = 250
        y_segments = np.arange(min_y, max_y, 25)
        y_best_segments = []
        most_points = 0
        for i in range(len(y_segments) - 4):
            for j in range(i + 4, len(y_segments)):
                y_ranges = (y_segments[i], y_segments[j])
                choosen_idxs = np.where((y >= y_ranges[0]) & (y < y_ranges[0] + y_range) |
                                        (y >= y_ranges[1]) & (y < y_ranges[1] + y_range))[0]
                point_counter = len(choosen_idxs)
                if point_counter > most_points:
                    y_best_segments = [y_ranges]
                    most_points = point_counter
                elif point_counter == most_points:
                    y_best_segments.append(y_ranges)
        y_best_segments = y_best_segments[:2][0]
        chosen_x1, chosen_y1, chosen_x2, chosen_y2 = [], [], [], []
        for i in range(len(y)):
            if y_best_segments[0] <= y[i] < y_best_segments[0] + y_range:
                chosen_x1.append(x[i])
                chosen_y1.append(y[i])
            elif y_best_segments[1] <= y[i] < y_best_segments[1] + y_range:
                chosen_x2.append(x[i])
                chosen_y2.append(y[i])
        return np.array(chosen_x1), np.array(chosen_y1), np.array(chosen_x2), np.array(chosen_y2)

    def find_lines(self, chosen_x, chosen_y):
        model = LinearRegression()
        model.fit(chosen_x.reshape(-1, 1), chosen_y)
        return model.coef_[0], model.intercept_

    def plot_with_walls_lines(self, all_x, all_y, x1, y1, x2, y2, a1, b1, a2, b2):
        plt.figure(figsize=(10, 6))
        plt.scatter(all_x, all_y, color='blue', label='All Points')
        plt.scatter(x1, y1, color='red', label='Points in first range')
        plt.scatter(x2, y2, color='green', label='Points in second range')
        plt.plot(np.arange(min(all_x), max(all_x), 0.1), a1 * np.arange(min(all_x), max(all_x), 0.1) + b1, color='orange', label='First line')
        plt.plot(np.arange(min(all_x), max(all_x), 0.1), a2 * np.arange(min(all_x), max(all_x), 0.1) + b2, color='purple', label='Second line')
        plt.xlabel('x')
        plt.ylabel('y')
        plt.title('Points in the best two y ranges')
        plt.legend()
        plt.grid(True)
        plt.show()

    def find_shift_and_angle(self, a1, b1, a2, b2):
        angle_1 = math.degrees(math.atan(a1))
        angle_2 = math.degrees(math.atan(a2))
        angle = -min(angle_1, angle_2) if angle_1 >= 0 and angle_2 >= 0 else -max(angle_1, angle_2)
        distance1 = b1 / math.sqrt(a1**2 + 1)
        distance2 = b2 / math.sqrt(a2**2 + 1)
        y_shift = -0.5 * (distance1 + distance2)
        return angle, y_shift

    def make_correct_coordinates_list(self, last_x, last_y, angle, y_shift):
        theta = math.radians(angle)
        cos_theta = math.cos(theta)
        sin_theta = math.sin(theta)
        x_corrected = [x * cos_theta - y * sin_theta for x, y in zip(last_x, last_y)]
        y_corrected = [x * sin_theta + y * cos_theta for x, y in zip(last_x, last_y)]
        return x_corrected, y_corrected
