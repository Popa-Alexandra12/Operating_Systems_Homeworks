#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include "a2_helper.h"

#define NUM_THREADS_OF_P3 4
#define NUM_THREADS_OF_P2 48

pthread_t P3_th[4];



typedef struct thread{
    pthread_t th[48];
    pthread_t th11;
    int id;
}TH;



sem_t sem1;
sem_t sem2;
sem_t sem3;
sem_t sem4;
sem_t sem5;
sem_t sem6;

void *P3_function_th(void *arg)
{
   
    if(pthread_self()==P3_th[1])
    {
        info(BEGIN,3,1);
        sem_post(&sem2);
        sem_wait(&sem1);
        info(END, 3,1);
       
    } else if(pthread_self()==P3_th[4]) 
        {
            sem_wait(&sem2);
            info(BEGIN,3,4);
            info(END,3,4);
            sem_post(&sem1);
            
       }    
        else{if(pthread_self()==P3_th[2])
        {
             info(BEGIN,3,2);
             info(END,3,2);
   
        }
         else
             if(pthread_self()==P3_th[3])
             {
                info(BEGIN,3,3);
                info(END,3,3);
             }

        }          
   return NULL;
    
}
void P3_create_th()
{   
    int create;
    
    if(sem_init(&sem1,0,0)!=0)
    {
       
        perror("Could not init the semaphore");
       exit(1);
    }
    if(sem_init(&sem2,0,0)!=0)
    {
       
        perror("Could not init the semaphore");
       exit(1);
    }
    
    for(int i=1;i<=NUM_THREADS_OF_P3;i++)
    {   create=0;
        
            create=pthread_create(&P3_th[i],NULL,P3_function_th,NULL);
            if(create!=0)
            {
              perror("Could not create thread");
              exit(1);         
            }

    }
    for(int i=1;i<=NUM_THREADS_OF_P3;i++)
    {
        pthread_join(P3_th[i],NULL);
    }
    sem_destroy(&sem1);
    sem_destroy(&sem2);
}
void *P2_function_th11()
{   
   
    sem_wait(&sem3);       //sem cu 4 permisiuni -> nu lasa sa ruleze mai mult de 4 th deodata
    info(BEGIN,2,11);
    
       
        for(int i=1;i<NUM_THREADS_OF_P2;i++)//ma asigur ca T11 incepe primul
        {
            sem_post(&sem4);
            //printf("SEM4 %d\n",i);
        } 
        
        for(int i=1;i<4;i++)
        {
            sem_wait(&sem6);
            //printf("SEM6 %d\n",i);
        }
           
   
    info(END,2,11);
   
        for(int i=1;i<48;i++)//ma asigur ca T11 se termina primul
        {
            sem_post(&sem5);
             //printf("SEM5 %d\n",i);
        }
       
    
    sem_post(&sem3);
    return NULL;
}
void *P2_function_rest_of_th(void *args)
{   
    int id=*(int*)args;
    sem_wait(&sem4);   //sem cu 0 permisiuni-> TH11 se incheie doar cand ruleza 4 th
        
   
    sem_wait(&sem3);       //sem cu 4 permisiuni -> nu lasa sa ruleze mai mult de 4 th deodata
    info(BEGIN,2,id);
     
        sem_post(&sem6);   
        sem_wait(&sem5);
    
    info(END,2,id);
    
    sem_post(&sem3);
    return NULL;
}

void P2_create_th()
{   
    pthread_t P2_th[48];
    
    
    int create;
    if(sem_init(&sem3,0,4)!=0)
    {
       
        perror("Could not init the semaphore");
        exit(1);
    }
     if(sem_init(&sem4,0,0)!=0)
    {
       
        perror("Could not init the semaphore");
        exit(1);
    }
     if(sem_init(&sem5,0,0)!=0)
    {
       
        perror("Could not init the semaphore");
        exit(1);
    }
     if(sem_init(&sem6,0,0)!=0)
    {
       
        perror("Could not init the semaphore");
        exit(1);
    }
    
    int param[49];
    
    
   
    for(int i=1;i<=NUM_THREADS_OF_P2;i++)  
    {   
        create=0;
       param[i]=i;
       
        if(i==11)
        {create=pthread_create(&P2_th[11],NULL,P2_function_th11,NULL);
        if(create!=0)
        {
            perror("Could not create thread");
            exit(1);         
        }
      
        }
        else{
            create=pthread_create(&P2_th[i],NULL,P2_function_rest_of_th,&param[i]);
        if(create!=0)
        {
            perror("Could not create thread");
            exit(1);         
        }
        }
    } 
   
    for(int i=1;i<=NUM_THREADS_OF_P2;i++)
    {
      
        pthread_join(P2_th[i],NULL);
    }
    
    sem_destroy(&sem3);
    sem_destroy(&sem4);
    sem_destroy(&sem5);
    sem_destroy(&sem6);
    
         
}
int main(){
    init();

   
    pid_t pid2,pid3,pid4,pid5,pid6,pid7;
    int ok_p2=0,ok_p3=0,ok_p4=0,ok_p5=0,ok_p6=0,ok_p7=0;
    info(BEGIN,1,0);
    //creare proces P2
    pid2=fork();
    if(pid2==-1)
    {
        perror("Could not create child process P2");
       exit(1);
    }else if(pid2 == 0) {
     
        info(BEGIN, 2, 0);
          
         
            pid4=fork();
            if(pid4 == -1) {
                perror("Could not create child process P4");
               exit(1);
            }else if(pid4==0){
              
                info(BEGIN, 4, 0);
                info(END, 4, 0);
                 exit(0);
            }
            else {
                pid5=fork();
                if(pid5 == -1) {
                    perror("Could not create child process P5");
                    exit(1);
                }else if(pid5==0){
                   
                    info(BEGIN, 5, 0);
                    info(END, 5, 0);
                     exit(0);
                    }
                    else{
                            pid6=fork();
                            if(pid6 == -1) {
                                perror("Could not create child process P6");
                                exit(1);
                            }else if(pid6==0){
                                info(BEGIN, 6, 0);
                                info(END, 6, 0);
                                exit(0);
                                }else{
                                    wait(NULL);
                                    ok_p6=1;
                                } 
                            wait(NULL);
                            ok_p5=1;
                        }
                     wait(NULL);
                     ok_p4=1;
                }
           
            
            if(ok_p4==1 && ok_p5==1 && ok_p6==1)
            {
                P2_create_th();
               
                info(END, 2, 0);
            }
            
            
           
     
    } else{
        pid3=fork();
        if(pid3 == -1) {
                perror("Could not create child process P3");
                exit(1);
            }else if(pid3==0){
                info(BEGIN, 3, 0);

                

                pid7=fork();
                if(pid7 == -1) {
                    perror("Could not create child process P7");
                   exit(1);
                }else if(pid7==0){
                    info(BEGIN, 7, 0);
                    info(END, 7, 0);
                     exit(0);
                }
                else{
                    wait(NULL);
                    ok_p7=1;
                }
                
               if(ok_p7==1)
                {
                P3_create_th();
                info(END, 3, 0);
                
                }
                
            }
            else{
        wait(NULL);
        ok_p3=1;
    }
    wait(NULL);
    ok_p2=1;
    }
    
   
    if(ok_p2==1 && ok_p3==1){
    info(END, 1, 0);
    }
    
    return 0;
}
