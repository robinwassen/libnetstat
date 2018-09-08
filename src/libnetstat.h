/*
 * Copyright 2018 Robin Andersson <me@robinwassen.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Credits:
 *    Arvydas Sidorenko for making bsd_var.h available
 *    https://github.com/Asido/SystemMonitor/blob/master/SystemMonitor/Network/bsd_var.h
 */

#ifndef SRC_LIBNETSTAT_H_
#define SRC_LIBNETSTAT_H_

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#elif __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

enum Protocol {
  TCPv4 = 0,
  TCPv6 = 1,
  UDPv4 = 2,
  UDPv6 = 3
};

struct SocketConnection {
  enum Protocol    protocol;
  int              state;
  struct in_addr   local_address;
  struct in6_addr  local_address_v6;
  int              local_port;
  struct in_addr   remote_address;
  struct in6_addr  remote_address_v6;
  int              remote_port;
  int              pid;
  int              offload_state;
};

static const struct in_addr   EmptyAddr4;
static const struct in6_addr  EmptyAddr6;

struct SocketConnection *GetActiveConnections(size_t *size);

#endif  // SRC_LIBNETSTAT_H_
