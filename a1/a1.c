#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

void listRec(const char *path, int has_perm_write_op , int sizeG)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL) {
        perror("Could not open directory");
        return;
    }
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) {
                //printf("%s\n", fullPath);
                if(has_perm_write_op==1){
                    if((statbuf.st_mode & S_IWUSR) != 0) //verifica daca bitul pt owner e setat sa scrie 
                    {
                    if(S_ISREG(statbuf.st_mode)){
                    if(statbuf.st_size>sizeG)
                    {
                      printf("%s\n", fullPath);  
                    }
                }
                else if(sizeG==0)printf("%s\n", fullPath);
                    }
                }
                else{
                if(S_ISREG(statbuf.st_mode)){
                    if(statbuf.st_size>sizeG)
                    {
                      printf("%s\n", fullPath);  
                    }
                }
                else if(sizeG==0)printf("%s\n", fullPath);
                }
                
                if(S_ISDIR(statbuf.st_mode)) {
                    listRec(fullPath,has_perm_write_op,sizeG);
                }
            }
        }
    }
    closedir(dir);
}

void listNormal(const char *path , int has_perm_write_op , int sizeG)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL) {
        perror("Could not open directory");
        return;
    }
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) {
                if(has_perm_write_op==1){
                    if((statbuf.st_mode & S_IWUSR) != 0) //verifica daca bitul pt owner e setat sa scrie 
                    {
                    if(S_ISREG(statbuf.st_mode)){
                    if(statbuf.st_size>sizeG)
                    {
                      printf("%s\n", fullPath);  
                    }
                }
                else if(sizeG==0)printf("%s\n", fullPath);
                    }
                }
                else{
                if(S_ISREG(statbuf.st_mode)){
                    if(statbuf.st_size>sizeG)
                    {
                      printf("%s\n", fullPath);  
                    }
                }
                else if(sizeG==0)printf("%s\n", fullPath);
                }
                
            }
        }
    }
    closedir(dir);
}

///////////////////// 2.4 //////////////////



typedef struct o_sectiune {
    char nume[18]; // trebuie sa fie de 17 caractere deci alocam 18 charuri
    //nu ni se cere offsetul deci nu il salvez
    int type;
    int size;
}o_sectiune;

typedef struct header {
    char magic[5];
    unsigned short version; //teoretic e doar un caracter dar nu il declar asa pt a imi fi mai usor la testat
    unsigned short no_of_sections;
    o_sectiune sectiuni[10]; //declaram maximul care poate fi 
}header;

void printare(header HEADER){
    printf("SUCCESS\n");
    printf("version=%d\n",HEADER.version);
     printf("nr_sections=%d\n",HEADER.no_of_sections);
     for(int i=0;i<HEADER.no_of_sections;i++){
        printf("section%d: %s %d %d\n",i+1 ,HEADER.sectiuni[i].nume , HEADER.sectiuni[i].type,HEADER.sectiuni[i].size);
     }
}

<<<<<<< HEAD
=======
//ce face optiunea de la parse? 
// daca optiune==0 chiar face ceea ce ni se cere pentru parsare , inclusiv printarea de  mai sus
// daca optiune==1 , inseamna ca functia va fi folosita , pt findAll , ceea ce inseamna ca  nu mai printeaza nimic si doar probeaza daca fiserul este sau nu binar

>>>>>>> e368b817e280518c9e8cd3841f33e5912bff79c7
int parse_function(const char *path,  header HEADER, int optiune){
    
    int fd = open(path, O_RDONLY); // nu scriem in el deci e mai usor asa 
    
    
     lseek(fd,-4,SEEK_END);  //incepem sa citim pt magic , luam 5 caractere pt \0
     if(read(fd, HEADER.magic, 4)==-1)  //punem magic
     return 0;
     else
     {HEADER.magic[4]='\0'; //pt ca un string sa fie "inchis" punem \0 la el . Ni s-a explicat la inceputul cursului 6
     if(strcmp(HEADER.magic,"Uv9J")!=0)
     {
        if(optiune==0)printf("ERROR\nwrong magic\n");
     return 0;
     }
     }

    //lseek(fd,0,SEEK_SET); //nu sunt sigura daca trebuie dar o sa tot mut cursorul la inceputul functiei
     lseek(fd,-6,SEEK_END); //apoi urcam 2 caractere si citim header_size , pe care nu il salvam deoarece nu ni se cere sa il printam
     unsigned short header_size=0; //char head_size[3]; 
     if(read(fd, &header_size, 2)==-1)
     return 3;
    //printf("%d",header_size);
     //header_size= 256*head_size[0]+head_size[1]; //am cautat pe net o functie care sa transforme un string intr-un unsigned short , deoarece atoi e doar pt int
     
     lseek(fd, -header_size, SEEK_END);
     HEADER.version=0;
     read(fd, &HEADER.version,1);
     //printf("%d",HEADER.version);
     if(HEADER.version<(unsigned short)93||HEADER.version>(unsigned short)213){
        if(optiune==0)printf("ERROR\nwrong version\n");
        
     return 0;
     }
     

     HEADER.no_of_sections=0;
     read(fd, &HEADER.no_of_sections,1);
     
     if(HEADER.no_of_sections<7||HEADER.no_of_sections>10){
        if(optiune==0)printf("ERROR\nwrong sect_nr\n");
     return 0;
     }
    //unsigned short poz_cursor=header_size-1;
    for(int i=0;i<HEADER.no_of_sections;i++)
    {
        
     read(fd, HEADER.sectiuni[i].nume,17);
     HEADER.sectiuni[i].nume[17]='\0';
     read(fd,&HEADER.sectiuni[i].type,4);
     if(HEADER.sectiuni[i].type!=22&&HEADER.sectiuni[i].type!=27&&HEADER.sectiuni[i].type!=63&&HEADER.sectiuni[i].type!=75)
     {
        if(optiune==0)printf("ERROR\nwrong sect_types\n");
     return 0;
     }
     int offset=0;
     read(fd,&offset,4);
     read(fd,&HEADER.sectiuni[i].size,4);
     if(optiune==1){
        if(HEADER.sectiuni[i].size>1209)
        return 0;
     }
     
     

    }
    if(optiune==0) printare(HEADER);

     

   
    

    close(fd);
    return 1;
}

/////////////////////////2.5////////////////////////////////////

//pt a cauta linia in folder , o sa folosesc functia implementata la labul4 ex1

int get_line(int fd, int lineNr, char *line, int maxLength, int offset){

int nr_linie=0,j=0,ok=0;
char c = 0;

//size = lseek(fd, 0, SEEK_END);//ca sa afalm size ul fisierului
lseek(fd, offset, SEEK_SET);
for(int i=0;i<maxLength ;i++){
    //printf("%d",j);
    if(read(fd, &c, 1) != 1) {
     perror("Reading error");
     
   
    }
    else{
        if(c == '\r') {
            char c1=0;
<<<<<<< HEAD
=======
            i++;
>>>>>>> e368b817e280518c9e8cd3841f33e5912bff79c7
            read(fd,&c1,1);
            if(c1=='\n')
            {
            nr_linie++;
            
            if (nr_linie == lineNr) {
                //crtPos_act = lseek(fd, 0, SEEK_CUR);
                line[j] = '\0';
                ok=1;
                return 0;
            }
                j=0;
            }
        }
            
          else{
            
                if(j>=maxLength){line[j]='\0';
                return -1;
                }
                
                else line[j++]=c;
            
            }
            
        }
        
    

}
if (nr_linie<lineNr) return -2;
line[j] = '\0';
if(ok==1) return 0;
else return -4;
}



int extract_function(const char *path, int section , int line){
     int fd = open(path, O_RDONLY); // nu scriem in el deci e mai usor asa 
    
    header HEADER;
     lseek(fd,-4,SEEK_END);  //incepem sa citim pt magic , luam 5 caractere pt \0
     if(read(fd, HEADER.magic, 4)==-1)  //punem magic
     return 0;
     else
     {HEADER.magic[4]='\0'; //pt ca un string sa fie "inchis" punem \0 la el . Ni s-a explicat la inceputul cursului 6
     if(strcmp(HEADER.magic,"Uv9J")!=0)
     {printf("ERROR\nwrong magic\n");
     return 0;
     }
     }

    //lseek(fd,0,SEEK_SET); //nu sunt sigura daca trebuie dar o sa tot mut cursorul la inceputul functiei
     lseek(fd,-6,SEEK_END); //apoi urcam 2 caractere si citim header_size , pe care nu il salvam deoarece nu ni se cere sa il printam
     unsigned short header_size=0; //char head_size[3]; 
     if(read(fd, &header_size, 2)==-1)
     return 3;
    //printf("%d",header_size);
     //header_size= 256*head_size[0]+head_size[1]; //am cautat pe net o functie care sa transforme un string intr-un unsigned short , deoarece atoi e doar pt int
     
     lseek(fd, -header_size, SEEK_END);
     HEADER.version=0;
     read(fd, &HEADER.version,1);
     //printf("%d",HEADER.version);
     if(HEADER.version<(unsigned short)93||HEADER.version>(unsigned short)213){
        printf("ERROR\nwrong version\n");
        
     return 0;
     }
     

     HEADER.no_of_sections=0;
     read(fd, &HEADER.no_of_sections,1);
     
     if(HEADER.no_of_sections<7||HEADER.no_of_sections>10){
        printf("ERROR\nwrong sect_nr\n");
     return 0;
     }

     if(section>HEADER.no_of_sections)
     {
        printf("ERROR\ninvalid section\n");
        return 0;
     }
    //unsigned short poz_cursor=header_size-1;
    for(int i=0;i<HEADER.no_of_sections;i++)
    {
        
     read(fd, HEADER.sectiuni[i].nume,17);
     HEADER.sectiuni[i].nume[17]='\0';
     read(fd,&HEADER.sectiuni[i].type,4);
     if(HEADER.sectiuni[i].type!=22&&HEADER.sectiuni[i].type!=27&&HEADER.sectiuni[i].type!=63&&HEADER.sectiuni[i].type!=75)
     {
        printf("ERROR\nwrong sect_types\n");
     return 0;
     }
     int offset=0;
     //int no_character=0;
     read(fd,&offset,4);
     read(fd,&HEADER.sectiuni[i].size,4);
     if(i+1==section){
        //printf("%d %d %d",offset, i, HEADER.sectiuni[i].size);
        char line_string[HEADER.sectiuni[i].size];
        lseek(fd,0,SEEK_SET);
<<<<<<< HEAD
=======
        
>>>>>>> e368b817e280518c9e8cd3841f33e5912bff79c7
        int zero = get_line(fd,line,line_string,HEADER.sectiuni[i].size,offset);
        if (zero < 0) {
        if(zero==-1)
        printf("ERROR\ninvalid line");
        if(zero==-2)
        printf("ERROR\ninvalid line");
<<<<<<< HEAD
=======
        break;
>>>>>>> e368b817e280518c9e8cd3841f33e5912bff79c7
        //printf("S-a produs o eroare in timpul executieie problemei \n");
        
    }
    else {
        //char lin[no_character+1];
        //lin[no_character]='\0';
        //strncpy(lin,line_string,512);
        printf("SUCCESS\n%s\n",line_string);
    break;
     }
     
     
     

    }
    }
    
    close(fd);
    return 1;
}

/////////////////////////2.6/////////////////////////////////////


void listFindAll(const char *path, int has_perm_write_op , int sizeG)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL) {
        perror("Could not open directory");
        return;
    }
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) {
                //printf("%s\n", fullPath);
                if(has_perm_write_op==1){
                    if((statbuf.st_mode & S_IWUSR) != 0) //verifica daca bitul pt owner e setat sa scrie 
                    {
                    if(S_ISREG(statbuf.st_mode)){
                    if(statbuf.st_size>sizeG)
                    {
                      printf("%s\n", fullPath);  
                    }
                }
                else if(sizeG==0)printf("%s\n", fullPath);
                    }
                }
                else{
                if(S_ISREG(statbuf.st_mode)){
                    if(statbuf.st_size>sizeG)
                    {
                        header HEADER;
                        //printf(" %s\n",fullPath);
                        if(parse_function(fullPath,HEADER,1)==1)
                      printf("%s\n", fullPath);  
                    }
                
                else if(sizeG==0){
                    header HEADER;
                    //printf("%s",path);
                    if(parse_function(path,HEADER,1))
                    printf("%s\n", fullPath);
                }
                }
                }
                
                if(S_ISDIR(statbuf.st_mode)) {
                    listFindAll(fullPath,has_perm_write_op,sizeG);
                }
            }
        }
    }
    closedir(dir);
}

int main(int argc, char **argv){
    if(argc >= 2){
        int optiune=0;
        if(strcmp(argv[1], "variant") == 0){
            printf("28807\n");
        }
        if(strcmp(argv[1], "list")==0){ //se rezovva cerinta 2.3
           char path[512]; char sz[20];
           int sizeG=0; //pt size greater
           int has_perm_write_op=0; 
            int recursiv=0;
            
            for(int i=2 ; i<argc; i++)
            {
            {
                if(strcmp(argv[i],"recursive")==0)
                recursiv=1;
                if(strstr(argv[i],"path=")!=NULL){
                    strcpy(path,argv[i]+5);
                    //printf("%s",path); proba
                }
                if(strstr(argv[i],"size_greater=")!=NULL){
                   strcpy(sz,argv[i]+13);
                   sizeG=atoi(sz); 
                }
                if(strstr(argv[i],"has_perm_write")!=NULL){
                    has_perm_write_op=1;
                }
                
            }
            }
            if(recursiv==1)
            {
                DIR *dir = NULL;
                 dir = opendir(path);
                    if(dir==NULL){
                    perror("ERROR\ninvalid directory path\n");
                    return -1;
                    }
                    else{
                        printf("SUCCESS\n");
                    }
                    closedir(dir);
                    listRec(path,has_perm_write_op,sizeG);
        
            }  
            else{
                DIR *dir = NULL;
                 dir = opendir(path);
                    if(dir==NULL){
                    perror("ERROR\ninvalid directory path\n");
                    return -1;
                    }
                    else{
                        printf("SUCCESS\n");
                    }
                    closedir(dir);
                    listNormal(path,has_perm_write_op,sizeG);

            } 
        }
            if(strcmp(argv[1],"parse")==0){
                char path[512];
                strcpy(path,argv[2]+5);
                 int fd = open(path,O_RDONLY);
                if (-1==fd) { //yoda style :))
                perror("can not open file");
                return -1;
                }
                close(fd); // de mentionat ca structura mea este : body si apoi header deci o voi lua de la final cu cititul
                header HEADER;
                
                //printf("%d",parse_function(path,HEADER));
               parse_function(path,HEADER,optiune);
                
                
            }  
            if(strcmp(argv[1],"extract")==0){
                char path[512] , section[100], line[100];
                int sectiune=0, linie=0;
                for(int i=2;i<argc;i++){
                    if(strstr(argv[i],"path=")!=NULL){
                    strcpy(path,argv[i]+5);
                    }
                    if(strstr(argv[i],"section=")!=NULL){
                        strcpy(section,argv[i]+8);
                        sectiune=atoi(section);
                    }
                    if(strstr(argv[i],"line=")!=NULL){
                        strcpy(line,argv[i]+5);
                        linie=atoi(line);
                        //printf("%d %d",sectiune,linie);
                    }
                }
                int fd = open(path,O_RDONLY);
                if (-1==fd) { //yoda style :))
                perror("ERROR\ninvalid file\n");
                return -1;
                }
                close(fd);
                extract_function(path,sectiune,linie);

            }
            if(strcmp(argv[1],"findall")==0){
                char path[512] ;
                
                for(int i=2;i<argc;i++){
                    if(strstr(argv[i],"path=")!=NULL){
                    strcpy(path,argv[i]+5);
                    }
                }
                DIR *dir = NULL;
                 dir = opendir(path);
                    if(dir==NULL){
                    perror("ERROR\ninvalid directory path\n");
                    return -1;
                    }
                    else{
                        printf("SUCCESS\n");
                    }
                    closedir(dir);
                listFindAll(path,0,0);
            }
              
            
            
        

    }
    return 0;
}
