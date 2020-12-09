#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define MY_MSG_SIZE 1000

key_t shmkey;
int shmid;
char *shared_data;
char buf[MY_MSG_SIZE];
char buf2[MY_MSG_SIZE];


int main(int argc, char * argv[]) {

    if (argc != 3)
    {
        perror("Nieprawidlowa ilosc argumentow");
        exit(1);
    }

	printf("[Klient]: Tworzenie klucza na bazie podanego pliku \"%s\"", argv[1]);

	if( (shmkey = ftok(argv[1], 1)) == -1)
	{
		printf("Blad tworzenia klucza!\n");
		exit(1);
	}

	printf("[Klient]: otwieram segment pamieci wspolnej");

	if( (shmid = shmget(shmkey, 0, 0)) == -1 )
	{
		printf(" blad shmget\n");
		exit(1);
	}

	printf("[Klient]: dolaczam pamiec wspolna...");

	shared_data = (char *) shmat(shmid, (void *)0, 0);

	if(shared_data == (char *)-1)
	{
		printf(" blad shmat!\n");
		exit(1);
	}

	printf("[Klient]: podaj komunikat ktory chcesz wpisac do pamieci wspolnej:\n");
	fgets(buf2, MY_MSG_SIZE, stdin);

	sprintf(buf, "[%s]: %s", argv[2], buf2);
	
	/* wpisywanie do pamieci dzielonej */
	buf[strlen(buf) - 1] = '\0'; /* techniczne: usuwam koniec linii */
	strcpy(shared_data, buf);
	
	printf("[Klient]: wpisalem komunikat do pamieci wspolnej\n");

	shmdt(shared_data);

	return 0;

}