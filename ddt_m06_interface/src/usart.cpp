// Copyright 2022 HarvestX Inc.
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


#include "ddt_m06_interface/usart.hpp"

MotorHandler::MotorHandler(int serial_port) : serial_port_(serial_port)
{
}



void MotorHandler::Control_Motor(uint16_t Speed, uint8_t ID, uint8_t Acce, uint8_t Brake_P,  Received &receiver)
{
  this->Tx[0] = ID;
  this->Tx[1] = 0x64;
  this->Tx[2] = Speed >> 8;
  this->Tx[3] = Speed & 0x00ff;
  this->Tx[4] = 0;
  this->Tx[5] = 0;
  this->Tx[6] = Acce;
  this->Tx[7] = Brake_P;
  this->Tx[8] = 0;
  this->Tx[9] = CRC8_Table(Tx.data(), 9);
  Send_Motor();

  Receive_Motor();
  receiver.BMode = this->Rx[1];
  receiver.ECurrent = (this->Rx[2] << 8) + this->Rx[3];
  receiver.BSpeed = (this->Rx[4] << 8) + this->Rx[5];
  receiver.Position = (this->Rx[6] << 8) + this->Rx[7];
  receiver.ErrCode = this->Rx[8];
}

void MotorHandler::Get_Motor(uint8_t ID, Received &receiver)
{
  Tx[0] = ID;
  Tx[1] = 0x74;
  Tx[2] = 0;
  Tx[3] = 0;
  Tx[4] = 0;
  Tx[5] = 0;
  Tx[6] = 0;
  Tx[7] = 0;
  Tx[8] = 0;
  Tx[9] = CRC8_Table(Tx.data(), 9);
  Send_Motor();

  Receive_Motor();
  receiver.BMode = Rx[1];
  receiver.ECurrent = (Rx[2] << 8) + Rx[3];
  receiver.BSpeed = (Rx[4] << 8) + Rx[5];
  receiver.Temp = Rx[6];
  receiver.Position = Rx[7];
  receiver.ErrCode = Rx[8];
}

void MotorHandler::Set_MotorMode(uint8_t Mode, uint8_t ID)
{
  Tx[0] = ID;
  Tx[1] = 0xA0;
  Tx[2] = 0;
  Tx[3] = 0;
  Tx[4] = 0;
  Tx[5] = 0;
  Tx[6] = 0;
  Tx[7] = 0;
  Tx[8] = 0;
  Tx[9] = Mode;
  Send_Motor();
}

void MotorHandler::Set_MotorID(uint8_t ID)
{
  Tx[0] = 0xAA;
  Tx[1] = 0x55;
  Tx[2] = 0x53;
  Tx[3] = ID;
  Tx[4] = 0;
  Tx[5] = 0;
  Tx[6] = 0;
  Tx[7] = 0;
  Tx[8] = 0;
  Tx[9] = 0;
  Send_Motor();
}

void MotorHandler::Check_Motor(Received &receiver)
{
  Tx[0] = 0xc8;
  Tx[1] = 0x64;
  Tx[2] = 0;
  Tx[3] = 0;
  Tx[4] = 0;
  Tx[5] = 0;
  Tx[6] = 0;
  Tx[7] = 0;
  Tx[8] = 0;
  Tx[9] = CRC8_Table(Tx.data(), 9);

  Send_Motor();
  Receive_Motor();

  receiver.BMode = Rx[1];
  receiver.ECurrent = (Rx[2] << 8) + Rx[3];
  receiver.BSpeed = (Rx[4] << 8) + Rx[5];
  receiver.Position = (Rx[6] << 8) + Rx[7];
  receiver.ErrCode = Rx[8];
}

void MotorHandler::Send_Motor()
{
  write(this->serial_port_, Tx.data(), 10);
}

void MotorHandler::Receive_Motor()
{
  int num_byte = read(this->serial_port_, Rx.data(), 10);
  if (num_byte < 0)
  {
    printf("Error reading: %s", strerror(errno));
    exit(1);
  }
}

unsigned char MotorHandler::CRC8_Table(uint8_t *p, int counter)
{
  unsigned char crc8 = 0;
  for (; counter > 0; counter--)
  {
    crc8 = CRC8Table[crc8 ^ *p];
    p++;
  }
  return (crc8);
}
