/*
@time: Jul 20, 2020
*/

#define AFL_MAIN
#define MESSAGES_TO_STDOUT

#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64

#ifdef __cplusplus
extern "C" {
#endif
#include "config.h"
#include "types.h"
#include "debug.h"
#include "alloc-inl.h"
#include "hash.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <ctype.h>
#include <fcntl.h>

#include <string.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#ifdef __cplusplus
}
#endif

#include <vector>
#include <string>
#include <iostream>

static char in_dir[256];
static char out_dir[256];
static int max_trials;
static long long start_time;

struct queue_entry {
  u8 fname[256];                          /* File name for the test case      */
  u32 len;                            /* Input length                     */

  u8  cal_failed,                     /* Calibration failed?              */
      trim_done,                      /* Trimmed?                         */
      was_fuzzed,                     /* Had any fuzzing done yet?        */
      passed_det,                     /* Deterministic stages passed?     */
      has_new_cov,                    /* Triggers new coverage?           */
      var_behavior,                   /* Variable behavior?               */
      favored,                        /* Currently favored?               */
      fs_redundant;                   /* Marked as redundant in the fs?   */

  u32 bitmap_size,                    /* Number of bits set in bitmap     */
      exec_cksum;                     /* Checksum of the execution trace  */

  u64 exec_us,                        /* Execution time (us)              */
      handicap,                       /* Number of queue cycles behind    */
      depth;                          /* Path depth                       */

  u8* trace_mini;                     /* Trace bytes, if kept             */
  u32 tc_ref;                         /* Trace bytes ref count            */

  struct queue_entry *next,           /* Next element, if any             */
                     *next_100;       /* 100 elements ahead               */

};

static std::vector<queue_entry*> input_queue; /* Fuzzing queue */

static u8* trace_bits;                /* SHM with instrumentation bitmap  */
static int child_pid = -1;            /* PID of the fuzzed program        */
static int shm_id;                    /* SHM ID */
static u16 count_class_lookup16[65536];
static FILE* plot_file; 

static s32 out_fd,
           dev_urandom_fd = -1,
           out_dir_fd = -1,
           dev_null_fd = -1;

static u8 *out_file;

/* Display usage hints. */

static void usage(char* argv0) {

  SAYF("Usage: \n%s input_dir output_dir max_trials /path/to/fuzzed_app \n\n", argv0);

  exit(1);

}

/* Make a copy of the current command line. */

static void save_cmdline(u32 argc, char** argv) {

  u32 len = 1, i;
  char* buf;

  for (i = 0; i < argc; i++)
    len += strlen(argv[i]) + 1;
  
  buf = ck_alloc(len);

  for (i = 0; i < argc; i++) {

    u32 l = strlen(argv[i]);
//    printf("%s\n", argv[i]);
    memcpy(buf, argv[i], l);
    buf += l;

    if (i != argc - 1) *(buf++) = ' ';

  }

  *buf = 0;

}

void fuzzing(char* app){

}


#ifdef __x86_64__

static inline void classify_counts(u64* mem) {

  u32 i = MAP_SIZE >> 3;

  while (i--) {

    /* Optimize for sparse bitmaps. */

    if (unlikely(*mem)) {

      u16* mem16 = (u16*)mem;

      mem16[0] = count_class_lookup16[mem16[0]];
      mem16[1] = count_class_lookup16[mem16[1]];
      mem16[2] = count_class_lookup16[mem16[2]];
      mem16[3] = count_class_lookup16[mem16[3]];

    }

    mem++;

  }

}

#else

#endif /* ^__x86_64__ */

/* Perform dry run of all test cases to confirm that the app is working as
   expected. This is done only for the initial inputs, and only once. */

void perform_dry_run(char* app){
  ACTF("Attempting dry run with '%s'...", app);

  int status = 0;
  int st_pipe[2], ctl_pipe[2];
  memset(trace_bits, 0, MAP_SIZE);

  char* argv[] = {app, "", NULL};
  pid_t pid;
  printf("Before wait %d\n",*(u32*)trace_bits);
  child_pid = fork();

  if(child_pid < 0){
    perror("fork error.");
    exit(EXIT_FAILURE);
  }
  
  if(!child_pid){ // This is child process
    printf("This is the child process");
    struct rlimit r;

    if (!getrlimit(RLIMIT_NOFILE, &r) && r.rlim_cur < FORKSRV_FD + 2) {

      r.rlim_cur = FORKSRV_FD + 2;
      setrlimit(RLIMIT_NOFILE, &r); /* Ignore errors */

    }

    /* Dumping cores is slow and can lead to anomalies if SIGKILL is delivered
       before the dump is complete. */

    r.rlim_max = r.rlim_cur = 0;

    setrlimit(RLIMIT_CORE, &r); /* Ignore errors */

    /* Isolate the process and configure standard descriptors. If out_file is
       specified, stdin is /dev/null; otherwise, out_fd is cloned instead. */

    setsid();

    dup2(dev_null_fd, 1);
    dup2(dev_null_fd, 2);

    if (out_file) {

      dup2(dev_null_fd, 0);

    } else {

      dup2(out_fd, 0);
      close(out_fd);

    }

    /* Set up control and status pipes, close the unneeded original fds. */

    if (dup2(ctl_pipe[0], FORKSRV_FD) < 0) PFATAL("dup2() failed");
    if (dup2(st_pipe[1], FORKSRV_FD + 1) < 0) PFATAL("dup2() failed");

    close(ctl_pipe[0]);
    close(ctl_pipe[1]);
    close(st_pipe[0]);
    close(st_pipe[1]);

    close(out_dir_fd);
    close(dev_null_fd);
    close(dev_urandom_fd);
    close(fileno(plot_file));

    /* This should improve performance a bit, since it stops the linker from
       doing extra work post-fork(). */

    if (!getenv("LD_BIND_LAZY")) setenv("LD_BIND_NOW", "1", 0);

    /* Set sane defaults for ASAN if nothing else specified. */

    setenv("ASAN_OPTIONS", "abort_on_error=1:"
                           "detect_leaks=0:"
                           "symbolize=0:"
                           "allocator_may_return_null=1", 0);

    /* MSAN is tricky, because it doesn't support abort_on_error=1 at this
       point. So, we do this in a very hacky way. */

    setenv("MSAN_OPTIONS", "exit_code=" STRINGIFY(MSAN_ERROR) ":"
                           "symbolize=0:"
                           "abort_on_error=1:"
                           "allocator_may_return_null=1:"
                           "msan_track_origins=0", 0);

    execv(app, argv);
    *(u32*)trace_bits = EXEC_FAIL_SIG;
    exit(0);
  }

  pid_t ret;
  ret = waitpid(child_pid, &status, 0);
  if(ret < 0){
    perror("wait error");
    exit(EXIT_FAILURE);
  }
  

  int tb4 = *(u32*)trace_bits;

  printf("after wait %d\n",*(u32*)trace_bits);
  u32 ck1 = hash32(trace_bits, MAP_SIZE, HASH_CONST);
  SAYF("aaaa %d\n", ck1);

  if (!WIFSTOPPED(status)) child_pid = 0;

  #ifdef __x86_64__
    classify_counts((u64*)trace_bits);
  #else
    classify_counts((u32*)trace_bits);  
  #endif /* ^__x86_64__ */
  
  u32 ck2 = hash32(trace_bits, MAP_SIZE, HASH_CONST);
  SAYF("bbbbbb %d\n", ck2);

  if (WIFEXITED(status))
        printf("child exited normal exit status= %d\n", WEXITSTATUS(status));

    else if (WIFSIGNALED(status))
        printf("child exited abnormal signal number= %d \n", WTERMSIG(status));
    else if (WIFSTOPPED(status))
        printf("child stoped signal number=%d\n", WSTOPSIG(status));

}

static void list_dir(const char *path)
{
    struct dirent *entry;

    DIR *dir = opendir(path);
    if (dir == NULL) {
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if(entry->d_name[0]=='.')
            continue;
        struct queue_entry *q = (struct queue_entry *)malloc(sizeof(queue_entry));
        //q->fname = entry->d_name;
        memcpy(q->fname, entry->d_name, strlen(entry->d_name));
        input_queue.push_back(q);
    }

    closedir(dir);
}

static void setup_shm(){

  printf("setup_shm enter!\n");
  u8* shm_str;

  shm_id = shmget(IPC_PRIVATE, MAP_SIZE, IPC_CREAT | IPC_EXCL | 0600);
  
  if (shm_id < 0) PFATAL("Fialed to creat a shared memory");
  
  shm_str = alloc_printf("%d", shm_id);
  
  trace_bits = shmat(shm_id, NULL, 0);
  if (!trace_bits) PFATAL("shmat() failed");

  printf("setup_shm Successfully!\n");
}

/* Main entry point */

int main(int argc, char** argv) {

  SAYF(cCYA "random-fuzz " cBRI VERSION cRST " by\n");

  if(argc < 5)  usage(argv[0]);
  memcpy(in_dir, argv[1], strlen(argv[1]));
  memcpy(out_dir, argv[2], strlen(argv[2]));
  max_trials = atoi(argv[3]);


  if (!strcmp(in_dir, out_dir))
    FATAL("Input and output directories can't be the same");
  
  setup_shm();
  OKF("Shared memory is ready.");
  u32 ck1 = hash32(trace_bits, MAP_SIZE, HASH_CONST);
  SAYF("main cksum %d\n", ck1);
  
  list_dir(in_dir);
  OKF("Input queue initialized with %d seeds.", input_queue.size());


  save_cmdline(argc, argv);
  OKF("Command line saved.");

  
//  start_time = get_cur_time();
//  SAYF("The start time is: %lld", start_time);

  OKF("Perform dry run!");
  perform_dry_run(argv[4]);
  
  OKF("Start fuzzing!");
  fuzzing(argv[4]);

  OKF("We're done here. Have a nice day!\n");

//  FILE* f = fopen("1.dat", "w");
//  for(int i = 0; i < MAP_SIZE; i++)
//  {
//    fprintf(f, "%d ", trace_bits[i]);
//  }
//  fclose(f);

  exit(0);

}
