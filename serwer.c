#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<signal.h>
#include<unistd.h>

#define MY_MSG_SIZE 1000

key_t shmkey;
int shmid;
int counter;
char  *shared_data;

void closeServer(int signal)
{
	printf("\n[Serwer]: Usuwanie pamieci wspoldzielonej");
	
	shmdt(shared_data);
	shmctl(shmid, IPC_RMID, 0;

	exit(0);
}

void printRecords(int signal)
{
	printf("\ntest\n");
}

int main(int argc, char * argv[])
{
	struct shmid_ds buf;
	
	if (argc != 2)
    	{
        perror("Nieprawidlowa ilosc argumentow");
        return 1;
    	}
	
	signal(SIGINT, closeServer);
	signal(SIGTSTP, printRecords);

	printf("[Serwer]: tworze klucz na bazie nazwy serwera: %s", argv[0]);

    if( (shmkey = ftok(argv[0], 1)) == -1) 
	{
	    printf("Blad tworzenia klucza!\n");
		exit(1);
	}

	printf("\n[Serwer]: tworze segment pamieci wspolnej");

	if( (shmid = shmget(shmkey, MY_MSG_SIZE * sizeof(char), 0600 | IPC_CREAT | IPC_EXCL)) == -1) 
	{
		printf(" blad shmget!\n");
		exit(1);
	}
	
	printf("\n[Serwer]: dolaczam pamiec wspolna");

	shared_data = (char *) shmat(shmid, (void *)0, 0);

	if(shared_data == (char *)-1)
	{
		printf(" blad shmat!\n");
		exit(1);
	}

	shared_data[0] = '\0';

	printf("\n[Serwer]: Aby wyswieltlic wpisy do ksiegi, wcisnij CTRL + Z");
	printf("\n[Serwer]: Aby zakonczyc prace serwera, wcisnij CTRL + C\n");

	for(;;) 
	{
		printf("\33[2K\r%s", shared_data);
		fflush(stdout); /* trik by nadpisywanie sie udalo */
		sleep(10);
	}

}

