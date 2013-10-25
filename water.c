#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sync_th;

sem_t hyd,oxy; //using semaphores to keep count.
int chip[10000]; //chip where reactions take place.
int energy=0; //the current energy of reactions.
int left_h,left_o; //to keep track of remaining atoms.
int num_site;
int ener_th;

void ready_h(void *tp)
{
	//printf("hyd\n");
	sem_post(&hyd); // make available first to avoid a deadlock. ****doing so gives more priority to Oxygen****
	sem_wait(&oxy);
}

void ready_o(void *tp)
{
	//printf("oxy\n");
	sem_wait(&hyd); //wait for 2 hyd.
	sem_wait(&hyd);

	while(energy+1>=ener_th)//check for energy constraint.
	{
		//do nothing.
	}
	energy++;

	
	int j=0,i=0,loc,fl=0;
	sem_wait(&sync_th);
	for (i=0;1;i++)//find suitable location for reaction.
	{
		if (chip[i]==0)
		{
			if ((i!=0 && chip[i-1]==0) && (i!=num_site-1 && chip[i+1]==0))
			{
				chip[i]=1;
				loc=i;
				break;
			}
			else if (i==0 && chip[i+1]==0)
			{
				chip[i]=1;
				loc=i;
				break;
			}
			else if (i==num_site-1 && chip[i-1]==0)
			{
				chip[i]=1;
				loc=i;
				break;
			}
		}
		if (i+1==num_site)
		{
			i=0;
		}
	}
	sem_post(&sync_th);
	sleep(3);
	sem_post(&oxy); //form a water molecule.
	sem_post(&oxy);

	left_h-=2;
	left_o-=1;

	printf("Water Formed at chip location %d. Total Energy : %d. Left Atoms : (%d H) (%d O).\n",loc,energy,left_h,left_o);
	
	chip[i]=0;
	energy-=1; //completing the process at a site.
}


int main(int argc,char** argv)
{
	int n=atoi(argv[3]); //number of sites.
	int nh=atoi(argv[1]); //number of H atoms.
	int no=atoi(argv[2]); //number of O atoms.
	int thresh=atoi(argv[4]); //threshold energy.


////////////////BEAUTIFICATION////////////////////

	printf("\n\n                             The Chip of Destiny\n\n");
	printf("We have : %d locations on chip with %d Hydrogen & %d Oxygen atoms to create water under Energy Barrier %d EMJ\n\n\n",n,nh,no,thresh);

/////////////////////////////////////////////////

	left_h=nh;
	left_o=no;
	num_site=n;
	ener_th=thresh;

	int i;
	char* tp;

	pthread_t hydth[10000];
	pthread_t oxyth[10000];

	sem_init(&hyd,0,1);
	sem_init(&oxy,0,1);
	sem_init(&sync_th,0,1);

	for (i=0;i<n;i++)
	{
		chip[i]=0;
	}

	for (i=0;i<nh;i++) //loop for Hydrogen atoms.
	{
		pthread_create(&hydth[i],NULL,(void *)ready_h,(void*) tp);
	}
	for (i=0;i<no;i++) // loop for Oxygen atoms.
	{
		pthread_create(&oxyth[i],NULL,(void *)ready_o,(void*) tp);
	}
	
	for (i=0;i<nh;i++)
	{
		pthread_join(hydth[i],NULL);
	}
	for (i=0;i<no;i++)
	{
		pthread_join(oxyth[i],NULL);
	}

	sem_destroy(&hyd);
	sem_destroy(&oxy);

	printf("\n");
	return 0;
}
