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
int n;

struct recordData
{
	int n;
	int counter;
	char record[MY_MSG_SIZE];
} *shared_data;

void closeServer(int signal)
{
	printf("\n[Serwer]: Usuwanie pamieci wspoldzielonej");
	
	shmdt(shared_data);
	shmctl(shmid, IPC_RMID, 0);

	exit(0);
}

void printRecords(int signal)
{	
	int i;
	printf("\n[Serwer]: Ksiega skarg i wnioskow:");
	printf("\n[Serwer]: Zajetych slotow: %d / %d\n", shared_data[0].counter, shared_data[0].n);

	for(i = 0; i < shared_data[0].counter; i++)
	{
		fflush(stdout);
		printf("%s", shared_data[i].record);
	}
	
	printf("\n[Serwer]: Koniec wpisow ksiegi");
}

int main(int argc, char * argv[])
{	
	if (argc != 2)
    {
        printf("Nieprawidlowa ilosc argumentow");
        exit(1);
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

	if( (shmid = shmget(shmkey, atoi(argv[1]) * sizeof(struct recordData), 0600 | IPC_CREAT | IPC_EXCL)) == -1) 
	{
		printf(" blad shmget!\n");
		exit(1);
	}
	
	printf("\n[Serwer]: dolaczam pamiec wspolna");

	shared_data = (struct recordData *) shmat(shmid, (void *)0, 0);

	if(shared_data == (struct recordData *) - 1)
	{
		printf(" blad shmat!\n");
		exit(1);
	}

	shared_data[0].n = atoi(argv[1]);
	shared_data[0].counter = 0;

	printf("\n[Serwer]: Utworzono slotow: %d", shared_data[0].n);
	printf("\n[Serwer]: Aby wyswieltlic wpisy do ksiegi, wcisnij CTRL + Z");
	printf("\n[Serwer]: Aby zakonczyc prace serwera, wcisnij CTRL + C\n");

	for(;;) 
	{
		sleep(1);
	}

}

