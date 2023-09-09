#include "edfvd_functions.h"

extern int hyper_period;
extern task *t; //dynamic array of task structures
extern int number_of_tasks;

int main(int argc, char *argv[])
{
	
	int n;
	FILE *fp;
	fp = fopen("C:\\Users/\\Varsha\\Desktop\\Systems\\input.txt","r");
	if(!fp)
		perror("fopen");
	
	fscanf(fp, "%d",&n);
	if(n==0) return 0;
	number_of_tasks=n;
	t=malloc(n*sizeof(task));
	
	int i=0;
	task *t1 = t;
	while(i<n)
	{
		fscanf(fp, "%d %d %d %d %d %d ", &t1->task_release_time,&t1->task_execution_time,&t1->task_actual_deadline,&t1->task_criticality,&t1->task_wcet_levels[0],&t1->task_wcet_levels[1]);
		t1->task_criticality--;
		t1->task_id=i;
		t1->task_virtual_deadline=(float)(t1->task_actual_deadline); 
		t1++;
		i++;
	}
	fclose(fp);
	
	// We need to find hyperperiod as we print out job schedules for every hyperperiod.
	hyper_period=hyperperiod_calc(t,n);
	printf("\nHyperperiod: %d",hyper_period);
	bool is_schedulable;
	is_schedulable = schedulable_offline_preprocessing(t);
	if(is_schedulable== false)
	{
		printf("\nGiven system cannot be scheduled!");
		return 0;
	}
	else printf("\nGiven system can be scheduled.\n");
	// Print out job schedule.
	runtime(n);
	
	
	printf("\nFinish");
	return 0;
}