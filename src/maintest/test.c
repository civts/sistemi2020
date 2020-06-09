    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
     
    #define BUFSIZE 256
     
    void child_process(int pipe_pair[2]){
     
        // closes the child's STDOUT descriptor and replaces it with
        // the write pipe linked to the parent's read pipe
        if (dup2(pipe_pair[1], STDOUT_FILENO) == -1){ 
          // if dup2 fails, perror writes to stderr so this reports
          // appropriately either way as the child and parent share stderr
          perror("dup2"); 
          // parent will know if the child failed since we return 1
          // (ideally return errno so that calling processes know the error code)
          exit(EXIT_FAILURE); 
        }   
     
        // duplicated by dup2 above, no longer needed
        close(pipe_pair[1]); 
        // close read end as we will never read from stdout
        close(pipe_pair[0]); 
     
        execv("report.o",NULL);

        // make sure the write buffer is flushed before we exit
        fflush(stdout); 
     
        // close to make sure read() returns 0 in the parent
        close(STDOUT_FILENO); 
     
        // child exits
        exit(EXIT_SUCCESS); 
    }
     
    void parent_process(int pipe_pair[2], pid_t cpid){
     
        // cstatus will store the return of the child process
        // buf will hold the child's writes to stdout --
        // {0} initializes the array elements to 0x00
        int cstatus; 
        char buf[BUFSIZE] = {0}; 
     
        close(pipe_pair[1]); // we won't write to stdout
     
        // read until closed, or error (0 or -1, respectively)
        for (int n = 0; (n = read(pipe_pair[0], buf, BUFSIZE)) > 0;){ 
          
        //   printf("Received %d bytes from child process: ", n); 
        //   printf("%d\n");
          // (needed otherwise write() may output before 
          // printf since stdio output to stdout is line buffered)
          fflush(stdout); 
          // writes just what we read so no need to reset buf
          write(STDOUT_FILENO, buf, n); 
          printf("\n");
          fflush(stdout);
        }   
     
        // close read pipe
        close(pipe_pair[0]); 
     
        // waits for child process with pid 'cpid' to
        // return and stores the exit code in cstatus
        waitpid(cpid, &cstatus, 0); 
     
        printf("Child exit status was: %d\n", cstatus);
     
        // terminate parent
        exit(EXIT_SUCCESS); 
     
    }
     
    int main(int argc, char **argv){
     
      // cpid stores the process id of the child process
      // stdout_pipe array = pipe descriptor pair -- 
      // [0] is the read end, [1] is the write end
      pid_t cpid; 
      int stdout_pipe[2]; 
     
      // call that creates the two unidirectional pipe streams 
      // and stores the descriptors in the array
      if (pipe(stdout_pipe) == -1){ 
        perror("pipe");
        exit(EXIT_FAILURE);
      }
     
      // fork happens here, cpid will have the child's
      // process id or -1 if the call fails
      cpid = fork(); 
      if (cpid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
      }
     
      // child (fork returns 0 in the child and the child ID for the parent)
      if (cpid == 0) 
        child_process(stdout_pipe);
      // else when cpid is not 0 or -1 we're in the parent
      else 
        parent_process(stdout_pipe, cpid);
     
      // we shouldn't get here, but return int from main for correctness
      return 0;
    }