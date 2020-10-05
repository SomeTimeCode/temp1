#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>



int main(int argc, char* argv[]) 
{
	printf("This is the BEGINNING of the program.\n\n");
	if(argc-1 != 3){
		printf("Error: The number of input integers now is %d. Please input 3 integers.\n",argc-1);
		return -1;
	}// Don't modify this Error Checking part

	{ 
    //The number of bytes of shared memory it successfully requested
    int shmid;
    int size_data = 3 * sizeof(int); 
      shmid = shmget(IPC_PRIVATE, size_data, 0666|IPC_CREAT);
    pid_t fpid = fork();
    if(fpid < 0){
      printf("Error in executing fork\n");
    }else if(fpid > 0){
      //parent process
      kill(fpid, SIGSTOP);
      printf ("Apply %d bytes.\n", size_data);
      //Parent’s PID and the differences of the three integers;
      int *shmc = shmat(shmid, NULL, 0);
      int differ_0 = atoi(argv[2]) - atoi(argv[1]);
      int differ_1 = atoi(argv[3]) - atoi(argv[2]);
      printf("Parent process ID: %d.\n",getpid());
      printf("Differences: %d, %d.\n", differ_0,  differ_1);
      shmc[0] = differ_0;
      shmc[1] = differ_1;
      shmdt(shmc);
      //The child’s PID which will receive the signal and the type of the signal sent by the parent;
      printf("Send a SIGCONT to process %d.\n\n", fpid);
      kill(fpid, SIGCONT);
      
      //The exited child’s PID (returned from wait())
      pid_t pid_wait = wait(NULL);
      printf("Exited Process ID: %d.\n", pid_wait);
      shmctl(shmid, IPC_RMID, NULL);
    }else{
      //child process denoted as process A
      // raise(SIGSTOP);
      printf("Receive a SIGCONT.\n"); 
      printf("Child process ID: %d.\n", getpid());
      int *shmp = shmat(shmid, 0, 0);
      int diff_2 = shmp[0] + shmp[1];
      shmp[2] = diff_2;
      printf("Sum of differences: %d.\n\n", diff_2);
      shmdt(shmp);
      exit(0);
    }
	}
	printf("This is the END of the program.\n");
	return 0;
} 



// Plase use the following lines to print related information if needed.
/*
sleep(0.001);  // which you may need for registration of signal handlers in child process, 
               //only allowd at the beginning of the parent process after fork();
printf ("Apply %d bytes.\n",***);
printf("Child process A ID: %d.\n", getpid());
printf("Child process B ID: %d.\n", getpid());
printf("Sum of differences: %d.\n\n", ***);
printf("Send a SIGCONT to Process %d.\n\n", ***); 
printf("Parent process ID: %d.\n");
printf("Differences: %d, %d.\n", ***,***);
printf("Send a SIGCONT to process %d.\n\n", ***);
printf("Exited Process ID: %d.\n", **); 
printf("Receive a SIGCONT.\n"); 
printf("Receive a SIGSTOP.\n"); 


printf("Exited Process ID: %d; Count: %d.\n", **, ***); 
printf("The 3rd argument is larger than the 1st argument.\n");
printf("The 3rd argument is smaller than the 1st argument.\n");
printf("The 3rd argument is equal to the 1st argument.\n");   
*/ 



