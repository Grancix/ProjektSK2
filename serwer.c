#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/msg.h>
#include<signal.h>
#include<unistd.h>

#define USR_REC_SIZE 1000
#define USR_NAME_SIZE 50

key_t shmkey;
int shmid;
int semid;

struct sembuf sb; 
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;

int n;

struct recordData
{
	int n;
	int counter;
	char record[USR_REC_SIZE];
	char username[USR_NAME_SIZE];
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
		printf("[%s]: ", shared_data[i].username);
		printf("%s\n", shared_data[i].record);
	}
	
	printf("\n");
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

	printf("[Serwer]: Tworzenie klucza na bazie nazwy serwera: %s...", argv[0]);

    if( (shmkey = ftok(argv[0], 1)) == -1) 
	{
	    printf("ERROR ftok!\n");
		exit(1);
	}

	printf("\n[Serwer]: Tworzenie pamieci wspolnej...");

	if( (shmid = shmget(shmkey, atoi(argv[1]) * sizeof(struct recordData), 0600 | IPC_CREAT | IPC_EXCL)) == -1) 
	{
		printf("ERROR shmget!\n");
		exit(1);
	}
	
	printf("\n[Serwer]: Dolaczanie pamieci wspolnej...");

	shared_data = (struct recordData *) shmat(shmid, (void *)0, 0);

	if(shared_data == (struct recordData *) - 1)
	{
		printf("ERROR shmat!\n");
		exit(1);
	}

	printf("\n[Serwer]: Tworzenie semafora i otwieranie go...");
	if ((semid = semget(shmkey, 1, 0)) == -1) {
        printf("ERROR semget!\n");
        exit(1);
    }
	printf("--%d--", semid);

	shared_data[0].n = atoi(argv[1]);
	shared_data[0].counter = 0;

	printf("\n[Serwer]: Utworzono slotow na wpisy: %d", shared_data[0].n);
	printf("\n[Serwer]: Aby wyswieltlic wpisy do ksiegi, wcisnij CTRL + Z");
	printf("\n[Serwer]: Aby zakonczyc prace serwera, wcisnij CTRL + C\n");

	for(;;) 
	{
		sleep(1);
	}

	return 0;
}

