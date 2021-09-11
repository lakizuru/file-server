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
  char buffer[fileSize];

  // Reading byte array
  read(sockfd, buffer, fileSize);

  fp = fopen(filename, "w");
  
  fwrite(buffer, 1, sizeof(buffer), fp);
  fclose(fp);

  /*
  while (1)
  {
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0)
    {
      break;
      return;
    }
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);
  }
  */
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
  //char buffer[SIZE];
  int fileSize;
  char *fileName;

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

  // Read file size
  read(new_sock, &fileSize, sizeof(fileSize));

 // Read file Name
 //read(new_sock, &fileName, sizeof(fileName));

  write_file(new_sock, fileSize);

  //printf("Save as: ");
  //scanf("%s", &fileName);

    rename("recv.txt", argv[1]);

  printf("[+]Data written in the file successfully.\n");

  return 0;
}
