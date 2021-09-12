#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void write_file(int sockfd, int fileSize)
{
  int n;
  FILE *fp;
  char *filename = "recv.txt";
  char *rename = "recv.jpg";

  char buffer[4096];
  bzero(buffer, sizeof(buffer));

  fp = fopen(filename, "w");

  // Reading byte array
  while(fileSize > 0){
    read(sockfd, buffer, 4096);
    fwrite(buffer, 1, sizeof(buffer), fp);
    fileSize -= 4096;
    bzero(buffer, sizeof(buffer));
  }

  fclose(fp);

  return;
}

int main(int argc, char **argv)
{
  char *ip = "127.0.0.1";
  int port = 6666;
  int e;

  int sockfd, new_sock;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;
  int fileSize;
  int fileNameSize;
  int maxFileSize = 104857600; // = 100MB

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (e < 0)
  {
    perror("[-]Error in bind");
    exit(1);
  }
  printf("[+]Binding successfull.\n");

for(;;){

  if (listen(sockfd, 10) == 0)
  {
    printf("[+]Listening....\n");
  }
  else
  {
    perror("[-]Error in listening");
    exit(1);
  }

  addr_size = sizeof(new_addr);

  new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);

  // Sending maximum file Size
  if (send(new_sock, &maxFileSize, sizeof(maxFileSize), 0) == -1)
    {
      perror("[-]Error in sending file size.");
      exit(1);
    }

  // Read file size
  read(new_sock, &fileSize, sizeof(fileSize));
  if (fileSize == -1){
    printf("[-]Maximum file size of %d exceeded.\n[-]File not tranferred.\n", maxFileSize);
    break;
  }

  // Read fileNameSize
  read(new_sock, &fileNameSize, sizeof(fileNameSize));
  printf("%d\n", fileNameSize);
  char *fileName[fileNameSize]; 
  bzero(fileName, sizeof(fileName));

 // Read file Name
 read(new_sock, &fileName, sizeof(fileName));
 printf("[+]File created as \'%s\'\n", fileName);

  write_file(new_sock, fileSize);
  rename("recv.txt", fileName);
  printf("[+]Data written in the file successfully.\n");
}

  return 0;
}
