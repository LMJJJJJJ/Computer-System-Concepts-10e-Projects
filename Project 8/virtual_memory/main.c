/* //////////////////////////////////////////////////////////
//
// TLB-miss -> FIFO
// Page Replacement -> None
//
////////////////////////////////////////////////////////// */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define MAX_FILENAME 20
#define MAX_LINE 10
#define VM_SIZE 65536
#define MEM_SIZE 65536
#define N_TLB_ENTRY 16
#define N_PAGE_TABLE_ENTRY 256
#define PAGE_FRAME_SIZE 256

int TLB[N_TLB_ENTRY][2];
int PAGE_TABLE[N_PAGE_TABLE_ENTRY]; // page number -> frame number
char MEMORY[MEM_SIZE];
char *LOGIC_MEMORY;
int next_frame_ptr = 0; // next beginning position

int tlb_item_count = 0; // # valid entries in TLB
int next_tlb_victim = 0;

int page_fault_count = 0;
int tlb_hit_count = 0;
int instruction_count = 0;

char line[10]; // read lines from the file

char in_filename[MAX_FILENAME];
char out_filename[] = "fifo_none.txt";
char store_filename[] = "BACKING_STORE.bin";

int get_page_number(int address);
int get_offset(int address);
void initialize();
int consult_tlb(int page_number);
void update_tlb(int page_number, int frame_number);
int consult_page_table(int page_number);
void load_to_memory(int page_number);



int main(int argc, char *argv[]){
	if (argc != 2) {
		printf("Please enter 1 argument.\n");
		return 1;
	}
	strcpy(in_filename, argv[1]);
	/* Initialize page table, TLB, logical memory and some variables*/
	initialize();
	int page_number, frame_number, offset, virtual_address, physical_address;
	
	/* prepare the I/O files */
	FILE *in_file, *out_file;
	in_file = fopen(in_filename, "r");
	if (in_file == NULL){
		printf("Cannot load the input file.\n");
		return 1;
	}
	out_file = fopen (out_filename, "w");
	if (out_file == NULL){
		printf("Cannot create the output file.\n");
		return 1;
	}

	/* Start working! */
	while (fgets(line, MAX_LINE, in_file)){
		instruction_count += 1;
		virtual_address = atoi(line);
		page_number = get_page_number(virtual_address);
		offset = get_offset(virtual_address);
		frame_number = consult_tlb(page_number);
		if (frame_number == -1){
			// TLB miss
			frame_number = consult_page_table(page_number);
		    if (frame_number == -1){
			    // page fault
			    load_to_memory(page_number);
			    frame_number = consult_page_table(page_number);
		    }
		    update_tlb(page_number, frame_number);
		}
		physical_address = frame_number * PAGE_FRAME_SIZE + offset;
		fprintf(out_file, "Virtual address: %d Physical address: %d Value: %d\n",
			               virtual_address, physical_address, MEMORY[physical_address]);
	}
	fprintf(out_file, "----------------------------------------\n");
	fprintf(out_file, "Page Fault Number: %d\n", page_fault_count);
	fprintf(out_file, "TLB Hit Number: %d\n", tlb_hit_count);
	fprintf(out_file, "Page Fault Rate: %.3f\n", page_fault_count / (float) instruction_count);
	fprintf(out_file, "TLB Hit Rate: %.3f\n", tlb_hit_count / (float) instruction_count);
	fclose(in_file);
    fclose(out_file);
	return 0;
}

/* address is 16-bit */
int get_page_number(int address){
	return address >> 8;
}

/* address is 16-bit */
int get_offset(int address){
	return address & 255;
}

void initialize(){
	int store_file = open(store_filename, O_RDONLY);
	LOGIC_MEMORY = mmap(0, 65536, PROT_READ, MAP_SHARED, store_file, 0);
	close(store_file);
	for (int i = 0; i < N_TLB_ENTRY; ++i){
		TLB[i][0] = -1;
		TLB[i][1] = -1;
	}
	for (int i = 0; i < N_PAGE_TABLE_ENTRY; ++i){
		PAGE_TABLE[i] = -1;
	}
	next_frame_ptr = 0;
}

int consult_page_table(int page_number){
	if (PAGE_TABLE[page_number] == -1){
		page_fault_count += 1;
	}
	return PAGE_TABLE[page_number];
}

void load_to_memory(int page_number){
	PAGE_TABLE[page_number] = next_frame_ptr;
	memcpy(MEMORY + next_frame_ptr * PAGE_FRAME_SIZE, LOGIC_MEMORY + page_number * PAGE_FRAME_SIZE, PAGE_FRAME_SIZE);
	next_frame_ptr += 1;
}

int consult_tlb(int page_number){
	for (int i = 0; i < N_TLB_ENTRY; ++i){
		if (TLB[i][0] == page_number){
			tlb_hit_count += 1;
			return TLB[i][1];
		}
	}
	return -1;
}

void update_tlb(int page_number, int frame_number){
	if (tlb_item_count < N_TLB_ENTRY){
		TLB[tlb_item_count][0] = page_number;
	    TLB[tlb_item_count][1] = frame_number;
		tlb_item_count += 1;
	}
	else {
		TLB[next_tlb_victim][0] = page_number;
	    TLB[next_tlb_victim][1] = frame_number;
		next_tlb_victim = (next_tlb_victim + 1) % N_TLB_ENTRY;
	}
}