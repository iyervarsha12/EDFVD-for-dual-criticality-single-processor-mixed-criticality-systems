#include "edfvd_functions.h"
#include <stdio.h>


int timer=-1;
int hyper_period;
int number_of_tasks=0;
int no_levels=2;
task *t;
job *jobs_to_add;
int number_of_jobs=0; //number of jobs to be added


int system_level=0;
node *head = NULL;
node *tail = NULL;
;

bool criticality_change_flag=false;

// Helper functions.
int gcd(int a, int b)
{
  if (b == 0)
	  return a;
  else
  return gcd(b, a%b);
}

int lcm(int *a, int n)
{
  int res = 1, i;
  for (i = 0; i < n; i++)
  {
    res = res*a[i]/gcd(res, a[i]);
  }
  return res;
}

// Calculating hyperperiod.
int hyperperiod_calc(task *t1,int n)
{
	int i=0,ht,a[10];
	while(i<n)

		{
		a[i]=t1->task_actual_deadline;
		t1++;
		i++;
		}
	ht=lcm(a,n);
	return ht;
}


// Used in ulksummer and ullsummer.
float ulkcalculator(task *t1, int l, int k)
{
	float ulk=0.0;
	int i=0;
	while(i<number_of_tasks)
	{
		if(t1->task_criticality == l)
		{
			ulk = ulk + (float)(t1->task_wcet_levels[k]) / (float)(t1->task_actual_deadline);
		}
		t1++;
		i++;
	}
	return ulk;
}

// Used to figure out x-factor for preprocessing phase.
float ulksummer(task *t1, int llt, int hlt, int k)
{
	float ulksum=0.0;
	
	int i=0;
	for(int l=llt;l<=hlt; l++)
		ulksum=ulksum + ulkcalculator(t1,l,k);
	return ulksum;
}

// Used to figure out x-factor for preprocessing phase.
float ullsummer(task *t1, int llt, int hlt)
{
	float ullsum=0.0;
	for(int l=llt;l<=hlt; l++)
		ullsum=ullsum + ulkcalculator(t1,l,l);
	return ullsum;
}

// Figure out if system is schedulable - if yes, figure out x factor and update virtual deadlines.
bool schedulable_offline_preprocessing(task *t1)
{
	float x_factor=1;
	if(ullsummer(t1,0,no_levels-1) <=1.0)
	{	
		int i=0;
			while(i<number_of_tasks)
			{
				;
				t1->task_virtual_deadline = t1->task_actual_deadline;
				t1++;
				i++;
			}
			printf("\nx_factor = 1");
			return true;
	}
	else
	{
		int k=0;
		for(k=0;k<no_levels-1;k++)
		{
			float expr1 = ulksummer(t1,k+1,no_levels-1,k)/(1-ullsummer(t1,0,k));
			float expr2=(1-ullsummer(t1,k+1,no_levels-1))/ullsummer(t1,0,k);
			if ((1-ullsummer(t1,0,k) >0) &&(expr1<=expr2))
			{
				x_factor=(float)(expr1+expr2)/(float)2.0;
				printf("\nX-factor = %f",x_factor);
				int i=0;
				while(i<number_of_tasks)
				{
					if(t1->task_criticality <= k)
						t1->task_virtual_deadline = (float)t1->task_actual_deadline;
					else
						t1->task_virtual_deadline = x_factor*(float)(t1->task_actual_deadline);
					
					i++;
					t1++;
				}
				return true;
				
			}
		}
		
	}
	
	return false;
}



int new_job_arrival(task *t1,int tmr,int n)
{
	int i=0;
	int next_arrival=2*hyper_period;
	job *to_be_added = malloc(sizeof(job));
	job *iterate = to_be_added;
	jobs_to_add=to_be_added;
	number_of_jobs=0;
	while(i<n)
	{
		int job_arrival_of_task;
		int k;
		for(k=0;k<=hyper_period/(t1->task_actual_deadline);k++)
		{
			
			if((timer< t1->task_release_time + k*t1->task_actual_deadline)&&(t1->task_criticality>=system_level))
			{
				job_arrival_of_task=t1->task_release_time + k*t1->task_actual_deadline;
				goto cont;
			}
		}
		return 0;
		
		cont:
		;
		if(next_arrival>job_arrival_of_task)
		{
			number_of_jobs=1;
			free(to_be_added);
			to_be_added = (job*) malloc((size_t) number_of_jobs*sizeof(job));
			to_be_added->job_id=t1->task_id;
			
			to_be_added->job_execution_time_copy = t1->task_execution_time;
			to_be_added->job_wcet_levels[0] = t1->task_wcet_levels[0];
			to_be_added->job_wcet_levels[1] = t1->task_wcet_levels[1];
			to_be_added->job_criticality = t1->task_criticality;
			
						
			to_be_added->job_release_time = job_arrival_of_task;
			to_be_added->job_execution_time = t1->task_execution_time;
			to_be_added->job_actual_deadline = (k+1)*t1->task_actual_deadline + t1->task_release_time;
			to_be_added->job_virtual_deadline = (float)(k*t1->task_actual_deadline) + t1->task_virtual_deadline + (float)t1->task_release_time;
			next_arrival=job_arrival_of_task;
		}
		else if(next_arrival==job_arrival_of_task)
		{
			number_of_jobs++;
			to_be_added = (job*) realloc(to_be_added,(size_t) number_of_jobs*sizeof(job));
			to_be_added++;
			to_be_added->job_id=t1->task_id;
			
			to_be_added->job_execution_time_copy = t1->task_execution_time;
			to_be_added->job_wcet_levels[0] = t1->task_wcet_levels[0];
			to_be_added->job_wcet_levels[1] = t1->task_wcet_levels[1];
			to_be_added->job_criticality = t1->task_criticality;
			
			to_be_added->job_release_time = job_arrival_of_task;
			to_be_added->job_execution_time = t1->task_execution_time;
			to_be_added->job_actual_deadline = (k+1)*t1->task_actual_deadline+ t1->task_release_time;
			to_be_added->job_virtual_deadline = (float)(k*t1->task_actual_deadline) + t1->task_virtual_deadline+ (float)t1->task_release_time;
			
		}
		t1++;
		i++;
	}

	return next_arrival;
}

bool insert_job(job *add)
{
	node *node_to_add = malloc(sizeof(node));
	node_to_add->job_node = add;
	node_to_add->next = NULL;
	
	if(head==NULL) //empty
	{
		head = node_to_add;
		head->next = NULL;
		tail=head;
		return true;
	}
	else
	{
		
		node *temp = head;
		if(add->job_virtual_deadline == (head->job_node)->job_virtual_deadline)
		{
			node_to_add->next = head->next;
			head->next = node_to_add;
			return true;
		}
		else if(add->job_virtual_deadline < (head->job_node)->job_virtual_deadline)
		{
			
			node_to_add->next = head;
			head = node_to_add;
			return true;
		}
		//what if add at the end?
		else if(add->job_virtual_deadline >= (tail->job_node)->job_virtual_deadline) 
		{
			tail->next = node_to_add;
			tail = node_to_add;
			tail->next =NULL;
			return true;
		}
		else 
		{
			while(temp->next!=NULL)
			{
				if((add->job_virtual_deadline >= (temp->job_node)->job_virtual_deadline)&&(add->job_virtual_deadline <= ((temp->next)->job_node)->job_virtual_deadline))
				{
					node_to_add->next = temp->next;
					temp->next = node_to_add;
					return true;
				}
				temp=temp->next;
			}
		}
	}
	return false;
	
}


void criticality_change_taskparameter_change(task *t1)
{
	int i=0;
	while(i<number_of_tasks)
	{
		t1->task_virtual_deadline = t1->task_actual_deadline;
		i++;
		t1++;
	}
}


bool criticality_change_job_removal()
{
	node *temp = head;
	node *temp_previous = NULL;
	if((head==tail)&&((head->job_node)->job_criticality <system_level))
	{
		node* to_delete = head;
		head = NULL;
		tail = NULL;
		free(to_delete);	
		return true;
	}
	if((head==tail)&&((head->job_node)->job_criticality >=system_level))
		(head->job_node)->job_virtual_deadline = (head->job_node)->job_actual_deadline;
	
	while(temp->next!=NULL)
	{
		if((temp->job_node)->job_criticality <system_level) //1
		{
			//if delete from head
			if(temp==head)
			{
				node* to_delete = head;
				if(head->next==NULL)
					head=NULL;
				else
					head=head->next;
				free(to_delete);
			}
			else
			{
				node *to_delete = temp;
				temp_previous->next = temp->next;
				free(to_delete);
				
			}
		}
		else
		{
			(temp->job_node)->job_virtual_deadline = (temp->job_node)->job_actual_deadline;
		}
		
		temp_previous = temp;
		temp = temp->next;
		
	}
	
	
	if((tail->job_node)->job_criticality <system_level)
			{
				node *to_delete = tail;
				tail = temp_previous;
				tail->next=NULL;
				free(to_delete);
			}
	else (tail->job_node)->job_virtual_deadline = (tail->job_node)->job_actual_deadline;
	return true;
	
}





void runtime(int n)
{
	
	int next_job_arrival = new_job_arrival(t,timer,n); 
	while(timer< hyper_period)
	  {
	
		 // If job arrival time:
		 if(timer==next_job_arrival)
		 {
			// Add jobs from to_be_added into job queue
			job *temp = jobs_to_add;
			int x=0;
			while(x<number_of_jobs)
			{
				insert_job(temp);
				x++;
				temp++;
			}
			// Update next closest job arrival time
			next_job_arrival = new_job_arrival(t,timer+1,n);
			
		 }
		 // If there is still a job to run:
		 if(head!=NULL)
		 {
			(head->job_node)->job_execution_time--;
			
			if(((head->job_node)->job_execution_time_copy - (head->job_node)->job_execution_time > (head->job_node)->job_wcet_levels[system_level])&&(system_level==0))
			{
				criticality_change_flag=true;
				
				printf("\nAt time %d,Criticality change!",timer);
				criticality_change_flag=true;
				system_level++;
				criticality_change_job_removal();
				criticality_change_taskparameter_change(t);
				
			}
		 }
		// If there is no job to run, the processor is idle.
		if((head==NULL)&&(timer>=0))
			 printf("\nTime %d: IDLE",timer);
		else if(timer>=0) 
			printf("\nTime %d: task %d", timer,(head->job_node)->job_id + 1);
		timer++; 
		 
		 // If the job queue is not free, and the job at the head has finished execution, remove the head.
			if((head!=NULL)&&((head->job_node)->job_execution_time ==0))
			{
				//remove head from queue
				node* to_delete = head;
				if(head->next==NULL)
					head=NULL;
				else 
					head=head->next;
				free(to_delete);
			}
		 
	  }
	  
	
}

