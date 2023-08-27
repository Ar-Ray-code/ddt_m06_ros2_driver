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


#include "ddt_m06_interface/ddt_m06_interface.hpp"

namespace ddt_m06_interface
{

ddtM06Handler::ddtM06Handler(const std::string &port_name)
{
  this->fd_ = open_port(port_name);
  configure_port(this->fd_);
  this->motor_handler_ = std::make_shared<MotorHandler>(this->fd_);
}

int ddtM06Handler::open_port(const std::string &port_name)
{
  int fd = open(port_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1)
  {
    printf("open_port: Unable to open %s. \n", port_name.c_str());
  }
  else
  {
    fcntl(fd, F_SETFL, 0);
    printf("[%d] port is open.\n", fd);
  }
  return fd;
}

void ddtM06Handler::configure_port(int fd)
{
  struct termios port_settings;

  cfsetispeed(&port_settings, B115200);
  cfsetospeed(&port_settings, B115200);

  port_settings.c_cflag &= ~PARENB;
  port_settings.c_cflag &= ~CSTOPB;
  port_settings.c_cflag &= ~CSIZE;
  port_settings.c_cflag |= CS8;
  port_settings.c_cflag &= ~CRTSCTS;

  port_settings.c_cc[VTIME] = 1;
  port_settings.c_cc[VMIN] = 0;

  if (tcsetattr(fd, TCSANOW, &port_settings) != 0)
  {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    exit(1);
  }
}

void ddtM06Handler::drive_motor(const uint8_t id, const int16_t value)
{
  uint8_t Acc = 0;
  uint8_t Brake_P = 0;
  Receiver received_data;

  switch (this->current_mode_)
  {
  case CURRENT_LOOP_MODE:
    this->motor_handler_->Control_Motor(this->current2input(value), id, Acc, Brake_P, &received_data);
    break;
  case VELOCITY_LOOP_MODE:
    this->motor_handler_->Control_Motor(this->velocity2input(value), id, Acc, Brake_P, &received_data);
    break;
  case ANGLE_LOOP_MODE:
    this->motor_handler_->Control_Motor(this->angle2input(value), id, Acc, Brake_P, &received_data);
    break;
  }
}


// input data ============================
uint16_t ddtM06Handler::angle2input(const int angle)
{
  return (uint16_t)(angle * 32767 / 360);
}

int16_t ddtM06Handler::current2input(const int current)
{
  return (uint16_t)(current * 32767 / 8);
}

int16_t ddtM06Handler::velocity2input(const int velocity)
{
  if (velocity > 330)
  {
    return 330;
  }
  else if (velocity < -330)
  {
    return -330;
  }
  else
  {
    return (int16_t)velocity;
  }
}

// output data ============================
float output2current(const uint16_t data)
{
  // 0x40
  if (data > 0x4000)
  {
    return ((float)(data - 0x7f7f) * 8 / 32767);
  }
  else
  {
    return (float)data * 8 / 32767;
  }
}
int16_t ddtM06Handler::output2velocity(const int16_t data)
{
  if (data > 0x4000)
  {
    return (int16_t)((data - 0x7f7f));
  }
  else
  {
    return (int16_t)data;
  }
}
uint16_t ddtM06Handler::output2angle(const int16_t data)
{
  return (uint16_t)(data * 360 / 32767);
}

void ddtM06Handler::switch_mode(const int mode)
{
  this->current_mode_ = mode;
}

} // namespace ddt_m06_interface
