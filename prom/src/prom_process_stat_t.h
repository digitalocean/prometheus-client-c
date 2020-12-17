/**
 * Copyright 2019-2020 DigitalOcean Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PROM_PROCESS_STATS_T_H
#define PROM_PROCESS_STATS_T_H

#include "prom_gauge.h"
#include "prom_procfs_t.h"

extern prom_gauge_t *prom_process_cpu_seconds_total;
extern prom_gauge_t *prom_process_virtual_memory_bytes;
extern prom_gauge_t *prom_process_resident_memory_bytes;
extern prom_gauge_t *prom_process_start_time_seconds;

/**
 * @brief Refer to man proc and search for /proc/[pid]/stat
 */
typedef struct prom_process_stat {
  int pid;                                   // (1) pid  %d
  char *comm;                                // (2) comm  %s
  char state;                                // (3) state  %c
  int ppid;                                  // (4) ppid  %d
  int pgrp;                                  // (5) pgrp  %d
  int session;                               // (6) session  %d
  int tty_nr;                                // (7) tty_nr  %d
  int tpgid;                                 // (8) tpgid  %d
  unsigned flags;                            // (9) flags  %u
  unsigned long minflt;                      // (10) minflt  %lu
  unsigned long cminflt;                     // (11) cminflt  %lu
  unsigned long majflt;                      // (12) majflt  %lu
  unsigned long cmajflt;                     // (13) cmajflt  %lu
  unsigned long utime;                       // (14) utime  %lu
  unsigned long stime;                       // (15) stime  %lu
  long int cutime;                           // (16) cutime  %ld
  long int cstime;                           // (17) cstime  %ld
  long int priority;                         // (18) priority  %ld
  long int nice;                             // (19) nice  %ld
  long int num_threads;                      // (20) num_threads  %ld
  long int itrealvalue;                      // (21) itrealvalue  %ld
  unsigned long long starttime;              // (22) starttime  %llu
  unsigned long vsize;                       // (23) vsize  %lu
  long int rss;                              // (24) rss  %ld
  unsigned long rsslim;                      // (25) rsslim  %lu
  unsigned long startcode;                   // (26) startcode  %lu  [PT]
  unsigned long endcode;                     // (27) endcode  %lu  [PT]
  unsigned long startstack;                  // (28) startstack  %lu  [PT]
  unsigned long kstkesp;                     // (29) kstkesp  %lu  [PT]
  unsigned long kstkeip;                     // (30) kstkeip  %lu  [PT]
  unsigned long signal;                      // (31) signal  %lu
  unsigned long blocked;                     // (32) blocked  %lu
  unsigned long sigignore;                   // (33) sigignore  %lu
  unsigned long sigcatch;                    // (34) sigcatch  %lu
  unsigned long wchan;                       // (35) wchan  %lu  [PT]
  unsigned long nswap;                       // (36) nswap  %lu
  unsigned long cnswap;                      // (37) cnswap  %lu
  int exit_signal;                           // (38) exit_signal  %d  (since Linux 2.1.22)
  int processor;                             // (39) processor  %d  (since Linux 2.2.8)
  unsigned rt_priority;                      // (40) rt_priority  %u  (since Linux 2.5.19)
  unsigned policy;                           // (41) policy  %u  (since Linux 2.5.19)
  unsigned long long delayacct_blkio_ticks;  // (42) delayacct_blkio_ticks
  unsigned long guest_time;                  // (43) guest_time  %lu  (since Linux 2.6.24)
  long int cguest_time;                      // (44) cguest_time  %ld  (since Linux 2.6.24)
  unsigned long start_data;                  // (45) start_data  %lu  (since Linux 3.3)  [PT]
  unsigned long end_data;                    // (46) end_data  %lu  (since Linux 3.3)  [PT]
  unsigned long start_brk;                   // (47) start_brk  %lu  (since Linux 3.3)  [PT]
  unsigned long arg_start;                   // (48) arg_start  %lu  (since Linux 3.5)  [PT]
  unsigned long arg_end;                     // (49) arg_end  %lu  (since Linux 3.5)  [PT]
  unsigned long env_start;                   // (50) env_start  %lu  (since Linux 3.5)  [PT]
  unsigned long env_end;                     // (51) env_end  %lu  (since Linux 3.5)  [PT]
  int exit_code;                             // (52) exit_code  %d  (since Linux 3.5)  [PT]
} prom_process_stat_t;

typedef prom_procfs_buf_t prom_process_stat_file_t;

#endif  // PROM_PROCESS_STATS_T_H
