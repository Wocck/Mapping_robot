import Controller
import LidarProcessor

def main():
    robot = Controller.RobotController("COM12")
    lidar_processor = LidarProcessor.LidarProcessor()


    if not robot.connect():
        return

    if not robot.check_connection():
        return

    try:
        robot.robot_start_lidar()
        data = robot.capture_lidar_data(duration=14)
        packets = robot.process_lidar_packets(data)
        print(f"Captured {len(packets)} packets in the first round.")
  
        lidar_map = lidar_processor.create_lidar_map_from_list(packets)
        x, y = lidar_processor.create_coordinates_list(lidar_map)
        x1, y1, x2, y2 = lidar_processor.find_wall_points(x, y)
        a1, b1 = lidar_processor.find_lines(x1, y1)
        a2, b2 = lidar_processor.find_lines(x2, y2)
        angle, y_shift = lidar_processor.find_shift_and_angle(a1, b1, a2, b2)
        lidar_processor.plot_with_walls_lines(x, y, x1, y1, x2, y2, a1, b1, a2, b2)

        print(f"angle: {angle}")
        # Move the robot based on the processed data
        robot.clear_serial_buffer()
        if not robot.robot_move(500, 10, 1500):  # Sample power, direction, and duration
            return  # Exit if the move command fails

        # Second LIDAR data collection
        robot.robot_start_lidar()
        data = robot.capture_lidar_data(duration=14)
        packets = robot.process_lidar_packets(data)
        print(f"Captured {len(packets)} packets in the second round.")

        # Process the second round of data
        lidar_map = lidar_processor.create_lidar_map_from_list(packets)
        x, y = lidar_processor.create_coordinates_list(lidar_map)
        x1, y1, x2, y2 = lidar_processor.find_wall_points(x, y)
        a1, b1 = lidar_processor.find_lines(x1, y1)
        a2, b2 = lidar_processor.find_lines(x2, y2)
        angle, y_shift = lidar_processor.find_shift_and_angle(a1, b1, a2, b2)
        lidar_processor.plot_with_walls_lines(x, y, x1, y1, x2, y2, a1, b1, a2, b2)
        print(f"angle: {angle}")
        robot.clear_serial_buffer()

    finally:
        robot.ser.close()  # Ensure serial connection is closed on exit

if __name__ == "__main__":
    main()
