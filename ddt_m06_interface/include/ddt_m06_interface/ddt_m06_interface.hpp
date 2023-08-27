// Copyright 2023 Ar-ray-code
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


#pragma once

#include "ddt_m06_interface/usart.hpp"
#include <iostream>
#include <array>
#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <limits>
#include <memory>
#include <sstream>
#include <bitset>
#include <vector>

#define CURRENT_LOOP_MODE 0x01
#define VELOCITY_LOOP_MODE 0x02
#define ANGLE_LOOP_MODE 0x03

int open_port(void);

void configure_port(const int fd);

namespace ddt_m06_interface
{

class ddtM06Handler
{
public:
  ddtM06Handler(const std::string&);
  ~ddtM06Handler();

  void drive_motor(const uint8_t, const int16_t);
  void get_motor(const uint8_t);
  void switch_mode(const int);

private:
  int open_port(const std::string&);
  void configure_port(int);
  uint16_t angle2input(const int);
  int16_t current2input(const int);
  int16_t velocity2input(const int);

  float output2current(const uint16_t);
  int16_t output2velocity(const int16_t);
  uint16_t output2angle(const int16_t);

  int fd_;
  int current_mode_ = VELOCITY_LOOP_MODE;
  std::vector<Receiver> receive_buffer_;
  std::shared_ptr<MotorHandler> motor_handler_;

};

} // namespace ddt_m06_interface
