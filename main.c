#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>

int main(){

        //Define variables
        int i, j,s;
	const int N = 80;
	const int M = 8+5;
	double *shared_array_1 = mmap(NULL, N * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        double *shared_array_2 = mmap(NULL, N * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        double *shared_result = mmap(NULL, N * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        double *result = malloc(sizeof(double) * N);
         pid_t pid[M], wpid;
        int state =0;
        clock_t start_time, end_time,start_time_shared, end_time_shared;
        double time_taken,time_taken_shared;
       
        
        //Initialize arrays
        for ( i = 0; i < N; i++) {
		shared_array_1[i] = (double)rand()/(double)(RAND_MAX/N);
		shared_array_2[i] = (double)rand()/(double)(RAND_MAX/N);
    	}
    	
    	
       // compute sum in the parent process
       start_time = clock();
       for ( i = 0; i < N; i++) {
		result[i] = shared_array_1[i] + shared_array_2[i];
	    }
       end_time = clock();
       time_taken = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
       
       
       //Create M number of child processes 
       start_time_shared = clock();
       for (i =0 ; i < M; i++){
                
                pid[i] = fork();
                
       		if (pid[i] == 0) {
		    int start = i * (N / M);
		    int end = (i + 1) * (N / M);
                    
            // Compute the sum for the current process
            for (j = start; j <= end; j++) {
                shared_result[j] = shared_array_1[j] + shared_array_2[j];
                 if(j != N)
                   s = j;
            }

            
            
             
            exit(0);
           
        }
       
        
        // remaining few items in the parent process
        for(s; s < N ; s++)
        shared_result[s] = shared_array_1[s] + shared_array_2[s];
       } 
       
       
      //Wait
       for(i=0;i<M;i++){
       		 wpid = waitpid(pid[i],&state,0);
       		if(wpid == -1){
       			perror("waitpid");
       			exit(1);
       		}
       }
       end_time_shared = clock();
       time_taken_shared = ((double) (end_time_shared - start_time_shared)) / CLOCKS_PER_SEC;
       
       
      // Check the result 
      for (i = 0; i < N; i++) {
        if (result[i] != shared_result[i]) {
            printf("Error: result[%d] = %lf, shared_result[%d] = %lf\n", i, result[i], i, shared_result[i]);
        }
        else{
            printf("Sucsess: result[%d] = %lf, shared_result[%d] = %lf\n", i, result[i], i, shared_result[i]);
        }
    }
    printf("Time taken: %f seconds\n", time_taken);
    printf("Time taken parallel: %f seconds\n", time_taken_shared);

	return 0;
}
