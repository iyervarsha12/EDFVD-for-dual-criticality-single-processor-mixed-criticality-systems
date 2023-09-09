# EDFVD-for-dual-criticality-single-processor-mixed-criticality-systems
This has the implementation for EDF-VD for dual criticality systems with a single processor, and are implicit. 
Assumptions:
1. This is a dual criticality system with HI and LO criticality jobs.
2. We are assuming implicit deadlines, i.e deadline = period.
3. We are assuming there is only one processor.
4. We are assuming time granulaity here is 1.
5. Input format for each task in taskset: task_release_time, task_execution_time, task_actual_deadline, task_criticality (0 for LO, 1 for HI), task_wcet_levels for LO, task_wcet_levels for HI.

Algorithm: 
1. An input of taskset is provided via an input file. Each task gives rise to a potentially unbounded sequence of jobs.
2. With the taskset provided, we figure out if the system is schedulable and calculate the 'x fasctor' which will be used to modify virtual deadlines for the tasks.
3. If the system is schedulable, we start the timer in a loop to run till hyperperiod. We figure out the closest incoming job arrival time (it can be at time 0 too). We populate jobs_to_add with incoming jobs, and update the next closest arrival time.
4. If the current time still has jobs to run, we reduce the job execution time of the head job by 1, and see if it's time for a criticality change, which happens if LO criticality jobs exceed their execution budget and be more than the allowed budget for LO job.
5. In the event of a criticality change, we remove all LO criticality jobs (talk about a loss of quality of service! To be remedied in the next project.) and all virtual deadlines are now updated with actual deadlines. It will run like a regular EDF algorithm now, with only HI jobs.
6. If processor is idle, print that out.
7. Else, print out the current running job.
8. Run this loop until timer reaches hyperperiod.
