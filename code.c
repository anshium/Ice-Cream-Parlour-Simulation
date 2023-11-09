#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <limits.h> // For INT_MAX limits when finding minimum and maximum
#include <stdbool.h>
#include <string.h>

int t = 0; // global time

// #define PRINT_SCAN

#define NUM_TOPPINGS 2
#define NAME_LENGTH 100

int current_number_of_customers = 0;

int machines_closed = 0;

typedef char* String;

typedef struct machine {
	pthread_t machine_thread_identifier;
	int index;
	int tm_start;
	int tm_stop;
} machine;

typedef struct flavour {
	int index;
	String name;
	int prep_time;
} flavour;

// They say that in spite of each topping taking time t_t to put, it can be put instantaneouly.
typedef struct topping {
	int index;
	String name;
	int quantity;
} topping;

typedef struct ice_cream {
	String flavour_name;
	int num_toppings;
	topping** toppings;
} ice_cream;

typedef struct customer {
	pthread_t cust_thread_identifier;
	int c;
	int t_arr;
	int num_ice_creams;
	ice_cream** ice_creams;
} customer;

typedef struct itemOrder {
	customer* customer_info;
	int ice_cream_index;
} itemOrder;

// array for machine availability. 0 = Not Available, 1 = Available
int* isAvailable;

sem_t* order_sems;

// If I would not escape the #, it would become markdown like in VS Code
int N; // \# ice cream machines
int F; // \# ice cream flavours
int T; // \# toppings
int K; // capacity of the parlour

machine* machine_info;
flavour* flavour_info;
topping* topping_info;
customer* customer_info;

itemOrder** allotedOrders;
int* orders_completed; // An array that would tell if all the order of a particular customer has been completed.

// Ok, let me use condition variables to check if an order is completed or not.

// I might not need the the entire cust_info, but since I have already made something like this and would be
// using a subset of it, I shall give this a go-ahead.
int canTheOrderBeCompleted(customer* cust_info){
	// Some redundant work here
	int* temp_quantities = (int*)malloc(sizeof(int) * T);
	for(int i = 0; i < T; i++){
		temp_quantities[i] = topping_info[i].quantity;
	}

	for(int i = 0; i < cust_info->num_ice_creams; i++){
		for(int j = 0; j < cust_info->ice_creams[i]->num_toppings; j++){
			temp_quantities[cust_info->ice_creams[i]->toppings[i]->index] --;
		}
	}

	for(int i = 0; i < T; i++){
		if(temp_quantities[i] < 0){
			free(temp_quantities);
			return 0;
		}
	}

	free(temp_quantities);
	return 1;
};

int find_time_to_make_from_flavour_name(String flavour_name){
	for(int i = 0; i < F; i++){
		if(!strcmp(flavour_info[i].name, flavour_name)){
			return flavour_info[i].prep_time;
		}
	}

	return 0;
}

int compareMachines(const void *a, const void *b) {
    return ((((machine*)a)->tm_stop - t) - ((machine*)b)->tm_stop - t);
}

int assignOrderToMachines(customer* cust_info){
	// To the machine which gets the order, do a sem_post

	// So how would I go about giving an order to a machine?
	// I'll use a machine algorithm

	int total_items = cust_info->num_ice_creams;

	int time_it_takes_for_each_item[total_items];
	for(int i = 0; i < total_items; i++){
		int time_to_prep = find_time_to_make_from_flavour_name(cust_info->ice_creams[i]->flavour_name);
		if(time_to_prep == 0){
			printf("Warning: time to prepare for %s was computed to be zero!\n", cust_info->ice_creams[i]->flavour_name);
		}
		time_it_takes_for_each_item[i] = time_to_prep;
	}

	// find the machine with the shortest time to live
	int min_ttl = INT_MAX;
	int min_machine_index;

	// create an array of machines
	machine* machines_to_sort;
	machines_to_sort = machine_info;

	qsort(machines_to_sort, N, sizeof(machine), compareMachines);

	int min_prep_time = INT_MAX;
	int prev_min = -1;
	int min_order_index;

	bool machineUsed[N]; // An array to track whether a machine has been used

	int S[total_items];

    for (int i = 0; i < N; i++) {
        machineUsed[i] = false;
    }

    for (int i = 0; i < total_items; i++) {
        int itemTime = time_it_takes_for_each_item[i];
        int machineIndex = -1; // Initialize machineIndex to -1

        for (int j = 0; j < N; j++) {
            if (!machineUsed[j] && (machines_to_sort[j].tm_stop - t) >= itemTime) {
                machineIndex = j; // Store the index of the first available machine with capacity >= itemTime
                machineUsed[j] = true; // Mark the machine as used
                break;
            }
        }

        S[i] = machineIndex; // Store the machine index in the result array
    }

	// i is the index of the corresponding ice_cream that the customer has in its ice_creams section.
	// S[i] is the corresponding machine index that will handle the request.

	// Now we have an S that stores in corresponding way, which machine is allotted to the item.

	printf("\t\tMapping of orders: ");
	for(int i = 0; i < total_items; i++){
		printf("%d ", S[i]);
	} printf("\n");

	int placing_successful = 0;

	//////// WAIT UNTIL MACHINE IS AVAILABLE, THEN POST THE ORDER /////////

	/////// LOCKS HERE /////////
	// This problem and its possible fix comes from the fact that a different thread might 
		for(int i = 0; i < total_items; i++){
			allotedOrders[S[i]]->customer_info = cust_info;
			allotedOrders[S[i]]->ice_cream_index = i;
			sem_post(&order_sems[S[i]]);
		}
	/////// LOCKS HERE /////////

	// Phew! Done!
};

void* customerThread(void* arg){
	// This is the customer thread
	customer* cust_info = (customer*)arg;

	printf("Customer %d arrives at %d second(s)\n", cust_info->c, cust_info->t_arr); // should it be t_arr or the current time. Both are likely to be the same.
	
	// Print their order (This should have locks too?)
	printf("Customer %d orders %d ice-cream(s).\n", cust_info->c, cust_info->num_ice_creams);
	for(int i = 0; i < cust_info->num_ice_creams; i++){
		printf("Ice cream %d: %s ", cust_info->c, cust_info->ice_creams[i]->flavour_name); // don't put \n here
		for(int j = 0; j < cust_info->ice_creams[i]->num_toppings; j++){
			printf("%s ", cust_info->ice_creams[i]->toppings[j]->name);
		}
	}
	// Now check if the order can be completed.
	// If no, then print this. This same print statement may be used elsewhere as where. Not localising it to just this place.
	int orderPossible = canTheOrderBeCompleted(cust_info);
	if(!orderPossible){
		printf("Customer %d left at %d second(s) with an unfulfilled order.\n", cust_info->c, t);
		return NULL; // If I do an exit(0) here then things may go haywire.
	}
	
	// If yes
	// Then assign machine(s) using some algorithm. Let's make that a blackbox.
	assignOrderToMachines(cust_info);

	// sem wait on all the orders (?) I mean I can keep an array that holds all the ...
		
	while(orders_completed[cust_info->c] != cust_info->num_ice_creams){
		// just wait
	}
	// int intial_time = t;
	// while(t - intial_time < 3){
	// 	// wait
	// }

	printf("Customer %d has collected their order(s) and left at %d second(s)\n", cust_info->c, t);
	return NULL;
}

// I did not think of doing so in reality but let's do it
// Making a machine thread
void* machineThread(void* arg){
	// Should the argument be an order? Or maybe just a pointer to a struct that holds the machine info as well.
	machine* machine_info = (machine*)arg;

	while(t - machine_info->tm_stop < 0){
		// wait for an order to come
		printf("\t\t\t\t\t\t Machine %d is waiting to get an order\n", machine_info->index);
		sem_wait(&order_sems[machine_info->index - 1]);
		if(t - machine_info->tm_stop >= 0){
			printf("Machine %d has stopped working at %d second(s)\n", machine_info->index, t);
			//// LOCKS HERE ////
			machines_closed += 1;
			//// LOCKS HERE ////
			pthread_exit(NULL);
		}

		isAvailable[machine_info->index - 1] = 0;

		// pick the order from the global array
		// printf("KKKK: %d\n", machine_info->index);
		itemOrder* order = allotedOrders[machine_info->index - 1];
		// if(order->customer_info == NULL){
		// 	exit(1);
		// }
		// printf("OLALALA\n");
		// printf("OOOLLLAAAALLAAA: %d\n", order->ice_cream_index);

		printf("Machine %d starts preparing ice cream %d of customer %d at %d second(s)", machine_info->index, order->ice_cream_index, order->customer_info->num_ice_creams, t);

		int start_time = t;
		while(t - start_time < find_time_to_make_from_flavour_name(order->customer_info->ice_creams[order->ice_cream_index]->flavour_name)){
			// Just Wait
		};
		printf("Machine %d completes preparing ice cream %d of customer %d at %d seconds(s)\n", machine_info->index, order->ice_cream_index, order->customer_info->c, t);
		// // Here check and decrement the quantities. And also send back the customer if the quantity falls. 
		// /////// LOCKS HERE /////////

		// /////// LOCKS HERE /////////

		isAvailable[machine_info->index] = 1;
	}
	printf("Machine %d has stopped working at %d second(s)\n", machine_info->index, t);
	return NULL;
}

int main(int argc, char** argv){
	#ifdef PRINT_SCAN 
	printf("Enter N, K, F, T: ");
	#endif
	scanf("%d %d %d %d", &N, &K, &F, &T);

	// Initialise the isAvailable array. This array stores if a particular machine is free.
	isAvailable = (int*)malloc(sizeof(int) * N);
	for(int i = 0; i < N; i++){
		isAvailable[i] = 0;
	}

	// Initialise the order_sems array
	order_sems = (sem_t*)malloc(sizeof(sem_t) * N); // order sems would be initialised with the total number of machines.
	for(int i = 0; i < N; i++){
		sem_init(&order_sems[i], 0, 0); // ek time par ek hi order place ho sakta hai machine mein
	}

	// Getting the machine information
	machine_info = (machine*)malloc(sizeof(machine) * N);
	for(int i = 0; i < N; i++){
		machine_info[i].index = i + 1;
		#ifdef PRINT_SCAN 
			printf("Enter machine %d's info: ", i + 1);
		#endif
		scanf("%d %d", &machine_info[i].tm_start, &machine_info[i].tm_stop);
	}

	// Getting the flavour information
	flavour_info = (flavour*)malloc(sizeof(flavour) * F);
	for(int i = 0; i < F; i++){
		flavour_info[i].name = malloc(sizeof(char) * NAME_LENGTH);

		flavour_info[i].index = i + 1;
		#ifdef PRINT_SCAN 
			printf("Enter flavour %d's info: ", i + 1);
		#endif
		
		scanf("%s %d", flavour_info[i].name, &flavour_info[i].prep_time);
	}

	// Getting the topping information
	topping_info = (topping*)malloc(sizeof(topping) * T);
	for(int i = 0; i < T; i++){
		topping_info[i].name = malloc(sizeof(char) * NAME_LENGTH);
		
		#ifdef PRINT_SCAN 
			printf("Enter topping %d's info: ", i + 1);
		#endif
		scanf("%s %d", topping_info[i].name, &topping_info[i].quantity);
	}

	// Getting the customer information (to improve)
	int C; // Number of customers. I'll change the any number of lines after that.
	#ifdef PRINT_SCAN 
		printf("Enter number of customers: "); 
	#endif
	scanf("%d", &C);

	// Initialising the orders completed array
	// The idea is to have locks around it and if the number corresponding to a particular customer index equals to the number of items
	// they ordered in their order, then the thread will exit, otherwise wait.
	// Caution: It can only be used index wise, not whole iteration.
	orders_completed = (int*)malloc(sizeof(int) * C);
	for(int i = 0; i < C; i++){
		orders_completed[i] = 0;
	}

	allotedOrders = (itemOrder**)malloc(sizeof(itemOrder*) * N);
	for(int i = 0; i < N; i++){
		allotedOrders[i] = (itemOrder*)malloc(sizeof(itemOrder) * N);
	}

	customer_info = (customer*)malloc(sizeof(customer) * C);
	
	for(int i = 0; i < C; i++){
		#ifdef PRINT_SCAN 
			printf("Enter customer %d's details: ID, t_arr, num_ice_creams: ", i + 1);
		#endif
		scanf("%d %d %d", &customer_info[i].c, &customer_info[i].t_arr, &customer_info[i].num_ice_creams);
		customer_info[i].ice_creams = (ice_cream**)malloc(sizeof(ice_cream*) * customer_info[i].num_ice_creams);
		for(int j = 0; j < customer_info[i].num_ice_creams; j++){
			customer_info[i].ice_creams[j] = (ice_cream*)malloc(sizeof(ice_cream));
			customer_info[i].ice_creams[j]->flavour_name = (String)malloc(sizeof(char) * NAME_LENGTH);
			
			customer_info[i].ice_creams[j]->toppings = (topping**)malloc(sizeof(topping*) * NUM_TOPPINGS);
			for(int k = 0; k < NUM_TOPPINGS; k++){
				customer_info[i].ice_creams[j]->toppings[k] = (topping*)malloc(sizeof(topping));
				customer_info[i].ice_creams[j]->toppings[k]->name = (String)malloc(sizeof(char) * NAME_LENGTH);
			}
		}
		// ! For now I am assuming that the number of toppings would be 2, as I want to get into solving it and not worrying about the input format.
		for(int j = 0; j < customer_info[i].num_ice_creams; j++){
			#ifdef PRINT_SCAN 
				printf("\t Enter flavour: ");
			#endif
			scanf("%s", customer_info[i].ice_creams[j]->flavour_name);
			customer_info[i].ice_creams[j]->num_toppings = NUM_TOPPINGS;

			#ifdef PRINT_SCAN 
				printf("\t Enter toppings: ");
			#endif
			scanf("%s %s", customer_info[i].ice_creams[j]->toppings[0]->name, customer_info[i].ice_creams[j]->toppings[1]->name);
		}
	}

	// print all the information gathered for testing
	printf("No. of ice cream machines: %d\n", N);
	printf("Capacity: %d\n", K);
	printf("No. of flavours: %d\n", F);
	printf("No. of toppings: %d\n", T);

	printf("Ice cream specs: tm_start tm_stop\n");
	for(int i = 0; i < N; i++){
		printf("Machine %d start: %d, stop: %d \n", machine_info[i].index, machine_info[i].tm_start, machine_info[i].tm_stop);
	}

	printf("\n");
	printf("Flavour information: \n");
	for(int i = 0; i < F; i++){
		printf("Flavour %d: %s, prep_time: %d\n", flavour_info[i].index, flavour_info[i].name, flavour_info[i].prep_time);
	}

	printf("\n");
	printf("Topping information: \n");
	for(int i = 0; i < T; i++){
		printf("Topping %d: %s, quantity: %d\n", topping_info[i].index, topping_info[i].name, topping_info[i].quantity);
	}

	printf("\n");
	printf("Customer information: \n");
	for(int i = 0; i < C; i++){
		printf("Customer %d: # ice-creams ordered: %d\n", customer_info[i].c, customer_info[i].num_ice_creams);
		for(int j = 0; j < customer_info[i].num_ice_creams; j++){
			printf("\t Ice-cream %s: ", customer_info[i].ice_creams[j]->flavour_name);
			for(int k = 0; k < customer_info[i].ice_creams[j]->num_toppings; k++){
				printf("%s ", customer_info[i].ice_creams[j]->toppings[k]->name);
			}
			printf("\n");
		}
	}	

	// exit(0);
	printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n");
	printf("Starting program in 2 seconds: \n");
	sleep(2);
	printf("Started: \n");
	printf("\n+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n\n");


	// This is the execution block
	while(1){
		// machine spawner
		for(int i = 0; i < N; i++){
			if(machine_info[i].tm_start == t){
				printf("Machine %d has started working at %d second(s)\n", machine_info[i].index, machine_info[i].tm_start);
				pthread_create(&machine_info[i].machine_thread_identifier, NULL, machineThread, &machine_info[i]);
				isAvailable[i] = 1;
			}
		}

		// customer spawner
		for(int i = 0; i < C; i++){
			if(customer_info[i].t_arr == t){
				// capacity checker
				if(current_number_of_customers >= K){
					printf("Parlour is full, go back you customer! यह कोई शादी नहीं है that you'll stand!! :P\n");
				}
				else{
					// Do pthread_create
					pthread_create(&customer_info[i].cust_thread_identifier, NULL, customerThread, &customer_info[i]);
					current_number_of_customers++;
				}
			}
		}

		// machine checker
		for(int i = 0; i < N; i++){
			if(machine_info[i].tm_stop == t){
				isAvailable[i] = 0;
				// give a sem_post and immediately exit
				sem_post(&order_sems[i]);
			}
		}

		// Parlour closed checker
		if(machines_closed >= N){ // for some reason I feel, greater can be used for more security
			printf("Parlour is closed for the day. Come back tomorrow! :)\n");
			exit(0);
		}

		sleep(1);
		t++; // be careful to not use continue in the main while loop.
		printf("\t\t\t\tCurrent time: %d\n", t);
	}

	return 0;
}
