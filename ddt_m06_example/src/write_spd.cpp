// Copyright 2023 Ar-Ray-code.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <ddt_m06_interface/ddt_m06_interface.hpp>
#include <iostream>
#include <thread>

void print_motor_status(MotorStatus status)
{
  printf("id: %d\tmode: %d\tspeed: %d\tposition: %d\tcurrent: %f\n", status.id, status.mode, status.speed, status.position, status.current);
}

int main(int argc, char ** argv)
{
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <id (0)> <port_name (/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AH00RC8V-if00-port0)>" << std::endl;
    return EXIT_FAILURE;
  }
  int TARGET_ID = std::stoi(argv[1]);
  std::string port_name = argv[2];

  auto ddt = std::make_shared<ddt_m06_interface::ddtM06Handler>(port_name);
  MotorStatus motor_status;

  ddt->switch_mode(TARGET_ID, VELOCITY_LOOP_MODE);

  ddt->drive_motor(TARGET_ID, 0, motor_status);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  for (int i = 0; i < 330; i++) {
    if (ddt->drive_motor(TARGET_ID, i, motor_status)) {
      printf("error\n");
      continue;
    }
    print_motor_status(motor_status);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
	for (int i = 330; i > -330; i--) {
		if (ddt->drive_motor(TARGET_ID, i, motor_status)) {
			printf("error\n");
			continue;
		}
		print_motor_status(motor_status);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	for (int i = -330; i < 0; i++) {
		if (ddt->drive_motor(TARGET_ID, i, motor_status)) {
			printf("error\n");
			continue;
		}
		print_motor_status(motor_status);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

  return EXIT_SUCCESS;
}