#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct
{
	int task_id;
	int task_release_time;
	int task_execution_time;  //given execution time
	int task_actual_deadline; //used in level 2. HERE period = task_actual_deadline
	float task_virtual_deadline; //used in level 1
	
	int task_criticality;
	int task_wcet_levels[2]; //STARTING FROM 0
} task; 

typedef struct
{
	int job_id;
	int job_release_time; //absolute
	int job_execution_time; //remaining
	int job_actual_deadline; //absolute
	float job_virtual_deadline; //absolute 
	
	int job_criticality;
	int job_wcet_levels[2];
	int job_execution_time_copy;
}job;

struct job_queue_node
{
		job *job_node;
		struct job_queue_node *next;
};

typedef struct job_queue_node node;

int gcd(int a, int b);
int lcm(int *a, int n);
extern int hyperperiod_calc(task *t1,int n);
float ulkcalculator(task *t1, int l, int k);
float ulksummer(task *t1, int llt, int hlt, int k);
float ullsummer(task *t1, int llt, int hlt);
bool schedulable_offline_preprocessing(task *t1);
int new_job_arrival(task *t1,int tmr,int n);
bool insert_job(job *add);
void criticality_change_taskparameter_change(task *t1);
bool criticality_change_job_removal();
extern void runtime(int n);