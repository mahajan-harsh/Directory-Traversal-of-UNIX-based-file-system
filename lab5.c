/* USAGE:
    make

-If you want to remove the ./lab5 file, I have put in a clean command in
the makefile.
-You will have change the makefile a bit if you want to test code with your
directories.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

//The below function will report the attributes of the directory/file.
//It will return if the current fileName is file or a directory.
int reportAttributes(char* fileName){
  //We shall use stat here as the 'attributes' of it can give us all the
  //info that we require with great ease.
  struct stat fileStat;
  struct group* grp;
  struct passwd* pwd;
  char buff[80];
  int sizeBlocks;
  long int currTime;
  struct tm timeStamp;

  //Getting file information using stat..
  if (stat(fileName, &fileStat) < 0){
    perror("ERROR! Cannot get info.");
    return 0;
  }

  //Storing the time..
  currTime = fileStat.st_mtime;
  timeStamp = *localtime(&currTime);//changing epoch timeStamp to readable format.
  strftime(buff, sizeof(buff), "%a %Y-%m-%d %H:%M:%S %Z", &timeStamp);

  //Getting group and owner struct
  grp = getgrgid(fileStat.st_gid);
  pwd = getpwuid(fileStat.st_uid);

  //Error Checking...
  if (grp == NULL){
    perror("ERROR! Cannot get group name.");
    return 0;
  }
  else if (pwd == NULL){
    perror("ERROR! Cannot get user name.");
    return 0;
  }

  //Getting size in blocks...
  sizeBlocks = ceil((((double) fileStat.st_size)/
                    ((double) fileStat.st_blksize)));

  //Reporting Information...
  printf("=============================================================\n");
  printf("MODE: %o\n", fileStat.st_mode);
  printf("NUMBER OF LINKS: %ld\n", fileStat.st_nlink);
  printf("OWNER: %s\n", pwd->pw_name);
  printf("GROUP: %s\n", grp->gr_name);
  printf("SIZE(in Bytes): %ld\n", fileStat.st_size);
  printf("SIZE(in blocks): %d\n", sizeBlocks);
  printf("LAST MODIFICATION TIME: %s\n", buff);
  printf("NAME: %s\n", fileName);
  printf("=============================================================\n");

  //If node is a directory, return 1.
  //Else, i.e., node is file, return 2.
  if (S_ISDIR(fileStat.st_mode)){
    return 1;
  }
  else {
    return 2;
  }
}

//The below function will traverse the directories in a recurssive manner..
void traverseDir(char* fileName){
  char currDir[256];
  struct dirent* dirEnt;
  char* innerFile;

  //Opening Directory...
  DIR* dir = opendir(fileName);
  //Error checking..
  if (dir == NULL){
    perror("ERROR! Cannot open Directory");
    return;
  }

  //Save current directory in currDir.
  getcwd(currDir, sizeof(currDir));

  if (chdir(fileName)){
    perror("ERROR! Cannot enter directory.");
    return;
  }

  //this is where the magic happens..
  while ((dirEnt = readdir(dir)) != NULL){
    if (strcmp(dirEnt->d_name, ".") == 0 || strcmp(dirEnt->d_name, "..") == 0)
      continue;
    innerFile = dirEnt->d_name;
    int dirFlag = reportAttributes(innerFile);
    if (dirFlag == 1 ){
      printf("%s is a Directory.\nTraversing %s...\n", innerFile, innerFile);
      traverseDir(innerFile); //recurssive call
    }
    else if (dirFlag == 2){
      //We have reached a file..
      printf("%s is a File\n", innerFile);
      printf("Traverse %s\n", fileName);
    }
  }
  //Now traversing is done
  printf("Completed Traversing %s\n", fileName);
  closedir(dir);
  chdir(currDir);
}


int main(int argc, char **argv) {
  //Ensuring correct format of cmnd line args..
  if (argc != 2){
    printf("ERROR! Please refer to USAGE and enter appropriate arguments\n");
    return 1;
  }
  char* input = argv[1];
  //status of given directory/file
  int inputFlag = reportAttributes(input);

  if (inputFlag == 1){
    printf("Traversing %s...\n", input);
    traverseDir(input); //recurssive call
  }
  else if (inputFlag == 2){
  printf("%s is a File.\n", input);
  }
  return 0;
}
