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
char  *shared_data[1000];

void closeServer(int signal)
{
	printf("\n[Serwer]: Konczenie pracy, czyszczenie pamieci");

	printf(" (odlaczenie: %s, usuniecie: %s)\n", 
			(shmdt(shared_data) == 0)        ?"OK":"blad shmdt",
			(shmctl(shmid, IPC_RMID, 0) == 0)?"OK":"blad shmctl");

	exit(0);
}

void printRecords(int signal)
{
	printf("\33[2K\r%s", shared_data[0]);
	printf("\33[2K\r%s", shared_data[1]);
	printf("\33[2K\r%s", shared_data[2]);
	printf("\ntest\n");
}

int main(int argc, char * argv[])
{
	int n;
	struct shmid_ds buf;
	
	if (argc != 2)
    	{
        perror("Nieprawidlowa ilosc argumentow");
        exit(1);
    	}
	
	signal(SIGINT, closeServer);
	signal(SIGTSTP, printRecords);

	n = atoi(argv[1]);
	printf("%d", n);

	printf("[Serwer]: tworze klucz na bazie nazwy serwera: %s", argv[0]);

    if( (shmkey = ftok(argv[0], 1)) == -1) 
	{
	    printf("Blad tworzenia klucza!\n");
		exit(1);
	}

	printf(" OK (klucz: %d)\n", shmkey);

	printf("[Serwer]: tworze segment pamieci wspolnej...");

	if( (shmid = shmget(shmkey, n * sizeof(*shared_data[1000]), 0600 | IPC_CREAT | IPC_EXCL)) == -1) 
	{
		printf(" blad shmget!\n");
		exit(1);
	}

	shmctl(shmid, IPC_STAT, &buf);
	printf(" OK (id: %d, rozmiar: %zub)\n", shmid, buf.shm_segsz);
	
	printf("[Serwer]: dolaczam pamiec wspolna...");

	shared_data = (char *) shmat(shmid, (void *)0, 0);

	if(shared_data == (char *)-1)
	{
		printf(" blad shmat!\n");
		exit(1);
	}

	printf(" OK (adres: %lX)\n", (long int)shared_data);

	shared_data[0] = '\0';

	printf("\n[Serwer]: CTRL + Z wyswietla ksiege, CTRL + C zamyka serwer");

	for(;;) 
		sleep(10);
}