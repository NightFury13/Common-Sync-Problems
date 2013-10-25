#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

void seed()
{
	srand(time(NULL));
}

typedef struct s
{
	int branch;
	int alloted[4];
	int prefer[8];
	int n_allot;
}s;

typedef struct c
{
	int n_stud;
	int spec;
	int branch_stu[4];
}c;

s student[100000];
c course[100000];

sem_t lock;

int n_student,n_course;

void allot(void *tp)
{
	int st = *(int *)tp;
	s current = student[st];
	int c_no;
	
	sem_wait(&lock); // critical section.

	int loop=0,i,mult;
	i=rand()%8;
	while(loop<8)
	{
		i=i%8;
		mult=12;
		c_no = current.prefer[i];
		if (course[c_no].n_stud < 60) //check overall strength constraint.
		{
			if (current.branch==2)
			{
				mult=24;
			}
			if (course[c_no].branch_stu[current.branch] < mult) //check specific branch quota.
			{
				course[c_no].n_stud++; //allot the course.
				course[c_no].branch_stu[current.branch]++;
				current.alloted[current.n_allot++]=c_no;
				i++;
				loop++;
				continue;
			}
			else
			{
				i++;//go to next preferred.
				loop++;
				continue;
			}
		}
		else
		{
			i++;//go to next preferred.
			loop++;
			continue;
		}
	}
	student[st]=current;
	sem_post(&lock); //critical section.
}

int main(int argc,char** argv)
{
	seed();

	n_student=atoi(argv[1]);
	n_course=atoi(argv[2]);

	int i,j;

	sem_init(&lock,0,1);

	for (i=0;i<n_student;i++)
	{
		student[i].n_allot=0; //initially no course is alloted.

		student[i].branch = rand()%4; //randomly alloting a branch to student.
		
		student[i].prefer[0] = rand()%(n_course/4); // randomly alloting courses ensuring each spectrum is covered.
		student[i].prefer[1] = rand()%(n_course/4);
		
		student[i].prefer[2] = n_course/4 + rand()%((n_course)/4);
		student[i].prefer[3] = n_course/4 + rand()%((n_course)/4);
		
		student[i].prefer[4] = (n_course/2) + rand()%((n_course)/4);
		student[i].prefer[5] = (n_course/2) + rand()%((n_course)/4);

		student[i].prefer[6] = (3*(n_course)/4) + rand()%(n_course/4);
		student[i].prefer[7] = (3*(n_course)/4) + rand()%(n_course/4);
	}

	for (i=0;i<n_course;i++)
	{
		course[i].n_stud = 0; //initially no student is alloted a course.
		
		course[i].spec = rand()%4; //randomly alloting a spectrum.

		for (j=0;j<4;j++)
		{
			course[i].branch_stu[j]=0; //no student alloted initially.
		}
	}

	int tp;
	pthread_t stu_th[n_student]; //creating a thread for each student.

	for (i=0;i<n_student;i++)
	{
		tp=i;
		pthread_create(&stu_th[i],NULL,(void *)allot,(void *)&tp);
	}

	for (i=0;i<n_student;i++)
	{
		pthread_join(stu_th[i],NULL);
	}

	printf("\nLeft Out:\n\n");
	printf("Id | Branch | NumAllot | Priority\n");
	for (i=0;i<n_student;i++)
	{
		//print the students with n_allot<4 on stdout.
		if (student[i].n_allot < 4)
		{
			printf("%d     %d      %d      ",i,student[i].branch,student[i].n_allot);
			for(j=0;j<8;j++)
			{
				printf("%d  ",student[i].prefer[j]);
			}
			printf("\n");
		}
	}
	
	//print the details of other students in a file.
	FILE *fp = fopen("allocation.txt","w");
	fprintf(fp,"Course No | Total | Spectrum | M.Com B.Com PhD Arts\n");
	for (i=0;i<n_course;i++)
	{
		fprintf(fp,"%d               %d     %d           %d   %d   %d   %d\n",i,course[i].n_stud,course[i].spec,course[i].branch_stu[0],course[i].branch_stu[1],course[i].branch_stu[2],course[i].branch_stu[3]);
	}
	fclose(fp);
	printf("\n");
	return 0;
}
