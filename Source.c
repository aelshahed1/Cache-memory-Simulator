/*
* Author: Ahmad Elshahed
* ID:10156370
* Date: 10 March 2020
*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

//Defining cache sizes for different modes
#define mode1_cache_size 256
#define mode2_cache_size 128
#define mode3_cache_size 64
#define mode4_cache_size 32
#define mode5_cache_size 16
#define mode6_cache_size 8
#define mode7_cache_size 4
#define mode8_cache_size 2

//function prototype
void cache_simulator(bool cross_correlation, int mode, int cache_size, int block_loop_counter, int NA_inc, FILE* fp_out, unsigned int cache[][3]);


int main() {        //start of main

	//Cache arrays for different modes, to be passed to function later
	unsigned int cache_mode1[mode1_cache_size][3]; ///cache array  //coloum 1 is address, coloum 2 is initialisation, coloum 3 is write or not
	unsigned int cache_mode2[mode2_cache_size][3]; ///cache array  //coloum 1 is address, coloum 2 is initialisation, coloum 3 is write or not
	unsigned int cache_mode3[mode3_cache_size][3]; ///cache array  //coloum 1 is address, coloum 2 is initialisation, coloum 3 is write or not
	unsigned int cache_mode4[mode4_cache_size][3]; ///cache array  //coloum 1 is address, coloum 2 is initialisation, coloum 3 is write or not
	unsigned int cache_mode5[mode5_cache_size][3]; ///cache array  //coloum 1 is address, coloum 2 is initialisation, coloum 3 is write or not
	unsigned int cache_mode6[mode6_cache_size][3]; ///cache array  //coloum 1 is address, coloum 2 is initialisation, coloum 3 is write or not
	unsigned int cache_mode7[mode7_cache_size][3]; ///cache array  //coloum 1 is address, coloum 2 is initialisation, coloum 3 is write or not
	unsigned int cache_mode8[mode8_cache_size][3]; ///cache array  //coloum 1 is address, coloum 2 is initialisation, coloum 3 is write or not

	FILE* fp_out;  // File pointer for output file
	fp_out = fopen("simulator_output.csv", "w"); //Assigning pointer to output file as a write file

	//Calling function for the 16 different operations required
	cache_simulator(true, 1, mode1_cache_size, 0, 1, fp_out, cache_mode1);
	cache_simulator(true, 2, mode2_cache_size, 2048, 2, fp_out, cache_mode2);
	cache_simulator(true, 3, mode3_cache_size, 1024, 4, fp_out, cache_mode3);
	cache_simulator(true, 4, mode4_cache_size, 512, 8, fp_out, cache_mode4);
	cache_simulator(true, 5, mode5_cache_size, 256, 16, fp_out, cache_mode5);
	cache_simulator(true, 6, mode6_cache_size, 128, 32, fp_out, cache_mode6);
	cache_simulator(true, 7, mode7_cache_size, 64, 64, fp_out, cache_mode7);
	cache_simulator(true, 8, mode8_cache_size, 32, 128, fp_out, cache_mode8);
	cache_simulator(false, 1, mode1_cache_size, 0, 1, fp_out, cache_mode1);
	cache_simulator(false, 2, mode2_cache_size, 2048, 2, fp_out, cache_mode2);
	cache_simulator(false, 3, mode3_cache_size, 1024, 4, fp_out, cache_mode3);
	cache_simulator(false, 4, mode4_cache_size, 512, 8, fp_out, cache_mode4);
	cache_simulator(false, 5, mode5_cache_size, 256, 16, fp_out, cache_mode5);
	cache_simulator(false, 6, mode6_cache_size, 128, 32, fp_out, cache_mode6);
	cache_simulator(false, 7, mode7_cache_size, 64, 64, fp_out, cache_mode7);
	cache_simulator(false, 8, mode8_cache_size, 32, 128, fp_out, cache_mode8);

	fclose(fp_out); //closing file


	return 0;
}      //End of main


void cache_simulator(bool cross_correlation, int mode, int cache_size, int block_loop_counter, int NA_inc, FILE* fp_out, unsigned int cache[][3]) {   //Start of function

	//Output Variables
	char cross_correlation_trace_file_name[] = "cross_correlation_trace_006";  //croos correlation file name
	char bubble_sort_trace_file_name[] = "bubble_sort_trace_006";      //bubble sort file name
	unsigned int mode_ID = mode; //The ID number of the cache configuration mode(1 … 8)
	unsigned int NRA = 0; //Total number of read accesses to the external memory
	unsigned int NWA = 0; //Total number of write accesses to the external memory
	unsigned int NCRH = 0; // Number of cache read hits
	unsigned int NCRM = 0; //Number of cache read misses
	unsigned int NCWH = 0; //Number of cache write hits
	unsigned int NCWM = 0; //Number of cache write misses

	int cache_index = 0;  //pointer in cache

	for (int i = 0; i < cache_size; i++) {  //Setting coloums 2 and 3 of cache array to 0
		cache[i][1] = 0;
		cache[i][2] = 0;
	}


	//File operations
	FILE* fp_in;  //creating file pointer for trace files

	if (cross_correlation == true) {   //cross_correlation trace file
		fp_in = fopen("cross_correlation_trace_006.trc", "r");
	}
	else {                            //Bubble sort trace file
		fp_in = fopen("bubble_sort_trace_006.trc", "r");
	}

	//Variables to store data from file
	char read_or_write[2];
	unsigned int  address;

	//Variables to control operation
	bool found = false;
	unsigned int block_address = 0;

	while (fscanf(fp_in, "%s" "%X", read_or_write, &address) == 2) {   //While reading data from file

		if (mode == 1) {   // If mode is 1, no need to go to the for loop to determine block number since every line contains 1 block
			block_address = address;
		}
		else {
			for (int i = 0; i < block_loop_counter; i++) {   // loop to determine block number
				if ((address >= (NA_inc * i)) && (address <= ((NA_inc * i) + (NA_inc - 1)))) {
					block_address = i;
				}
			}
		}

		if (cache_index > cache_size - 1) {   //Overflow cache pointer to operate as FIFO
			cache_index = 0;
		}

		if (read_or_write[0] == 'R') {  //Read Operation
			for (int i = 0; i < cache_size; i++) {    //check if address is in cache
				if (block_address == cache[i][0]) {
					found = true;
					break;
				}
				else {
					found = false;
				}
			}
			if (found) {   //Cache Read Hit
				NCRH++;   //Increment read hits counter
			}
			else {         //Cache Read Miss
				NRA += NA_inc;   //Read from main memory
				NCRM++;		//Increment read misses counter
				if (cache[cache_index][1] == 1) {      //check if line is initiliased
					if (cache[cache_index][2] == 1) {  //If line has been written to, write to main memory
						NWA += NA_inc;
					}
					cache[cache_index][0] = block_address;  //Store address in cache
					cache[cache_index++][2] = 0;  //store that nothing has been written to this memory
				}
				else {
					cache[cache_index][1] = 1;    // Initiliasing line
					cache[cache_index++][0] = block_address;  //Store address in cache
				}
			}
		}
		else {     //Write Operation
			for (int i = 0; i < cache_size; i++) {   //check if address is in cache
				if (block_address == cache[i][0]) {
					cache[i][2] = 1;   //store that this memory has been written to it
					found = true;
					break;
				}
				else {
					found = false;
				}
			}
			if (found) {  //Cache Write Hit
				NCWH++;    //Increment write hits counter
			}
			else {         //Cache Write Miss
				NRA += NA_inc;   //Read from main memory
				NCWM++;     //Increment write miss counter
				if (cache[cache_index][1] == 1) {      //check if line is initiliased
					if (cache[cache_index][2] == 1) {  //If line has been written to, write to main memory
						NWA += NA_inc;
					}
					cache[cache_index][0] = block_address; //Store address in cache
					cache[cache_index++][2] = 1;  //store that this memory has been written to it
				}
				else {
					cache[cache_index][1] = 1;    // Initiliasing line
					cache[cache_index][0] = block_address; //Store address in cache
					cache[cache_index++][2] = 1;  //store that this memory has been written to it
				}
			}
		}
	}

	//Store data in .csv file 
	if (cross_correlation == true) {
		fprintf(fp_out, " %s , %d , %d , %d , %d , %d , %d , %d \n", cross_correlation_trace_file_name, mode_ID, NRA, NWA, NCRH, NCRM, NCWH, NCWM);
	}
	else {
		fprintf(fp_out, " %s , %d , %d , %d , %d , %d , %d , %d \n", bubble_sort_trace_file_name, mode_ID, NRA, NWA, NCRH, NCRM, NCWH, NCWM);
	}

	//Close file
	fclose(fp_in);

}  //End of function
