#include <stdio.h>
#include <string.h>//strlen
#include <fcntl.h> //open()
#include <unistd.h> //write(),read(),close()
#include <stdlib.h> //exit()

int8_t write_buf[1024];
int8_t read_buf[1024];
int main()
{
        int fd;
        char option;
        printf("*********************************\n");
        fd = open("/dev/char_device", O_RDWR);
        if(fd < 0) {
		perror("Error: ");
                printf("Cannot open device file...\n");
                return 0;
        }
        while(1) {
                printf("****Please Enter the Option******\n");
                printf("        1. Write               \n");
                printf("        2. Read                 \n");
                printf("        3. Exit                 \n");
                printf("*********************************\n");
                scanf(" %c", &option);
                printf("Your Option = %c\n", option);
                
                switch(option) {
                        case '1':
                                printf("Enter the string to write into driver :");
                                scanf("  %[^\t\n]s", write_buf);
                                printf("Data Writing ...");
                                write(fd, write_buf, strlen(write_buf)+1);
                                printf("Done!\n");
                                break;
                        case '2':
                                printf("Data Reading ...");
                                read(fd, read_buf, 1024);
                                printf("reading Done from kernel space!\n\n");
				printf("Data = %s\n\n", read_buf);

				FILE * filePointer = fopen("Text","a+");
				if ( filePointer == NULL )
				{
					printf( "Text file failed to open." ) ;
				}
				else
				{

					printf("The file is now opened.\n") ;
					if ( strlen ( read_buf ) > 0 )
					{

						fputs(read_buf, filePointer) ;
						fputs("\n", filePointer) ;
					}

					// Closing the file using fclose()
					fclose(filePointer) ;

					printf("Data successfully written in file text\n");
					printf("The file is now closed.") ;
				}

				break;
			case '3':
				close(fd);
				exit(1);
				break;
			default:
				printf("Enter Valid option = %c\n",option);
				break;
		}
	}
	close(fd);
}
