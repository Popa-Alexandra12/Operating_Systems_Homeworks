#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
int verifica_dimensiune_fisier(char* path,char* size_smaller)
{
    int s_s=atoi(size_smaller);
    int fd=-1;
    off_t size;
    fd=open(path,O_RDONLY);
    size=lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);
    if(fd==-1)
         {
        perror("Could not open this file");
        return 0;
         }
        else if(fd<0)  {perror("end of file");
                       return 0;}
            else {
                
                printf("%ld \n",size);
                if(size>s_s) 
                        return 0;
                 else return 1;
             }
    
    return 1;
    
}
void afisare_fisiere_filtru_size(char *path,int size_smaller,bool recursive){
        DIR *d;
        struct dirent *director;
        struct stat file_info;
        char filePath[512];
        d=opendir(path);
        if(d==NULL) 
        {
            perror("Cannot open the directory !");
            exit(1);
        }
        
        while((director=readdir(d))!=NULL)
        {
            
           if(strcmp(director->d_name, ".") != 0 && strcmp(director->d_name, "..") != 0){
            
            snprintf(filePath, 512, "%s/%s", path, director->d_name);
            if(lstat(filePath, &file_info) == 0) {
                if(S_ISREG(file_info.st_mode) ){
                 
                    if(file_info.st_size<size_smaller){
                      printf("%s\n", filePath);
                   }
                  }
                  if(S_ISDIR(file_info.st_mode) && recursive==true) {
                     
                   afisare_fisiere_filtru_size(filePath,size_smaller,recursive);
                }
            }
           }
        }
        closedir(d);
}
void  afisare_fisiere_filtru_permisiune(char *path,bool recursive)
{
        DIR *dir;
        struct dirent *entry;
        struct stat statbuf;
        char filePath[512];
        dir=opendir(path);
        if(dir==NULL) 
        {
            perror("Cannot open the directory !");
            exit(1);
        }
        
        while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
        snprintf(filePath, 512, "%s/%s", path, entry->d_name);
            if(lstat(filePath, &statbuf) == 0)
            {
                if(statbuf.st_mode & S_IXUSR){
                   printf("%s\n", filePath);
                   if(S_ISDIR(statbuf.st_mode) && recursive==true) {
                    afisare_fisiere_filtru_permisiune(filePath,recursive);
                }
                }
            }
        }
        }
        closedir(dir);

}
void afisaza_fisiere(char *path,bool recursive)
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
            if(lstat(fullPath, &statbuf)== 0) {
                printf("%s\n", fullPath);
               if(recursive==true){
                if(S_ISDIR(statbuf.st_mode) ) {
                    afisaza_fisiere(fullPath,recursive);
                }
               }
            }
        }
    }
    closedir(dir);
}
void optiunea_list(int argc,char **argv)
{
        char *path=NULL;
        char *size_smaller=NULL;
        bool has_filter=false;
        bool has_perm_execute=false;
        bool recursive=false;
         for(int i=2;i<argc+1;i++)
        {
            if(argv[i]!=NULL){
            if(strncmp("path=",argv[i],5)==0){
                path=argv[i]+5;
            }
            else if(strncmp("size_smaller=",argv[i],13)==0)
                   {
                       size_smaller=argv[i]+13;
                       has_filter=true;
                   } 
                 else if(strncmp("has_perm_execute",argv[i],16)==0){
                     has_perm_execute=true;
                     has_filter=true;
                }
                    else{
                        if(strncmp("recursive",argv[i],9)==0)
                        {
                            recursive=true;
                        }
                    }
            }
        } 
        if(path!=NULL){
        if(has_filter==true){
            if(size_smaller!=NULL){
              printf("SUCCESS\n");
            afisare_fisiere_filtru_size(path,atoi(size_smaller),recursive);
             }
            else {if(has_perm_execute==true){
                printf("SUCCESS\n");
                afisare_fisiere_filtru_permisiune(path,recursive);
            }}
        }
        else{
            printf("SUCCESS\n");
            afisaza_fisiere(path,recursive);
        }
        }else{
            perror("invalid directory path !");
            exit(1);
        }

}
void optiunea_parse(char* pathIn){
    char* path;
    int fd=-1;
    char magic[4];
    int header_size=0;
    int version=0;
    int no_of_sections=0;
    char sect_name[12];
    int sect_type=0;
    int sect_size=0;
    int sect_offset=0;
     if(strncmp("path=",pathIn,5)==0){
                path=pathIn+5;
            
    fd=open(path,O_RDONLY);
    if(fd==-1)
    {
        perror("Could not open this file");
        exit(1);
    }
    int r=read(fd,magic,4);
    if(r<0)
    {
         perror("Could not read from this file");
          exit(1);
    }
    if(strncmp("eF1S",magic,4)!=0)
    {
        printf("ERROR\n wrong magic");
         exit(1);
    }
    r=read(fd,&header_size,2);
    if(r<0)
    {
         perror("Could not read from this file");
    }
    r=read(fd,&version,4);
     if(r<0)
    {
         perror("Could not read from this file");
    }
    if(version<77 || version>158)
    {
         printf("ERROR\n wrong version");
          exit(1);
    }
    r=read(fd,&no_of_sections,1);
     if(r<0)
    {
         perror("Could not read from this file");
    }
    if(no_of_sections<2 || no_of_sections>11)
    {
        printf("ERROR\n wrong sect_nr");
         exit(1);
    }
    int poz_cur=11;
    for(int i=1;i<=no_of_sections;i++)
    {
        r=read(fd,sect_name,12);
        if(r<0)
        {
             perror("Could not read from this file");
        }
        r=read(fd,&sect_type,2);
        if(r<0)
        {
             perror("Could not read from this file");
        }
       
        if(sect_type!=40 && sect_type!=25 && sect_type!=96 && sect_type!=45 && sect_type!=47 && sect_type!=26 && sect_type!=72 ){
            printf("ERROR\n wrong sect_types");
             exit(1);
        }
        
            r=read(fd,&sect_offset,4);
            if(r<0)
             {
             perror("Could not read from this file");
             }
            r=read(fd,&sect_size,4);
            if(r<0)
            {
             perror("Could not read from this file");
            }
    
    }
    printf("SUCCESS\n");
    printf("version=%d\n",version);
    printf("nr_sections=%d\n",no_of_sections);
    lseek(fd,poz_cur,SEEK_SET);
    
    for(int j=1;j<=no_of_sections;j++){
        read(fd,&sect_name,12);
        read(fd,&sect_type,2);
        lseek(fd,4,SEEK_CUR);
        read(fd,&sect_size,4);
        printf("section%d: %s %d %d\n",j,sect_name,sect_type,sect_size);
       
    }
     }
}
void optiunea_extract(char* argv[])
{
    char*path;
    int section;
    int line;
    char magic[4];
    int header_size=0;
    int version=0;
    int no_of_sections=0;
    char sect_name[12];
    int sect_type=0;
    int sect_size=0;
    int sect_offset=0;
   
    if(argv[2]!=0){
        if(strncmp("path=",argv[2],5)==0)
        {
            path=argv[2]+5;
        }
        if(path==NULL){
        printf("ERROR\ninvalid file\n");
         exit(1);
    }
    }
     if(argv[3]!=0){
        if(strncmp("section=",argv[3],8)==0)
        {
            section=atoi(argv[3]+8);
        }
    }
     if(argv[4]!=0){
        if(strncmp("line=",argv[4],5)==0)
        {
            line=atoi(argv[4]+5);
        }
    }
    int fd=-1;
    fd=open(path,O_RDONLY);
    if(fd==-1)
    {
        perror("Could not open this file");
        exit(1);
    }
    int r=read(fd,magic,4);
    if(r<0)
    {
         perror("Could not read from this file");
          exit(1);
    }
    if(strncmp("eF1S",magic,4)!=0)
    {
         exit(1);
    }
    r=read(fd,&header_size,2);
    r=read(fd,&version,4);
    if(version<77 || version>158)
    {
          exit(1);
    }
    read(fd,&no_of_sections,1);
    if(no_of_sections<2 || no_of_sections>11)
    {
         exit(1);
    }
    if(no_of_sections<section)
    {
        printf("ERROR\ninvalid section");
         exit(1);
    }
    for(int i=1;i<=no_of_sections;i++)
    {   
        read(fd,sect_name,12);
        read(fd,&sect_type,2);
        if(sect_type!=40 && sect_type!=25 && sect_type!=96 && sect_type!=45 && sect_type!=47 && sect_type!=26 && sect_type!=72 ){
           
             exit(1);
        }
        
            read(fd,&sect_offset,4); 
            read(fd,&sect_size,4);
           
             if(i==section){
                lseek(fd,sect_offset,SEEK_SET);
                 int nrCrt=0;
                 char c='x';
                 int ok=0;
                 int nrLinie=0;
                 int nrCrtLinie=0;
                while(nrCrt<=sect_size)
                {
                  int r1=read(fd,&c,1);
                    if(r1<0){
                  perror("Could not read from this file ");
                     }
                  else{nrCrt++;
                    if(c=='\n'){
                    
                     nrLinie++;
                    if(nrLinie==line){
                    ok=1;
                    lseek(fd,-nrCrtLinie,SEEK_CUR);
                    char linie[nrCrtLinie-1];
                    int r2=read(fd,linie,nrCrtLinie);
                    if(r2<0){
                        perror("Could not read from this file");
                    }
                    else {
                        printf("SUCCESS\n");
                         for(int v=sizeof(linie)-1;v>=0;v--)
                        {
                            printf("%c",linie[v]);
                        }
                        printf("\n");
                            break;}        
                }
                 nrCrtLinie=0;
            }
                nrCrtLinie++;
        

        }
    }
       if(ok==0) {
                printf("ERROR\ninvalid line\n");
                exit(1);
           }
             }
         
    
    }
 
}
int findall_corespunzator(char *path)
{  
    int fd=-1;
    char magic[5];
    int header_size=0;
    int version=0;
    int no_of_sections=0;
    char sect_name[13];
    int sect_type=0;
    int sect_size=0;
    int sect_offset=0;
    fd=open(path,O_RDONLY);
    if(fd==-1)
    {
        perror("Could not open this file");
        close(fd);
        return 0;
    }
    int r=read(fd,magic,4);
    if(r<0)
    {
         perror("Could not read from this file");
        close(fd);
        return 0;
    }
    if(strncmp("eF1S",magic,4)!=0)
    {
        close(fd);
        return 0;
    }
    r=read(fd,&header_size,2);
   
    r=read(fd,&version,4);
  
    if(version<77 || version>158)
    {
        close(fd);
        return 0;
    }
    read(fd,&no_of_sections,1);
       
    if(no_of_sections<2 || no_of_sections>11)
    {
        close(fd);
        return 0;
    }
    int ok=0;
    for(int i=1;i<=no_of_sections;i++)
    { 
        lseek(fd,11 + (i-1)* 22,SEEK_SET);
        read(fd,sect_name,12);
        read(fd,&sect_type,2);
        if(sect_type!=40 && sect_type!=25 && sect_type!=96 && sect_type!=45 && sect_type!=47 && sect_type!=26 && sect_type!=72 ){
           close(fd);
             return 0;
        }
            read(fd,&sect_offset,4); 
            read(fd,&sect_size,4);
            lseek(fd,sect_offset,SEEK_SET);
            int nrCrt=0;
            char c='x';
            int nrLinie=0;
            while(nrCrt<sect_size)
            { 
                int r1=read(fd,&c,1);
                if(r1<0)
                {
                  perror("Could not read from this file ");
                }
                else
                {
                    nrCrt++;
                    if(c=='\n')
                    {
                        nrLinie++;
                    }
                }
            }
       if(nrLinie+1 >=15 ) {
               ok=1;
               break;
           }
         
    }
   if(ok==1) {
        close(fd);
        return 1;
    }
     
    close(fd);
    return 0;
}
void optiunea_findall(char *path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char fullPath[512];
    dir=opendir(path);
    if(dir==NULL) 
    {
        perror("Cannot open the directory !");
        exit(1);
    }
    printf("SUCCESS\n");
     while((entry = readdir(dir)) != NULL) {
          
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) { 
                if(S_ISREG(statbuf.st_mode)){
                    if(findall_corespunzator(fullPath)==1) {

                        printf("%s\n", fullPath);
                    }
                }
                if(S_ISDIR(statbuf.st_mode)) {
                    optiunea_findall(fullPath);
                }
            }
        }
    }
    closedir(dir);

}
int main(int argc, char **argv){

    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("65859\n");
        }
   else if(strcmp(argv[1],"list")==0)
    {
       optiunea_list(argc,argv);

    }
    else if(strcmp(argv[1],"parse")==0)
    {
        if(argv[2]!=NULL){
        optiunea_parse(argv[2]);
        }
        else {
            perror("invalid directory path !");
            exit(1);
        }
    }
    else if(strcmp(argv[1],"extract")==0)
    {
            optiunea_extract(argv);
    } 
    else if(strcmp(argv[1],"findall")==0)
        {
            char *path;
             if(argv[2]!=NULL)
             {
                 if(strncmp("path=",argv[2],5)==0){
                path=argv[2]+5;
             if(path==NULL)
             {

                 printf("invalid directory path\n");
                 exit(1);
              }
              else  optiunea_findall(path);
             }
             }
             else {
            perror("invalid directory path !");
            exit(1);
             }

        }
    }
    return 0;
}