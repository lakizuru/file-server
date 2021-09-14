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
  const char	*cli_ip;
  char	cli_ip_dest[4096];

  // Setting up logging
  FILE *log;
  log = fopen("log.txt", "a");
  fprintf(log, "[+]Server started!\n");

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("[-]Error in socket");
    fprintf(log, "[-]Error in socket\n");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");
  fprintf(log, "[+]Server ocket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (e < 0)
  {
    perror("[-]Error in bind");
    fprintf(log, "[-]Error in bind\n");
    exit(1);
  }
  printf("[+]Binding successfull.\n");
  fprintf(log, "[+]Binding successfull.\n[+]Server is in LISTENING state\n");

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

  // Get CLI IP into presentation format for logging.
  if( (cli_ip = inet_ntop(AF_INET, &new_addr.sin_addr, cli_ip_dest, sizeof(cli_ip_dest))) == NULL) {
		fprintf(stderr, "inet_ntop error \n");
		exit (1);
  }

  printf("[+]New client connection from %s\n", cli_ip );
  fprintf(log, "[+]New client connection from %s\n", cli_ip);

  // Sending maximum file Size
  if (send(new_sock, &maxFileSize, sizeof(maxFileSize), 0) == -1)
    {
      perror("[-]Error in sending file size.");
      fprintf(log, "[-]Error in sending file size for client %s\n", cli_ip);
      exit(1);
    }

  // Read file size
  read(new_sock, &fileSize, sizeof(fileSize));
  if (fileSize == -1){
    printf("[-]Maximum file size of %d exceeded.\n[-]File not tranferred.\n", maxFileSize);
    fprintf(log, "[-]Maximum file size of %d exceeded from client %s\n", maxFileSize , cli_ip);
    printf("[+]Client connection from %s successfully terminated\n", cli_ip);
    fprintf(log, "[+]Client connection from %s successfully terminated\n", cli_ip);
    break;
  }

  // Read fileNameSize
  read(new_sock, &fileNameSize, sizeof(fileNameSize));
  //printf("%d\n", fileNameSize);
  char *fileName[fileNameSize]; 
  bzero(fileName, sizeof(fileName));

 // Read file Name
 read(new_sock, &fileName, sizeof(fileName));
 printf("[+]File created as \'%s\'\n", fileName);
 fprintf(log, "[+]File %s created for client %s\n", fileName, cli_ip);

  write_file(new_sock, fileSize);
  rename("recv.txt", fileName);
  printf("[+]Data written in the file successfully.\n");
  fprintf(log, "[+]File %s saved successfully for client %s\n", fileName, cli_ip);
  printf("[+]Client connection from %s successfully terminated\n", cli_ip );
  fprintf(log, "[+]Client connection from %s successfully terminated\n", cli_ip);
}
  fprintf(log, "[+]Stopping Server\n");
  fclose(log);
  return 0;
}
