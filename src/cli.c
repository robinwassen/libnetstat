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
 */

#include <stdio.h>

#include "libnetstat.h"

void printV4(struct in_addr addr) {
  char ipv4Addr[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(addr), ipv4Addr, INET_ADDRSTRLEN);
  printf("%-39s ", ipv4Addr);
}

void printV6(struct in_addr6 addr) {
  char ipv6Addr[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &(addr), ipv6Addr, INET6_ADDRSTRLEN);
  printf("%-39s ", ipv6Addr);
}

void printFriendlyProtocolName(enum Protocol proto) {
  switch (proto) {
    case TCPv4:
      printf("%-10s ", "TCPv4");
      break;
    case TCPv6:
      printf("%-10s ", "TCPv6");
      break;
    case UDPv4:
      printf("%-10s ", "UDPv4");
      break;
    case UDPv6:
      printf("%-10s ", "UDPv6");
      break;
  }
}

int main() {
  printf("%-10s %-39s %-11s %-39s %-11s %-10s %-3s\n",
    "protocol",
    "local_address",
    "local_port",
    "remote_address",
    "remote_port",
    "pid",
    "offload_state");

  size_t connection_table_size;
  struct SocketConnection *connection_table = GetActiveConnections(&connection_table_size);

  for (size_t i = 0; i < connection_table_size; i++) {
    struct SocketConnection* sc = &connection_table[i];

    printFriendlyProtocolName(sc->protocol);

    if (sc->protocol == 1 || sc->protocol == 3) {
      printV6(sc->local_address_v6);
      printf("%-11d ", sc->local_port);
      printV6(sc->remote_address_v6);
      printf("%-11d ", sc->remote_port);
    } else {
      printV4(sc->local_address);
      printf("%-11d ", sc->local_port);
      printV4(sc->remote_address);
      printf("%-11d ", sc->remote_port);
    }

    printf("%-10d ", sc->pid);
    printf("%-3d\n", sc->offload_state);
  }

  free(connection_table);
  return 0;
}
