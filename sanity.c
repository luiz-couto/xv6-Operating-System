
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

#define MILLION_LOOP 1000000
#define TEN_THOUSAND 10000
#define HUNDRED_LOOP 100

int stdout = 1;

// CPU-Bound
void processTypeZero() {
  for (int i=0; i<HUNDRED_LOOP; i++) {
    for (int j=0; j<MILLION_LOOP; j++) {
      asm("nop");
    }
  }
}

// S-CPU
void processTypeOne() {
  for (int i=0; i<HUNDRED_LOOP; i++) {
    int count = 0;
    for (int j=0; j<MILLION_LOOP; j++) {

      if (count == TEN_THOUSAND) {
        yieldCall();
        count = 0;
        continue;
      }

      asm("nop");
      count++;
    }
  }
}

// IO-BOUND
void processTypeTwo() {
  for (int i=0; i<HUNDRED_LOOP; i++) {
    sleep(1);
  }
}

const char* getTypeName(int type) {
  if (type == 0) return "CPU-Bound";
  else if (type == 1) return "S-CPU";
  else if (type == 2) return "IO-Bound";
  return "";
}

void printStatistics(char *name, int vec[], int bound_count, int s_count, int io_count) {
  printf(stdout, "Tempo médio %s: \n", name);
  int selected_type;
  for (int i=0; i<3; i++) {
    if (i == 0) selected_type = bound_count;
    else if (i == 1) selected_type = s_count;
    else if (i == 2) selected_type = io_count;

    printf(stdout, "%s: %d\n", getTypeName(i), vec[i]/selected_type);

  }
  printf(stdout, "\n");
}


int
main(int argc, char *argv[]) {

  if (argc == 1) {
    printf(stdout, "\nUsage: - sanity [n] - where the number of created processes is n multiplied by 5\n\n");
    exit();
  }

  int n = atoi(argv[1]);

  if (n <= 0) {
    printf(stdout, "\nn needs to be a positive number! \n");
    printf(stdout, "\nUsage: - sanity [n] - where the number of created processes is n multiplied by 5\n\n");
    exit();
  }

  int bound_count = 0; int s_count = 0; int io_count = 0;
  int ready_sum[3], run_sum[3], sleep_sum[3];
  for (int i=0; i<3; i++) {
    ready_sum[i] = 0;
    run_sum[i] = 0;
    sleep_sum[i] = 0;
  }


  for (int i=0; i<5; i++) {
    for (int j=0; j<n; j++) {
      
      int pid = fork();
      int curr_retime, curr_rutime, curr_stime;
      int curr_pid = getpid();
      int type = curr_pid%3;
      
      if (pid == 0) {
        
        if (type == 0) processTypeZero();
        else if (type == 1) processTypeOne();
        else if (type == 2) processTypeTwo(); 

        exit();

      }

      int child_pid = wait2(&curr_retime, &curr_rutime, &curr_stime);
      int child_type = child_pid%3;

      if (child_type == 0) bound_count++;
      else if (child_type == 1) s_count++;
      else if (child_type == 2) io_count++;

      ready_sum[child_type] = ready_sum[child_type] + curr_retime;
      run_sum[child_type] = run_sum[child_type] + curr_rutime;
      sleep_sum[child_type] = sleep_sum[child_type] + curr_stime;

      printf(stdout, "Process pid: %d \n", child_pid);
      printf(stdout, "Process type: %s \n", getTypeName(child_type));
      printf(stdout, "Running time: %d \n", curr_rutime);
      printf(stdout, "Sleeping time: %d \n", curr_stime);
      printf(stdout, "Runnable time: %d \n \n", curr_retime);

    }
  }

  printf(stdout, "////////////////////////////////////////////// \n\n");
  printStatistics("RUNNABLE", ready_sum, bound_count, s_count, io_count);
  printStatistics("RUNNING", run_sum, bound_count, s_count, io_count);
  printStatistics("SLEEPING", sleep_sum, bound_count, s_count, io_count);

  int turnaround_sum[3];
  for (int i=0; i<3; i++) {
    turnaround_sum[i] = ready_sum[i] + run_sum[i] + sleep_sum[i];
  }
  printStatistics("Turnaround (Total)", turnaround_sum, bound_count, s_count, io_count);
  printf(stdout, "////////////////////////////////////////////// \n");

  exit();

}
