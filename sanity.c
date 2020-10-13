
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

int
main(int argc, char *argv[]) {

  if (argc == 1) {
    printf(stdout, "\nUsage: - sanity n - where the number of created processes is n multiplied by 5\n");
    exit();
  }

  int n = atoi(argv[1]);

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

      printf(stdout, "Process pid: %d \n", child_pid);
      printf(stdout, "Process type: %s \n", getTypeName(child_pid%3));
      printf(stdout, "Running time: %d \n", curr_rutime);
      printf(stdout, "Sleeping time: %d \n", curr_stime);
      printf(stdout, "Runnable time: %d \n \n", curr_retime);

    }
  }



  exit();


}
