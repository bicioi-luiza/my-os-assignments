#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>

#define NR_THREADS_P6 4 //la a doua cerinta ni se specifica ca N pt P6 este 4
#define NR_THREADS_P3 38 

typedef struct {
    int ID;
   
    sem_t *sem0;
} TH_STRUCT_P6;
//semafoare pt a doua cerinta
 sem_t sem2;
sem_t sem3;
//semafoare pt a patra cerinta
sem_t *sem68_4;
sem_t *sem86_24;
//semafor pt a treia cerinta , cu nr permisiuni 3 astfel incat sa putem inchide threadul 15 cand 4 semafoare , cu tot cu el ruleaza
sem_t sem3_15;
int inceput=0;
int count=0;
sem_t sem3_inceput;
sem_t sem_fin;
sem_t sem3_nou;
sem_t sem_pornit; //trebuie sa ne asiguram ca primeste macar 3 posturi

//pt prob 2
void *threadFn(void *unused)
{
     TH_STRUCT_P6 *s = (TH_STRUCT_P6*)unused;
  
     
        if(s->ID==2){
            
            info(BEGIN, 6,2);
            //printf("incepe 2\n");
            sem_post(&sem2);
            sem_wait(&sem3);
            //sem_wait(&sem2);
            info(END, 6, 2);
            //sem_wait(&sem2);
            //printf("se termina 2\n");
            //sem_post(s->sem0);
        }
        if(s->ID==3) // inseamna ca suntem la threadul 3 care trebuie sa astepte sa inceappa T2
        {
        sem_wait(&sem2); //asteptam dupa sem2 ca sa stim ca a pornit t2
       
        info(BEGIN, 6, 3);
       //printf("incepe 3\n");
        
        info(END, 6, 3);
        //printf("se termina 3\n");
        sem_post(&sem3); //transimte ca poate sa se termine th2
        //sem_wait(&sem3);
        }
        if(s->ID==1){info(BEGIN, 6, s->ID);
            info(END, 6, s->ID);
            
        }
        if(s->ID==4){
            sem_wait(sem68_4);
            info(BEGIN, 6, s->ID);
            info(END, 6, s->ID);
            sem_post(sem86_24);
        }
     
    
    return NULL;
}
void *threadFn2(void *unused)
{
    TH_STRUCT_P6 *s = (TH_STRUCT_P6*)unused;
         
            sem_wait(s->sem0); //asigura ca nu exista mai mult de 4 threaduri pornite 
           
           
            if(s->ID!=15){
                 info(BEGIN, 3, s->ID);
                sem_post(&sem_pornit);
                 sem_wait(&sem3_inceput); //pt var globala inceput
                if(inceput==0) sem_wait(&sem_fin); //are 34 de permisiuni deci daca sa zicem ca th 15 nu a pornit inca , il obliga sa porneasca ca sa se poata continua
                sem_post(&sem3_inceput);
            }
            if(s->ID==15){
            info(BEGIN, 3, s->ID);
            
            sem_wait(&sem3_inceput);
            inceput=1;
            sem_post(&sem_fin); // cazurile pt th 15 pornit impreuna cu ultimele 3 threaduri
            sem_post(&sem_fin);
            sem_post(&sem_fin);

            sem_post(&sem3_inceput);
           
           
            
            
           

            

          
           //sem_post(&sem3_15);
            }
            if(s->ID!=15){
               
                sem_wait(&sem3_inceput);
                if(inceput==1) {
                      
                   sem_wait(&sem3_nou); //sem pt variabila count
                    count++; 
                    if(count<=3)  sem_wait(&sem3_15);//3 threaduri o sa fie oprite pana cand se termina 15
                    else {
                         info(END, 3, s->ID);
                        sem_post(s->sem0);
                    }
                    sem_post(&sem3_nou);
                   
                   
                }
                
                sem_post(&sem3_inceput);
               
                 
            }
            if(s->ID==15){

                sem_wait(&sem_pornit);
                sem_wait(&sem_pornit);
                sem_wait(&sem_pornit); // se asigura ca exista 3 semafoare pornite , inainte sa se inchida 
               sem_wait(&sem3_inceput);
                 info(END, 3, s->ID);
                 sem_post(s->sem0);
            inceput=2; //se seteaza inceput pe 2 ca sa se poata printa direct finalul celorlalte threaduri de dupa 15
            sem_post(&sem3_15); //deblocheaza celelalte semafoar ai sa se poata inchide 
           sem_post(&sem3_15);
           sem_post(&sem3_15);
           
            sem_post(&sem3_inceput);


           
            }
            if(s->ID!=15){
                 //printf("%d %d\n",count ,s->ID);
                 sem_wait(&sem3_inceput);
                 if(inceput==2||inceput==0) //daca s-a terminat th 15 sau daca nu a inceput dar inca mai sunt mai mult de 4 th ramase 
                info(END, 3, s->ID);
                sem_post(s->sem0);
                sem_post(&sem3_inceput);
                 
            }
           
               //sem_post(s->sem0);
            return NULL;
}
//pt ultima problema , doar face 
void *threadFn3(void *unused)
{
    TH_STRUCT_P6 *s = (TH_STRUCT_P6*)unused;
            if(s->ID!=4&&s->ID!=2)
            {info(BEGIN, 8, s->ID);
            info(END, 8, s->ID);
            }
            if(s->ID==4){
                info(BEGIN, 8, 4);
                info(END, 8, 4);
                sem_post(sem68_4);
            }
            if(s->ID==2){
                sem_wait(sem86_24);
                info(BEGIN, 8, 2);
                info(END, 8, 2);

            }
            return NULL;
}

int main(){
    init();
    //implementare ex1 
    pid_t P2=-1,P3=-1,P4=-1,P5=-1,P6=-1,P7=-1,P8=-1;
    int status1=0, status2=0, status3=0; // ca trebuie sa astepte maxim dupa 3 copii
    //pt sincronizare threaduri din acelasi proces
    
         sem68_4 = sem_open("semafor_th4_proc68", O_CREAT, 0644, 0);
        
        sem86_24 = sem_open("semafor_th24_proc82", O_CREAT, 0644, 0);
       

    info(BEGIN, 1, 0); //apelam fork daca intra in if suntem in procesul copil , altfel suntem in parinte
    P2 = fork();
    if(P2 == 0) {//deci P2 copilul lui P1
    info(BEGIN, 2, 0);
    P4 = fork();
        if(P4 == 0) {//deci P4 copilul lui P2
            info(BEGIN, 4, 0);
            P6 = fork();
                if(P6 == 0) {//deci P6 copilul lui P4
                
                sem_t sem0;
                TH_STRUCT_P6 params[NR_THREADS_P6];
                pthread_t tids[NR_THREADS_P6+1];

                if(sem_init(&sem2, 0, 0) != 0) {
                perror("Could not init the semaphore");
                return -1;
                }
                if(sem_init(&sem3, 0, 0) != 0) {
                perror("Could not init the semaphore");
                return -1;
                }
                if(sem_init(&sem0, 0, 4) != 0) {
                 perror("Could not init the semaphore");
                return -1;
                }
                //create the threads
                for(int i=0; i<NR_THREADS_P6; i++) {
                params[i].ID = i+1; // ca sa fie threadul 1,2...N
               
                params[i].sem0 = &sem0;
        
                }
                for(int i=0;i<NR_THREADS_P6;i++){
                     pthread_create(&tids[i], NULL, threadFn, &params[i]);
                    
                }
                for(int i=0; i<NR_THREADS_P6; i++) {
                pthread_join(tids[i], NULL);
                }
                sem_destroy(&sem2);
                sem_destroy(&sem3);
                sem_destroy(&sem0);
                //if(ok==4)
                info(BEGIN, 6, 0);
                info(END, 6, 0);
                exit(1);
                
                
                }
            waitpid(P6, &status1, 0);//asteapta dupa p6 , ca apoi sa se termine p4
            info(END, 4, 0);
            exit(1);
        }
    P5 = fork();
        if(P5 == 0) {//deci P5 copilul lui P2
            info(BEGIN, 5, 0);
            
            info(END, 5, 0);
             exit(1);
        }
    P8 = fork();
        if(P8== 0) {//deci P8 copilul lui P2
            sem_t sem0;
            TH_STRUCT_P6 params[NR_THREADS_P6];
            pthread_t tids[NR_THREADS_P6+1]; //fol aceeasi structura chiar daca initial am declarat-o cu gandul sa o fol doar pt prima prob
            if(sem_init(&sem0, 0, 4) != 0) {
                 perror("Could not init the semaphore");
                return -1;
                }
                //create the threads
                for(int i=0; i<NR_THREADS_P6; i++) {
                params[i].ID = i+1; // ca sa fie threadul 1,2...N
               
                params[i].sem0 = &sem0;
        
                }
                for(int i=0;i<NR_THREADS_P6;i++){
                     pthread_create(&tids[i], NULL, threadFn3, &params[i]);
                    
                }
                for(int i=0; i<NR_THREADS_P6; i++) {
                pthread_join(tids[i], NULL);
                }
                sem_destroy(&sem0);
            info(BEGIN, 8, 0);

            info(END, 8, 0);
             exit(1);
        }
        wait(&status1);
        wait(&status2);
        wait(&status3);// asteptam sa se termine toti cei 3 copii
    info(END, 2, 0);
    exit(1);
    }

     P3= fork();
    if(P3== 0) {//deci P3 copilul lui P1
        sem_t sem0;
        TH_STRUCT_P6 params[NR_THREADS_P3];
        pthread_t tids[NR_THREADS_P3+1]; //fol aceeasi structura chiar daca initial am declarat-o cu gandul sa o fol doar pt prima prob
                if(sem_init(&sem0, 0, 4) != 0) {
                 perror("Could not init the semaphore");
                return -1;
                }
                if(sem_init(&sem3_15, 0, 0) != 0) { //asteapta 3 sem_posturi ca sa inchida th 15
                 perror("Could not init the semaphore");
                return -1;
                }
                if(sem_init(&sem3_inceput, 0, 1) != 0) { //prima data nu asteapta nimic
                 perror("Could not init the semaphore");
                return -1;
                }
                if(sem_init(&sem_fin, 0, 34) != 0) { //prima data nu asteapta nimic
                 perror("Could not init the semaphore");
                return -1;
                }
                if(sem_init(&sem3_nou, 0, 1) != 0) { //prima data nu asteapta nimic
                 perror("Could not init the semaphore");
                return -1;
                }
                if(sem_init(&sem_pornit, 0, 0) != 0) { //asteapta 3 sem_posturi ca sa inchida th 15
                 perror("Could not init the semaphore");
                return -1;
                }
                //create the threads
                for(int i=0; i<NR_THREADS_P3; i++) {
                params[i].ID = i+1; // ca sa fie threadul 1,2...N
               
                params[i].sem0 = &sem0;
        
                }
                for(int i=0;i<NR_THREADS_P3;i++){
                     pthread_create(&tids[i], NULL, threadFn2, &params[i]);
                    
                }
               
                for(int i=0; i<NR_THREADS_P3; i++) {
                pthread_join(tids[i], NULL);
                }
                 sem_destroy(&sem0);
                  sem_destroy(&sem3_15);
                   sem_destroy(&sem3_inceput);
                   sem_destroy(&sem_fin);
                   sem_destroy(&sem3_nou);
    info(BEGIN, 3, 0);
    
    info(END, 3, 0);
     exit(1);
    }
    P7= fork();
    if(P7== 0) {//deci P7 copilul lui P1
    info(BEGIN, 7, 0);

    info(END, 7, 0);
    exit(1);
    }
    
    wait(&status1);
    wait(&status2);
    wait(&status3);
        sem_destroy(sem68_4);
        sem_destroy(sem86_24);
    info(END, 1, 0);
     exit(1);
      
    return 0;
}