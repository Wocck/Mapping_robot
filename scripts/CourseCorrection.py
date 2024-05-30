import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
import struct
import numpy as np
import math


def create_lidar_map_from_list(list):
    lidar_map = [0] * 360

    for chunk in list:
        if len(chunk) != 26:
            continue

        byte_list_bytes = [bytes.fromhex(s) for s in chunk]
        output_bytes = b''.join(byte_list_bytes)
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


def create_coordinates_list(distances):
    angles = np.deg2rad(np.arange(0, 360)[::-1])
    x = distances * np.sin(angles)
    y = distances * np.cos(angles)

    distance_to_cut = 200
    for idx, dis_x in enumerate(x):
        if (dis_x > 2000 or dis_x < -2000 or
                (-1 * distance_to_cut < dis_x < distance_to_cut)):
            x[idx] = 0
            y[idx] = 0
    for idx, dis_y in enumerate(y):
        if (dis_y > 2000 or dis_y < -2000 or
                (-1 * distance_to_cut < dis_y < distance_to_cut)):
            x[idx] = 0
            y[idx] = 0

    # Usunięcie punktów (0, 0) z danych
    non_zero_indices = np.where((x != 0) & (y != 0))
    x_filtered = x[non_zero_indices]
    y_filtered = y[non_zero_indices]
    return x_filtered, y_filtered


def find_wall_points(x, y):
    min_y = np.min(y)
    max_y = np.max(y)
    y_range = 250

    # Przedziały y o wielkości 100, sprawdzane co 25
    y_segments = np.arange(min_y, max_y, 25)

    y_best_segments = []
    most_points = 0

    # Szukanie przedziałów z największą liczbą punktów
    for i in range(len(y_segments)):
        for j in range(i + 4, len(y_segments)):
            y_ranges = y_segments[i], y_segments[j]
            choosen_idxs = np.where((y >= y_ranges[0]) & (y < y_ranges[0] + y_range) |
                                       (y >= y_ranges[1]) & (y < y_ranges[1] + y_range))[0]
            point_counter = len(choosen_idxs)
            if point_counter > most_points:
                y_best_segments = [y_ranges]
                most_points = point_counter
            elif point_counter == most_points:
                y_best_segments.append(y_ranges)

    y_best_segments = y_best_segments[:2][0]

    chosen_x1 = []
    chosen_y1 = []
    chosen_x2 = []
    chosen_y2 = []

    for i in range(len(y)):
        if (y[i] >= y_best_segments[0]) & (y[i] < y_best_segments[0] + y_range):
            chosen_x1.append(x[i])
            chosen_y1.append(y[i])
        elif (y[i] >= y_best_segments[1]) & (y[i] < y_best_segments[1] + y_range):
            chosen_x2.append(x[i])
            chosen_y2.append(y[i])

    return np.array(chosen_x1), np.array(chosen_y1), np.array(chosen_x2), np.array(chosen_y2)


def find_lines(chosen_x, chosen_y):
    # Inicjowanie i dopasowanie modelu regresji liniowej
    model = LinearRegression()
    model.fit(chosen_x.reshape(-1, 1), chosen_y)

    # Współczynniki prostej
    a = model.coef_[0]
    b = model.intercept_

    return a, b


def plot_with_walls_lines(all_x, all_y, x1, y1, x2, y2, a1, b1, a2, b2):
    plt.figure(figsize=(10, 6))
    plt.scatter(all_x, all_y, color='blue', label='Wszystkie punkty')
    plt.scatter(x1, y1, color='red', label='Punkty w pierwszym przedziale')
    plt.scatter(x2, y2, color='green', label='Punkty w drugim przedziale')

    plt.plot(np.arange(min(all_x), max(all_x), 0.1), a1 * np.arange(min(all_x), max(all_x), 0.1) + b1, color='orange',
             label='Pierwsza prosta')

    plt.plot(np.arange(min(all_x), max(all_x), 0.1), a2 * np.arange(min(all_x), max(all_x), 0.1) + b2, color='purple',
             label='Druga prosta')

    plt.xlabel('x')
    plt.ylabel('y')
    plt.title('Punkty w dwóch najlepszych zakresach y o wielkości 100')
    plt.legend()
    plt.grid(True)
    plt.show()


def find_shift_and_angle(a1, b1, a2, b2):
    angle_1 = math.degrees(math.atan(a1))
    angle_2 = math.degrees(math.atan(a2))
    if angle_1 >= 0.0 and angle_2 >= 0.0:
        angle = -1 * min(angle_2, angle_1)
    elif angle_1 <= 0.0 and angle_2 <= 0.0:
        angle = -1 * max(angle_2, angle_1)
    else:
        angle = -1 * angle_1
    distance1 = b1 / math.sqrt(a1 ** 2 + 1)
    distance2 = b2 / math.sqrt(a2 ** 2 + 1)

    y_shift = -0.5 * (distance1 + distance2)
    return angle, y_shift


def make_correct_coordinates_list(last_x, last_y, angle, y_shift):
    theta = math.radians(angle)
    cos_theta = math.cos(theta)
    sin_theta = math.sin(theta)
    x_corrected = []
    y_corrected = []

    for x, y in zip(last_x, last_y):
        y += y_shift
        x_new = x * cos_theta - y * sin_theta
        y_new = x * sin_theta + y * cos_theta
        x_corrected.append(x_new)
        y_corrected.append(y_new)

    return x_corrected, y_corrected


def create_final_lists(list):
    lidar_map = create_lidar_map_from_list(list)
    x, y = create_coordinates_list(lidar_map)
    x1, y1, x2, y2 = find_wall_points(x, y)
    a1, b1 = find_lines(x1, y1)
    a2, b2 = find_lines(x2, y2)
    plot_with_walls_lines(x, y, x1, y1, x2, y2, a1, b1, a2, b2)
    angle, y_shift = find_shift_and_angle(a1, b1, a2, b2)
    final_x, final_y = make_correct_coordinates_list(x, y, angle, y_shift)
    return final_x, final_y, angle


def plot_lidar_distances(x_coordinates_list, y_coordinates_list, x_distance):
    x_shift = -x_distance
    max_range = 2 * x_distance * len(x_coordinates_list)
    plt.figure(figsize=(15, 15))
    for x_coordinates, y_coordinates in zip(x_coordinates_list, y_coordinates_list):
        x_shift += x_distance
        for x, y in zip(x_coordinates, y_coordinates):
            plt.plot(-x - x_shift, y, 'bo')
    plt.xlim(-max_range, max_range)  # Ustawienie zakresu osi x
    plt.ylim(-max_range, max_range)  # Ustawienie zakresu osi y
    plt.gca().set_aspect('equal', adjustable='box')
    plt.title('Mapa pomieszczenia na podstawie danych lidaru')
    plt.xlabel('Współrzędna X')
    plt.ylabel('Współrzędna Y')
    plt.grid(True)
    plt.show()


lista1 = []
lista2 = []

# Dane globalne, które powinny być dla całego projektu
distance = 1125  # odległość w mm ile czasu jedzie robot
x_combined_list = []
y_combined_list = []

# Dwa przykładowe wywołania dla poszczególnych pomiarów
x_lidar_map, y_lidar_map, angle = create_final_lists(lista1)
x_combined_list.append(x_lidar_map)
y_combined_list.append(y_lidar_map)
plot_lidar_distances(x_combined_list, y_combined_list, distance)
print(angle)

x_lidar_map, y_lidar_map, angle = create_final_lists(lista2)
x_combined_list.append(x_lidar_map)
y_combined_list.append(y_lidar_map)
plot_lidar_distances(x_combined_list, y_combined_list, distance)
print(angle)
