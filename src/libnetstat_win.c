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

#include "libnetstat.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

struct SocketConnection *GetActiveConnections(size_t *size) {
  // TCP, IPv4
  int tcp_v4_buffer_size = 0;
  if (ERROR_INSUFFICIENT_BUFFER != GetTcpTable2(NULL, (ULONG*)&tcp_v4_buffer_size, false)) {
    printf("Error fetching buffer size");
    return NULL;
  }

  MIB_TCPTABLE2* tcp_v4_table = (MIB_TCPTABLE2*)malloc(tcp_v4_buffer_size);

  if (NO_ERROR != GetTcpTable2(tcp_v4_table, (ULONG*)&tcp_v4_buffer_size, false)) {
    free(tcp_v4_table);
    printf("Error fetching TCP IPv4 table");
    return NULL;
  }

  // TCP, IPv6
  int tcp_v6_buffer_size = 0;
  if (ERROR_INSUFFICIENT_BUFFER != GetTcp6Table2(NULL, (PULONG)&tcp_v6_buffer_size, false)) {
    free(tcp_v4_table);
    printf("Error fetching buffer size");
    return NULL;
  }

  PMIB_TCP6TABLE2 tcp_v6_table = (PMIB_TCP6TABLE2)malloc(tcp_v6_buffer_size);
  if (NO_ERROR != GetTcp6Table2(tcp_v6_table, (PULONG)&tcp_v6_buffer_size, false)) {
    free(tcp_v4_table);
    free(tcp_v6_table);
    printf("Error fetching TCP IPv6 table");
    return NULL;
  }

  // UDP IPv4
  int udp_v4_buffer_size = 0;
  if (ERROR_INSUFFICIENT_BUFFER != GetExtendedUdpTable(NULL, (ULONG*)&udp_v4_buffer_size, false, AF_INET, UDP_TABLE_OWNER_PID, 0)) {
    free(tcp_v4_table);
    free(tcp_v6_table);
    printf("Error fetching buffer size");
    return NULL;
  }

  MIB_UDPTABLE_OWNER_PID* udp_v4_table = (MIB_UDPTABLE_OWNER_PID*)malloc(udp_v4_buffer_size);
  if (NO_ERROR != GetExtendedUdpTable(udp_v4_table, (ULONG*)&udp_v4_buffer_size, false, AF_INET, UDP_TABLE_OWNER_PID, 0)) {
    free(tcp_v4_table);
    free(tcp_v6_table);
    free(udp_v4_table);
    printf("Error fetching UDP IPv4 table");
    return NULL;
  }

  // UDP IPv6
  int udp_v6_buffer_size = 0;
  if (ERROR_INSUFFICIENT_BUFFER != GetExtendedUdpTable(NULL, (ULONG*)&udp_v6_buffer_size, false, AF_INET6, UDP_TABLE_OWNER_PID, 0)) {
    free(tcp_v4_table);
    free(tcp_v6_table);
    free(udp_v4_table);
    printf("Error fetching UDP IPv6 buffer size");
    return NULL;
  }

  MIB_UDP6TABLE_OWNER_PID* udp_v6_table = (MIB_UDP6TABLE_OWNER_PID*)malloc(udp_v6_buffer_size);
  if (NO_ERROR != GetExtendedUdpTable(udp_v6_table, (ULONG*)&udp_v6_buffer_size, false, AF_INET6, UDP_TABLE_OWNER_PID, 0)) {
    free(tcp_v4_table);
    free(tcp_v6_table);
    free(udp_v4_table);
    free(udp_v6_table);
    printf("Error fetching UDP IPv6 table");
    return NULL;
  }

  *size = tcp_v4_table->dwNumEntries + tcp_v6_table->dwNumEntries + udp_v4_table->dwNumEntries + udp_v6_table->dwNumEntries;

  struct SocketConnection *connection_table = (struct SocketConnection*)malloc(*size * sizeof(struct SocketConnection));

  UINT connection_index = 0;
  for (UINT i = 0; i < tcp_v4_table->dwNumEntries; i++) {
    MIB_TCPROW2* row = &tcp_v4_table->table[i];
    struct SocketConnection *con = &connection_table[connection_index];

    con->protocol           = TCPv4;
    con->state              = row->dwState;
    con->local_address      = *(struct in_addr*)&row->dwLocalAddr;
    con->local_address_v6   = EmptyAddr6;
    con->local_port         = row->dwLocalPort;
    con->remote_address     = *(struct in_addr*)&row->dwRemoteAddr;
    con->remote_address_v6  = EmptyAddr6;
    con->remote_port        = row->dwRemotePort;
    con->pid                = row->dwOwningPid;
    con->offload_state      = row->dwOffloadState;

    connection_index++;
  }

  free(tcp_v4_table);

  for (UINT i = 0; i < tcp_v6_table->dwNumEntries; i++) {
    MIB_TCP6ROW2* row = &tcp_v6_table->table[i];
    struct SocketConnection *con = &connection_table[connection_index];

    con->protocol           = TCPv6;
    con->state              = row->State;
    con->local_address      = EmptyAddr4;
    con->local_address_v6   = row->LocalAddr;
    con->local_port         = row->dwLocalPort;
    con->remote_address     = EmptyAddr4;
    con->remote_address_v6  = row->RemoteAddr;
    con->remote_port        = row->dwRemotePort;
    con->pid                = row->dwOwningPid;
    con->offload_state      = row->dwOffloadState;

    connection_index++;
  }

  free(tcp_v6_table);

  for (UINT i = 0; i < udp_v4_table->dwNumEntries; i++) {
    MIB_UDPROW_OWNER_PID* row = &udp_v4_table->table[i];
    struct SocketConnection *con = &connection_table[connection_index];

    con->protocol           = UDPv4;
    con->state              = 0;
    con->local_address      = *(struct in_addr*)&row->dwLocalAddr;
    con->local_address_v6   = EmptyAddr6;
    con->local_port         = row->dwLocalPort;
    con->remote_address     = EmptyAddr4;
    con->remote_address_v6  = EmptyAddr6;
    con->remote_port        = 0;
    con->pid                = row->dwOwningPid;
    con->offload_state      = 0;

    connection_index++;
  }

  free(udp_v4_table);

  for (UINT i = 0; i < udp_v6_table->dwNumEntries; i++) {
    MIB_UDP6ROW_OWNER_PID* row = &udp_v6_table->table[i];
    struct SocketConnection *con = &connection_table[connection_index];

    con->protocol           = UDPv6;
    con->state              = 0;
    con->local_address      = EmptyAddr4;
    con->local_address_v6   = *(struct in_addr6*)&row->ucLocalAddr;
    con->local_port         = row->dwLocalPort;
    con->remote_address     = EmptyAddr4;
    con->remote_address_v6  = EmptyAddr6;
    con->remote_port        = 0;
    con->pid                = row->dwOwningPid;
    con->offload_state      = 0;

    connection_index++;
  }

  free(udp_v6_table);

  return connection_table;
}
