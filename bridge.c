#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

sem_t geek;
sem_t nongeek;
sem_t lock;

int n_geek,n_nongeek,n_singer;
int wt_geek,al_geek;
int wt_nongeek,al_nongeek;
int wt_singer,al_singer;
int onbridge;

void arrives(void *tp)
{
	int fl;
	fl =*(int *)tp;
	if (fl==0)
	{
		sem_wait(&geek);
		wt_geek++;
		sem_post(&geek);
	}
	else if (fl==1)
	{
		sem_wait(&nongeek);
		wt_nongeek++;
		sem_post(&nongeek);
	}
	else if (fl==2)
	{
		//take lite. its a singer.
	}

	sem_wait(&lock); //critcal section.

	al_singer=al_geek=al_nongeek=onbridge=0;//initialization for each thread.
	if (wt_singer>0)
	{

		if (wt_geek>=2 && wt_nongeek>=1) // 2geeks+1nongeek+1singer. 
		{
			wt_singer--;
			al_singer++;
			onbridge++; //singer given most priority, so it will be on bridge no matter what.
	
			wt_geek-=2;
			al_geek+=2;//giving 2 geeks together.
			onbridge+=2;

			wt_nongeek--;
			al_nongeek++;
			onbridge++;

			//cross the bridge.
		}
		else if (wt_nongeek>=2 && wt_geek>=1) // 2nongeeks+1geek+1singer.
		{
			wt_singer--;
			al_singer++;
			onbridge++; //singer given most priority, so it will be on bridge no matter what.
			
			wt_nongeek-=2;
			al_nongeek+=2;//giving 2 non-geeks together.
			onbridge+=2;

			wt_geek--;
			al_geek++;
			onbridge++;

			//cross the bridge.
		}
		else if (wt_geek>=3 && n_nongeek==0)
		{
			wt_singer--;
			al_singer++;
			onbridge++; //singer given most priority, so it will be on bridge no matter what.

			wt_geek-=3;
			al_geek+=3;
			onbridge+=3;

			//cross the bridge.
		}
		else if (wt_nongeek>=3 && n_nongeek==0)
		{
			wt_singer--;
			al_singer--;
			onbridge++;

			wt_nongeek-=3;
			al_nongeek+=3;
			onbridge+=3;

			//cross the bridge.
		}
	}
	else
	{
		if (wt_geek>=2 && wt_nongeek>=2)
		{
			wt_geek-=2;
			al_geek+=2;
			onbridge+=2;

			wt_nongeek-=2;
			al_nongeek+=2;
			onbridge+=2;

			//cross bridge.
		}
		else if (wt_geek>=4)
		{
			wt_geek-=4;
			al_geek+=4;
			onbridge+=4;

			//cross the bridge.
		}
		else if (wt_nongeek>=4)
		{
			wt_nongeek-=4;
			al_nongeek+=4;
			onbridge+=4;

			//cross the bridge.
		}
	}

	if (onbridge==4) //if there are 4 people waiting, let'em cross!
	{
		printf("Bridge Crossed! Geeks : %d, Non-Geeks : %d & Singers : %d\n",al_geek,al_nongeek,al_singer);
	}

	sem_post(&lock); //end of critical section.
}


int main(int argc,int** argv)
{
	n_geek=atoi(argv[1]);
	n_nongeek=atoi(argv[2]);
	n_singer=atoi(argv[3]);

	//////////////BEAUTIFICATION//////////////////////////////

	printf("\n\n         Crossing the Bridge\n\n");
	printf("We have %d geeks, %d non-geeks & %d singers.\n\n",n_geek,n_nongeek,n_singer);

	/////////////////////////////////////////////////////////

	wt_singer=n_singer;
	onbridge=al_geek=al_nongeek=al_singer=0;

	sem_init(&geek,0,1);
	sem_init(&nongeek,0,1);
	sem_init(&lock,0,1);

	int i;
	int fl;

	pthread_t person[100000];
	for (i=0;i<n_geek;i++)
	{
		fl=0;
		pthread_create(&person[i],NULL,(void *)arrives,(void *)&fl);
	}
	for (i=n_geek;i<n_geek+n_nongeek;i++)
	{
		fl=1;
		pthread_create(&person[i],NULL,(void *)arrives,(void *)&fl);
	}

	for (i=0;i<n_geek+n_nongeek;i++)
	{
		pthread_join(person[i],NULL);
	}

	printf("\n");
	return 0;
}
