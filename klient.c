#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define MY_MSG_SIZE 1000

key_t shmkey;
int shmid;
char buf[MY_MSG_SIZE + 50];
char buf2[MY_MSG_SIZE];

struct recordData
{
	int n;
	int counter;
	char record[MY_MSG_SIZE];
} *shared_data;

int main(int argc, char * argv[]) {

	int i;
    if (argc != 3)
    {
        printf("Nieprawidlowa ilosc argumentow");
        exit(1);
    }

	printf("\n[Klient]: Tworzenie klucza na bazie podanego pliku \"%s\"", argv[1]);

	if( (shmkey = ftok(argv[1], 1)) == -1)
	{
		printf("Blad tworzenia klucza!\n");
		exit(1);
	}

	printf("\n[Klient]: otwieram segment pamieci wspolnej");

	if( (shmid = shmget(shmkey, 0, 0)) == -1 )
	{
		printf(" blad shmget\n");
		exit(1);
	}

	printf("\n[Klient]: Dołączenie segmentu pamięci wspólnej");

	shared_data = (struct recordData *) shmat(shmid, (void *)0, 0);

	if(shared_data == (struct recordData *) - 1)
	{
		printf(" blad shmat!\n");
		exit(1);
	}

	printf("\n[Serwer]: Zajetych slotow: %d / %d\n", shared_data[0].counter, shared_data[0].n);

	if(shared_data[0].counter < shared_data[0].n)
	{
		i = shared_data[0].counter;

		printf("[Klient]: Opisz swój problem:\n");
		fgets(buf2, MY_MSG_SIZE, stdin);

		sprintf(buf, "[%s]: %s", argv[2], buf2);
		//buf[strlen(buf) - 1] = '\0';
		strcpy(shared_data[i].record, buf);
		shared_data[0].counter++;
		
		printf("[Klient]: Komunikat zostal wpisany w slocie %d%d\n", i);
	}

	else
		printf("[Klient]: Brak wolnych slotow w ksiedze");
	
	shmdt(shared_data);
	printf("[Klient]: Odlaczanie pamieci, konczenie pracy programu\n");

	return 0;

}