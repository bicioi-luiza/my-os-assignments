#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

#define FIFO_RESP "RESP_PIPE_28807"
#define FIFO_REQ "REQ_PIPE_28807"
//declarata globala , pt a nu fii nevoie sa fie transmisa ca parametru pentru mai multe functii si dintr-o fctie in alta 
int shmFd=-1;
 volatile char *sharedData = NULL;

//int fd_w=-1;
void ping(int fd_w){
    printf("\na ajuns aici\n");
    char ping[]="PING$";
    char pong[]="PONG$";
    unsigned int varianta= 28807;
    write(fd_w, &ping, (strlen(ping)));
    write(fd_w, &varianta, sizeof(unsigned int));
    write(fd_w, &pong, (strlen(pong)));
}

void shm_create(int fd_w,unsigned int dim){
    int ok=1;
    //luaate din labul 12
    
    

    shmFd = shm_open("/D2W28j", O_CREAT | O_RDWR, 0664);
    if(shmFd < 0) {
        ok=0;
        perror("Could not aquire shm");
        //return 1;
    }
    ftruncate(shmFd, dim);
    sharedData = (volatile char*) mmap(NULL, dim, PROT_WRITE | PROT_READ, MAP_SHARED, shmFd, 0);
    if(sharedData == (void*)-1) {
        perror("Could not map the shared memory");
        ok=0;
        //return 1;
    }
    char create_sh[]="CREATE_SHM$";
    char suc[]="SUCCESS$";
    char er[]="ERROR$";
    write(fd_w,&create_sh,strlen(create_sh));
    if(ok==1)   write(fd_w,&suc,strlen(suc));
    else write(fd_w,&er,strlen(er));
}

void map_file(int fd_w,char *path){
//pr 1 lab12
    int fd;
     printf("\n%s\n",path);
    int ok=1;
    off_t size;
    char *data = NULL;
    fd = open(path, O_RDONLY);
    if(fd == -1) {
        ok=0;
        perror("Could not open input file");
        //return 1;
    }
    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    data = (char*)mmap(NULL, size,  PROT_READ , MAP_SHARED, fd, 0);
    if(data == (void*)-1) {
        ok=0;
        perror("Could not map file");
        close(fd);
        //return 1;
    }
    char map[]="MAP_FILE$";
    char suc[]="SUCCESS$";
    char er[]="ERROR$";
    write(fd_w,&map,strlen(map));
    if(ok==1)   write(fd_w,&suc,strlen(suc));
    else write(fd_w,&er,strlen(er));

}

void write_sh(int fd_w,unsigned int offset,unsigned int value){
    printf("\ns-a intrat in fctie\n");
    int ok=1;
     if(shmFd < 0) {
        ok=0;
        perror("Could not aquire shm");
        //return 1;
    }
     if(sharedData == (void*)-1) {
        perror("Could not map the shared memory");
        ok=0;
        //return 1;
    }
    if(offset>3218090||offset<0||(offset+4)>3218090)
    {
        ok=0;
        perror("Invalid offset");
    }
    char wr_val[]="WRITE_TO_SHM$";
    char suc[]="SUCCESS$";
    char er[]="ERROR$";
    write(fd_w,&wr_val,strlen(wr_val));
    if(ok==1)  { 
     
     char *string_value=(char*)&value;
     //sharedData=sharedData+offset;
     strncpy((char*)(sharedData+offset),string_value,4);
     write(fd_w,&suc,strlen(suc));

    }
    else write(fd_w,&er,strlen(er));
    
}

int main(void)
{
    int fd_r=-1,fd_w=-1;
    int ok = 1;
     unlink(FIFO_RESP);
    //create fifo
    if(mkfifo(FIFO_RESP, 0600) != 0) {
        printf("ERROR\ncannot create the response pipe\n");
        ok=0; //PAS1
        return 1;
    }

    //open, write and close fifo
    if(ok==1)fd_r = open(FIFO_REQ, O_RDONLY); //PAS 2 
    if(fd_r== -1) {
        printf("ERROR\ncannot open the request pipe\n");
        ok=0;
        return 1;
    }
    fd_w = open(FIFO_RESP, O_WRONLY); //PAS 3 
    if(fd_w== -1) {
        printf("ERROR\ncannot open the response pipe\n"); //PAS3
        ok=0;
        return 1;
    }
   char hi[]="HELLO$";
   ssize_t citit=-1;
   citit=write(fd_w, &hi, 6);//PAS4
   if(citit<6) ok=0;
   while(1){
   if(ok==1){
    printf("SUCCESS\n");
    
    int i = 0;
    char *request = (char *)calloc(250, sizeof(char)); //maxim 250 stringul deci 
    char c;
    do
    {
        read(fd_r, &c, sizeof(char)); //citim caracter cu caracter pana la finalul stringului
        request[i] = c;
        i++;
    } while (c!= '$');// se citeste stringul pana cand se ajunge la finalul lui ,adica caracterul $
    request[i - 1] = 0;
    
    
    printf("\n%s\n",request);
    if(strcmp(request,"PING")==0){
        ping(fd_w);
    }
    if(strcmp(request,"CREATE_SHM")==0){
        unsigned dim=0;
        read(fd_r,&dim,sizeof(unsigned int));
       shm_create(fd_w,dim);
    }
    if(strcmp(request,"MAP_FILE")==0)
    {
        int j=0;
        char *file_name = (char *)calloc(512, sizeof(char)); //maxim 512 path ul
        char c;
    do
    {
        read(fd_r, &c, sizeof(char)); //citim caracter cu caracter pana la finalul stringului
        file_name[j] = c;
        j++;
    } while (c!= '$');// se citeste stringul pana cand se ajunge la finalul lui ,adica caracterul $
    file_name[j - 1] = 0;
    printf("\n%s\n",file_name);
    map_file(fd_w,file_name);
    free(file_name);
    }
    if(strcmp(request,"WRITE_TO_SHM")==0){
        unsigned int offset,value;
        printf("\nhere\n");
        read(fd_r,&offset,sizeof(unsigned int));
        read(fd_r,&value,sizeof(unsigned int));
        write_sh(fd_w,offset,value);
    }
    if(strcmp(request,"READ_FROM_FILE_OFFSET")==0){
        close(fd_r);
        close(fd_w);
        unlink(FIFO_RESP);
        break;
    }
    if(strcmp(request,"READ_FROM_FILE_SECTION")==0){
        close(fd_r);
        close(fd_w);
        unlink(FIFO_RESP);
        break;
    }
    if(strcmp(request,"READ_FROM_LOGICAL_SPACE_OFFSET")==0){
        close(fd_r);
        close(fd_w);
        unlink(FIFO_RESP);
        break;
    }
    if(strcmp(request,"EXIT")==0){
        close(fd_r);
        close(fd_w);
        unlink(FIFO_RESP);
        break;
        
    }


    free(request);
   }
}
    
    
    //delete fifo
    

    return 0;
}