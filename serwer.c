#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define USR_REC_SIZE 1000
#define USR_NAME_SIZE 50

key_t shmkey;
int shmid;
int semid;
int n;
struct sembuf sb;

struct recordData
{
	int n;
	int counter;
	char record[USR_REC_SIZE];
	char username[USR_NAME_SIZE];
} *shared_data;

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
} arg;



void closeServer(int signal)
{
	printf("\n[Serwer]: Usuwanie pamieci wspoldzielonej, semaforow, konczenie pracy");
	
	shmdt(shared_data);
	shmctl(shmid, IPC_RMID, 0);
	semctl(semid, 0, IPC_RMID, arg);

	exit(0);
}

void printRecords(int signal)
{	
	int i;

	printf("\n[Serwer]: ------Ksiega skarg i wnioskow---------");
	printf("\n[Serwer]: Zajetych slotow wpisow: %d / %d\n", shared_data[0].counter, shared_data[0].n);

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
        printf("[ERROR]: Nieprawidlowa ilosc argumentow");
        exit(1);
    }

	/*obsluga sygnalow CTRL + C/Z*/

	signal(SIGINT, closeServer);
	signal(SIGTSTP, printRecords);

	printf("[Serwer]: Tworzenie klucza na bazie nazwy serwera: %s...", argv[0]);

    if( (shmkey = ftok(argv[0], 1)) == -1) 
	{
	    printf("\n[ERROR]: ftok!\n");
		exit(1);
	}

	printf("\n[Serwer]: Tworzenie pamieci wspolnej...");

	/*alokacja pamieci pod sktrukture recordData*/
	if( (shmid = shmget(shmkey, atoi(argv[1]) * sizeof(struct recordData), 0600 | IPC_CREAT | IPC_EXCL)) == -1) 
	{
		printf("\n[ERROR]: shmget!\n");
		exit(1);
	}
	
	printf("\n[Serwer]: Dolaczanie pamieci wspolnej...");

	shared_data = (struct recordData *) shmat(shmid, (void *)0, 0);

	if(shared_data == (struct recordData *) - 1)
	{
		printf("\n[ERROR]: shmat!\n");
		exit(1);
	}

	sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;

	printf("\n[Serwer]: Tworzenie semafora i jego inicjalizacja...");
	if ((semid = semget(shmkey, 1, 0666 | IPC_CREAT)) == -1)
	{
        printf("\n[ERROR]: semget!\n");
        exit(1);
    }

	arg.val = 1;
	if (semctl(semid, 0, SETVAL, arg) == -1)
	{
        printf("\n[ERROR]: semctl");
        exit(1);
    }

	shared_data[0].n = atoi(argv[1]);
	shared_data[0].counter = 0;

	printf("\n[Serwer]: Utworzono slotow na wpisy: %d", shared_data[0].n);
	printf("\n[Serwer]: Aby wyswieltlic wpisy do ksiegi, wcisnij CTRL + Z");
	printf("\n[Serwer]: Aby zakonczyc prace serwera, wcisnij CTRL + C\n");

	for(;;) 
		sleep(1);

	return 0;
}