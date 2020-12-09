#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define USR_REC_SIZE 1000
#define USR_NAME_SIZE 50

key_t shmkey;
int shmid;
int semid;
char buf[USR_REC_SIZE];
struct sembuf sb;

struct recordData
{
	int n;
	int counter;
	char record[USR_REC_SIZE];
	char username[USR_NAME_SIZE];
} *shared_data;

int main(int argc, char * argv[]) {

	int i;
    if (argc != 3)
    {
        printf("Nieprawidlowa ilosc argumentow");
        exit(1);
    }

	printf("\n[Klient]: Tworzenie klucza na bazie podanego pliku \"%s\"...", argv[1]);

	if( (shmkey = ftok(argv[1], 1)) == -1)
	{
		printf("Blad tworzenia klucza!\n");
		exit(1);
	}

	printf("\n[Klient]: otwieram segment pamieci wspolnej...");

	if( (shmid = shmget(shmkey, 0, 0)) == -1 )
	{
		printf(" blad shmget\n");
		exit(1);
	}

	printf("\n[Klient]: Dołączenie segmentu pamięci wspolnej, laczenie z semaforem, prosze czekac...");

	shared_data = (struct recordData *) shmat(shmid, (void *)0, 0);

	if(shared_data == (struct recordData *) - 1)
	{
		printf(" blad shmat!\n");
		exit(1);
	}

	if ((semid = semget(shmkey, 1, 0)) == -1)
	{
        printf("ERROR semget");
        exit(1);
    }

	sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;

	semop(semid, &sb, 1);
	printf("\n[Klient]: Polaczenie udane!");
	printf("\n[Klient]:" Zajetych slotow wpisow: %d / %d\n", shared_data[0].counter, shared_data[0].n);

	if(shared_data[0].counter < shared_data[0].n)
	{
		i = shared_data[0].counter;

		printf("[Klient]: Opisz swój problem:\n");
		fgets(buf, USR_REC_SIZE, stdin);

		buf[strlen(buf) - 1] = '\0';
		strcpy(shared_data[i].record, buf);
		strcpy(shared_data[i].username, argv[2]);
		shared_data[0].counter++;
		
		printf("[Klient]: Komunikat zostal wpisany w slocie %d\n", i);
	}

	else
		printf("[Klient]: Brak wolnych slotow w ksiedze");
	
	printf("[Klient]: Odlaczanie pamieci, odblokowanie semafora, konczenie pracy programu\n");

	sb.sem_op = 1;
	semop(semid, &sb, 1);
	shmdt(shared_data);
	
	return 0;

}