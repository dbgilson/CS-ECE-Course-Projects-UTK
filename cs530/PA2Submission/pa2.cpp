//COSC 530: PA2 -- Speculative Dynamically Scheduled Pipeline Simulator
//By: David Gilson
//This program reads in a trace of RISC-V instructions to be executed and 
//determines for each instruction the exact cycles when it proceeds through
//each appropriate stage of the dynamically scheduled pipeline.
#include <iostream>
#include <queue>
#include <set>
#include <vector>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <bits/stdc++.h>

using namespace std;

//Enumerations for RISC V Operations
enum OPS {
    ADD, SUB, FADD, FSUB, FMUL, FDIV, LW, SW, FLW, FSW, BEQ, BNE
};

//Config info for pipeline
struct Pconfig{
  int effaddr;
  int fp_add_buff;
  int fp_mul_buff;
  int ints_buff;
  int reorder_buff;
  
  int fp_add_lat;
  int fp_sub_lat;
  int fp_mul_lat;
  int fp_div_lat;
};

struct Pconfig *preturn = (struct Pconfig*)malloc(sizeof (struct Pconfig));

int getnum(char *token){
	char tokenbuff[50];
	while(token != NULL){
		strcpy(tokenbuff, token);
		token = strtok(NULL, " ");
	}
	int x;
	x = atoi(tokenbuff);
	return x;
}

//Function to parse the config.txt file into the Pconfig
int parseconfig(){
	FILE *fp;
	char buff[255];
	char *token;
	double tmp;
	fp = fopen("config.txt", "r");

  printf("Configuration\n");
  printf("-------------\n");
  printf("buffers:\n");
  
	//Buffer Config
	fgets(buff, 255, (FILE*)fp);
	fgets(buff, 255, (FILE*)fp);
  fgets(buff, 255, (FILE*)fp);
	token = strtok(buff, " ");
	preturn->effaddr = getnum(token);
	printf("   eff addr: %d\n", preturn->effaddr);

	fgets(buff, 255, (FILE*)fp);
	token = strtok(buff, " ");
	preturn->fp_add_buff = getnum(token);
	printf("    fp adds: %d\n", preturn->fp_add_buff);

  fgets(buff, 255, (FILE*)fp);
	token = strtok(buff, " ");
	preturn->fp_mul_buff = getnum(token);
	printf("    fp muls: %d\n", preturn->fp_mul_buff);

  fgets(buff, 255, (FILE*)fp);
	token = strtok(buff, " ");
	preturn->ints_buff = getnum(token);
	printf("       ints: %d\n", preturn->ints_buff);
 
  fgets(buff, 255, (FILE*)fp);
	token = strtok(buff, " ");
	preturn->reorder_buff = getnum(token);
	printf("    reorder: %d\n\n", preturn->reorder_buff);
  
  //Latency Config
  
  printf("latencies:\n");
  
	fgets(buff, 255, (FILE*)fp);
	fgets(buff, 255, (FILE*)fp);
  fgets(buff, 255, (FILE*)fp);
  fgets(buff, 255, (FILE*)fp);
  token = strtok(buff, " ");
	preturn->fp_add_lat = getnum(token);
	printf("   fp add: %d\n", preturn->fp_add_lat);
 
  fgets(buff, 255, (FILE*)fp);
  token = strtok(buff, " ");
	preturn->fp_sub_lat = getnum(token);
	printf("   fp sub: %d\n", preturn->fp_sub_lat);
 
  fgets(buff, 255, (FILE*)fp);
  token = strtok(buff, " ");
	preturn->fp_mul_lat = getnum(token);
	printf("   fp mul: %d\n", preturn->fp_mul_lat);
 
  fgets(buff, 255, (FILE*)fp);
  token = strtok(buff, " ");
	preturn->fp_div_lat = getnum(token);
	printf("   fp div: %d\n\n\n", preturn->fp_div_lat);
 
  printf("                    Pipeline Simulation\n");
  printf("-----------------------------------------------------------\n");
  printf("                                      Memory Writes\n");
  printf("     Instruction      Issues Executes  Read  Result Commits\n");
  printf("--------------------- ------ -------- ------ ------ -------\n");

	fclose(fp);

	return 1;
}

//Counters for storing instruction cycles
vector<string> ori_ins;
vector<int> issue_counter(500);
vector<int> execute_st_counter(500);
vector<int> execute_counter(500);
vector<int> memory_counter(500);
vector<int> writeback_counter(500);
vector<int> commit_counter(500);
vector<int> store_counter(500);

const char * DELIM2 = ", \t()";

//Struct to hold instruction information
typedef struct _instruction {
    OPS op;
    int rd;
    int rs; 
    bool point_rob_rs;
    int rt; 
    bool point_rob_rt;
    int period;
    _instruction(OPS op, int rd, int rs, int rt)
            : op(op), rs(rs), rd(rd), rt(rt) {};
    _instruction() {};

} instruction;

//Struct to hold Reorder Buffer Element
typedef struct _ROB_element {
    instruction ins;
    OPS op;
    int rob_id;
    int value;
    bool done;
    bool write_back_done;
    bool committed;
    int period_left;
    bool require_memory;
	bool require_CDB;

    _ROB_element(instruction ins, OPS op, int rob_id)
            : ins(ins), op(op), rob_id(rob_id) {
        this->period_left = ins.period;
        this->done = false;
        this->write_back_done = false;
        this->committed = false;
        this->require_memory = false;
		this->require_CDB = true;
    };

} ROB_element;

//Register table element
typedef struct _RAT_element {
    bool point_rob;
    int refer_id;
    int value;

    _RAT_element(bool point_rob, int refer_id, int value)
            : point_rob(point_rob), refer_id(refer_id), value(value) {};
} RAT_element;

//FP register table element
typedef struct _RAT_f_element {
    bool point_rob;
    int refer_id;
    int value;

    _RAT_f_element(bool point_rob, int refer_id, int value)
            : point_rob(point_rob), refer_id(refer_id), value(value) {};
} RAT_f_element;


vector<ROB_element> ROB;
vector<RAT_element> RAT;
vector<RAT_f_element> RAT_f;
queue<instruction> inss;

//Global size vectors
int ROB_size = 0;
int ROB_in = 0;
int rs_ints_sz = 0;
int rs_fp_add_sz = 0;
int rs_fp_mul_sz = 0;
int rs_effaddr_sz = 0;

void issue();
void excute();
void memory();
void write_back();
void commit();

//Cycle counter
static int counter = 1;

//Global SW commit indicator
int sw_on_commit = 0;

void getinstruction(instruction ins1);
void print1();

int main(int argc, char** argv) {
	//Parse config and instructions
    parseconfig();
    instruction ins1;
    getinstruction(ins1);
	
	//Initialize register tables
    for (int i = 0; i < 32; i++) {
        RAT.push_back(RAT_element(false, 0, 0));
        RAT_f.push_back(RAT_f_element(false, 0, 0));
    }

    int size = inss.size();
    while (commit_counter[size - 1] == 0) {
        commit();
        write_back();
        memory();
        excute();
        issue();
        counter++;
		//If looping error, just end program.
        if(counter > 10000){
          return 0;
        }
    }
	//Print pipeline cycles
    print1();
}

//Function to simulate issue
void issue() {
    if (inss.size() <= 0)
        return;

	//Check reorder buff, then check reservation station, then issue instruction
    instruction ins = inss.front();
	if(ROB_size < preturn->reorder_buff){
    switch (ins.op) {
        case OPS::ADD:
        case OPS::SUB:
            if (rs_ints_sz < preturn->ints_buff) {
                if (RAT[ins.rs].point_rob == true) {
                    ins.point_rob_rs = true;
                    ins.rs = RAT[ins.rs].refer_id;
                } else {
                    ins.point_rob_rs = false;
                    ins.rs = RAT[ins.rs].value;
                }
                if (RAT[ins.rt].point_rob == true) {
                    ins.point_rob_rt = true;
                    ins.rt = RAT[ins.rt].refer_id;
                } else {
					ins.point_rob_rt = false;
                    ins.rt = RAT[ins.rt].value;
                }
                RAT[ins.rd].refer_id = ROB_in;
                RAT[ins.rd].point_rob = true;

                rs_ints_sz++;
                ROB.push_back(ROB_element(ins, ins.op, ROB_in));
				ROB_in++;
				ROB_size++;
                inss.pop();
                issue_counter[ROB.size() - 1] = counter;
            }
            break;
        case OPS::FADD:
        case OPS::FSUB:
            if (rs_fp_add_sz < preturn->fp_add_buff) {
                if (RAT_f[ins.rs].point_rob == true) {
                    ins.point_rob_rs = true;
                    ins.rs = RAT_f[ins.rs].refer_id;
                } else {
                    ins.point_rob_rs = false;
                    ins.rs = RAT_f[ins.rs].value;
                }
                if (RAT_f[ins.rt].point_rob == true) {
                    ins.point_rob_rt = true;
                    ins.rt = RAT_f[ins.rt].refer_id;
                } else {
					ins.point_rob_rt = false;
                    ins.rt = RAT_f[ins.rt].value;
                }
                RAT_f[ins.rd].refer_id = ROB_in;
                RAT_f[ins.rd].point_rob = true;

                rs_fp_add_sz++;
                ROB.push_back(ROB_element(ins, ins.op, ROB_in));
				ROB_in++;
				ROB_size++;
                inss.pop();
                issue_counter[ROB.size() - 1] = counter;
            }
            break;
        case OPS::FMUL:
        case OPS::FDIV:
            if (rs_fp_mul_sz < preturn->fp_mul_buff) {
                if (RAT_f[ins.rs].point_rob == true) {
                    ins.point_rob_rs = true;
                    ins.rs = RAT_f[ins.rs].refer_id;
                } else {
                    ins.point_rob_rs = false;
                    ins.rs = RAT_f[ins.rs].value;
                }
                if (RAT_f[ins.rt].point_rob == true) {
                    ins.point_rob_rt = true;
                    ins.rt = RAT_f[ins.rt].refer_id;
                } else {
					ins.point_rob_rt = false;
                    ins.rt = RAT_f[ins.rt].value;
                }
                RAT_f[ins.rd].refer_id = ROB_in;
                RAT_f[ins.rd].point_rob = true;

                rs_fp_mul_sz++;
                ROB.push_back(ROB_element(ins, ins.op, ROB_in));
				ROB_in++;
				ROB_size++;
                inss.pop();
                issue_counter[ROB.size() - 1] = counter;
            }
            break;
		case OPS::BNE:
        case OPS::BEQ:
			if (rs_effaddr_sz < preturn->effaddr) {
                if (RAT[ins.rs].point_rob == true) {
                    ins.point_rob_rs = true;
					ins.rs = RAT[ins.rs].refer_id;
                } else {
                    ins.point_rob_rs = false;
					ins.rs = RAT[ins.rs].value;
                }
                RAT[ins.rd].refer_id = ROB_in;
                RAT[ins.rd].point_rob = true;
                rs_effaddr_sz++;
                ROB.push_back(ROB_element(ins, ins.op, ROB_in));
				ROB_in++;
				ROB_size++;
				ROB[ROB.size() - 1].require_CDB = false;
				ROB[ROB.size() - 1].write_back_done = true;
                inss.pop();
                issue_counter[ROB.size() - 1] = counter;
            }
            break;
		case OPS::SW:
			if (rs_effaddr_sz < preturn->effaddr) {
                if (RAT[ins.rs].point_rob == true) {
                    ins.point_rob_rs = true;
					ins.rs = RAT[ins.rs].refer_id;
                } else {
                    ins.point_rob_rs = false;
					ins.rs = RAT[ins.rs].value;
                }
				if (RAT[ins.rt].point_rob == true) {
                    ins.point_rob_rt = true;
                    ins.rt = RAT[ins.rt].refer_id;
                } else {
					ins.point_rob_rt = false;
                    ins.rt = RAT[ins.rt].value;
                }
                RAT[ins.rd].refer_id = ROB_in;
                RAT[ins.rd].point_rob = true;
                rs_effaddr_sz++;
                ROB.push_back(ROB_element(ins, ins.op, ROB_in));
				ROB_in++;
				ROB_size++;
				ROB[ROB.size() - 1].require_CDB = false;
				ROB[ROB.size() - 1].write_back_done = true;
                inss.pop();
                issue_counter[ROB.size() - 1] = counter;
            }
            break;
		case OPS::LW:
			if (rs_effaddr_sz < preturn->effaddr) {
                if (RAT[ins.rs].point_rob == true) {
                    ins.point_rob_rs = true;
					ins.rs = RAT[ins.rs].refer_id;
                } else {
                    ins.point_rob_rs = false;
					ins.rs = RAT[ins.rs].value;
                }
				ins.point_rob_rt = false;
                RAT[ins.rd].refer_id = ROB_in;
                RAT[ins.rd].point_rob = true;
                rs_effaddr_sz++;
                ROB.push_back(ROB_element(ins, ins.op, ROB_in));
				ROB_in++;
				ROB_size++;
                ROB[ROB.size() - 1].require_memory = true;
                inss.pop();
                issue_counter[ROB.size() - 1] = counter;
            }
            break;
        case OPS::FSW:
			if (rs_effaddr_sz < preturn->effaddr) {
                if (RAT_f[ins.rs].point_rob == true) {
                    ins.point_rob_rs = true;
					ins.rs = RAT_f[ins.rs].refer_id;
                } else {
                    ins.point_rob_rs = false;
					ins.rs = RAT_f[ins.rs].value;
                }
				if (RAT_f[ins.rt].point_rob == true) {
                    ins.point_rob_rt = true;
                    ins.rt = RAT_f[ins.rt].refer_id;
                } else {
					ins.point_rob_rt = false;
                    ins.rt = RAT_f[ins.rt].value;
                }

                RAT_f[ins.rd].refer_id = ROB_in;
                RAT_f[ins.rd].point_rob = true;
                rs_effaddr_sz++;
                ROB.push_back(ROB_element(ins, ins.op, ROB_in));
				ROB_in++;
				ROB_size++;
                ROB[ROB.size() - 1].require_CDB = false;
				ROB[ROB.size() - 1].write_back_done = true;
                inss.pop();
                issue_counter[ROB.size() - 1] = counter;
            }
            break;
        case OPS::FLW:
            if (rs_effaddr_sz < preturn->effaddr) {
                if (RAT[ins.rs].point_rob == true) {
                    ins.point_rob_rs = true;
					ins.rs = RAT[ins.rs].refer_id;
                } else {
                    ins.point_rob_rs = false;
					ins.rs = RAT[ins.rs].value;
                }
				ins.point_rob_rt = false;
                RAT_f[ins.rd].refer_id = ROB_in;
                RAT_f[ins.rd].point_rob = true;
                rs_effaddr_sz++;
                ROB.push_back(ROB_element(ins, ins.op, ROB_in));
				ROB_in++;
				ROB_size++;
                ROB[ROB.size() - 1].require_memory = true;
                inss.pop();
                issue_counter[ROB.size() - 1] = counter;
            }
            break;
        default:
            printf("Op is wrong in issue\n");
    }
	}
}

//Function to simulate execute
void excute() {
	//For each instruction in Reorder buff, check if it needs executing
    for (auto i = ROB.begin(); i != ROB.end(); ++i) {
        if (i->done) continue;
		//Check if instruction RS points to ROB return
        if (i->ins.point_rob_rs == true) {
            if (ROB[i->ins.rs].write_back_done) {
                i->ins.point_rob_rs = false;
            }
            if (i->ins.point_rob_rt == true) { 
              if (ROB[i->ins.rt].write_back_done) {
                i->ins.point_rob_rt = false;
				if(ROB[i->ins.rt].committed == false)
					continue;
              }
            }

			if(ROB[i->ins.rs].committed == false)
				continue;
        }
		//Check if instruction RT points to ROB return
        if (i->ins.point_rob_rt == true) { 
            if (ROB[i->ins.rt].write_back_done) {
                i->ins.point_rob_rt = false;
            }
            if (i->ins.point_rob_rs == true) {
              if (ROB[i->ins.rs].write_back_done) {
                i->ins.point_rob_rs = false;
				continue;
				if(ROB[i->ins.rs].committed == false)
					continue;
              }
            }

            if(ROB[i->ins.rt].committed == false)
				continue;
        }
        if (i->ins.point_rob_rs == true || i->ins.point_rob_rt == true) {
            continue;
        } else {
            i->period_left--;
        }
        //Execute the command
        if (i->period_left <= 0) {
            i->done = true;
            switch (i->op) {
                case OPS::ADD:
					execute_st_counter[i->rob_id] = counter;
                    break;
                case OPS::SUB:
					execute_st_counter[i->rob_id] = counter;
                    break;
                case OPS::FADD:
					execute_st_counter[i->rob_id] = counter - preturn->fp_add_lat + 1;
                    break;
                case OPS::FSUB:
					execute_st_counter[i->rob_id] = counter - preturn->fp_sub_lat + 1;
                    break;
                case OPS::FMUL:
					execute_st_counter[i->rob_id] = counter - preturn->fp_mul_lat + 1;
                    break;
                case OPS::FDIV:
					execute_st_counter[i->rob_id] = counter - preturn->fp_div_lat + 1;
                    break;
                case OPS::LW:
                case OPS::FLW:
					execute_st_counter[i->rob_id] = counter;
                    break;
				case OPS::BNE:
				case OPS::BEQ:
				case OPS::SW:
				case OPS::FSW:
					rs_effaddr_sz--;
					execute_st_counter[i->rob_id] = counter;
                    break;
                default:
                    printf("Op is wrong in execute\n");
            }
            execute_counter[i->rob_id] = counter;
        }
    }
};

//Function to simulate memory
void memory() {
	//For each instruction in Reorder buff, check if it requires memory and is done executing
	//Also check if SW/FSW is using commit to write to memory
    for (auto i = ROB.begin(); i != ROB.end(); ++i) {
        if (i->done && i->require_memory && sw_on_commit == 0) {
            i->require_memory = false;
            memory_counter[i->rob_id] = counter;
            break;
        }
    }
};

//Function to simulate writeback
void write_back() {
	//For each instruction in Reorder buff, check if it requires writeback and is done executing
    for (auto i = ROB.begin(); i != ROB.end(); ++i) {
        if (i->done && i->write_back_done == false && i->require_memory == false && i->require_CDB == true) {
            i->write_back_done = true;
            if (i->op == ADD || i->op == SUB || i->op == LW || i-> op == SW)
                RAT[i->ins.rd].value = i->value;
            else
                RAT_f[i->ins.rd].value = i->value;
            writeback_counter[i->rob_id] = counter;
			switch (i->op) {
                case OPS::ADD:
                case OPS::SUB:
                    rs_ints_sz--;
                    break;
                case OPS::FADD:
                case OPS::FSUB:
                    rs_fp_add_sz--;
                    break;
                case OPS::FMUL:
                case OPS::FDIV:
                    rs_fp_mul_sz--;
                    break;
                case OPS::LW:
                case OPS::FLW:
                    rs_effaddr_sz--;
                    break;
				case OPS::SW:
				case OPS::FSW:
				case OPS::BNE:
				case OPS::BEQ:
					writeback_counter[i->rob_id] = 0;
					break;
                default:
                    printf("Op is wrong in write back\n");
			}
            break;
        }
    }
};

//Function to simulate commit
void commit() {
	//For each instruction in Reorder buff, check if it is done with write back (unless it is SW/FSW)
	sw_on_commit = 0;
    for (auto i = ROB.begin(); i != ROB.end(); ++i) {
        if (i->write_back_done && i->committed)
            continue;
        if (i->write_back_done && i->committed == false) {
			if(i->op == SW || i->op == FSW)
				sw_on_commit = 1;
            i->committed = true;
			ROB_size--;
            commit_counter[i->rob_id] = counter;
            break;
        } else
            break;
    }
};

//Function to get instruction from Stdin (cin)
void getinstruction(instruction ins1)
{
    char buf[30];
    string buf1;

    while (cin.getline(buf,65535)) {
        buf1=buf;
        ori_ins.push_back(buf1);
        char * word;
        vector<char *> words;
        string op;
        string token, label, dest, src, trgt;
        int i_src=0,i_trgt=0,i_dest=0,i_label=0;
        word = strtok(buf, DELIM2);//DELIM2=", \t()";

        if(word == NULL)continue;
        while (word) {
            words.push_back(word);
            word = strtok(NULL, DELIM2);
        }

        // Set the operator:
        op = words[0];
        // Parse the op:
		if(op.compare("bne")==0){
            dest = words[1];
            src =words[2];
            trgt = words[3];
            ins1.op=OPS::BNE;
            ins1.period = 1;
        }else if(op.compare("beq")==0) {
            dest = words[1];
            src = words[2];
            trgt = words[3];
            ins1.op=OPS::BEQ;
            ins1.period = 1;
        } 
        else if(op.compare("sw")==0){
            dest = words[1];
            src =words[1];
            trgt = words[3];
            ins1.op=OPS::SW;
            ins1.period = 1;
        }else if(op.compare("lw")==0) {
            dest = words[1];
            src = words[3];
            trgt = words[3];
            ins1.op=OPS::LW;
            ins1.period = 1;
        } 
        else if(op.compare("fsw")==0) {
            dest = words[1];
            src = words[1];
            trgt = words[3];
            ins1.op=OPS::FSW;
            ins1.period = 1;
        }
        else if(op.compare("flw")==0) {
            dest = words[1];
            src = words[3];
            trgt = words[3];
            ins1.op=OPS::FLW;
            ins1.period = 1;
        }else if (op.compare("add")==0) {
            dest = words[1];
            src = words[2];
            trgt = words[3];
            ins1.op=OPS::ADD;
            ins1.period = 1;
        } else if (op.compare("sub")==0) {
            dest = words[1];
            src = words[2];
            trgt = words[3];
            ins1.op=OPS::SUB;
            ins1.period = 1;
        } else if (op.compare("fsub.s")==0) {
            dest = words[1];
            src = words[2];
            trgt = words[3];
            ins1.op=OPS::FSUB;
            ins1.period = preturn->fp_sub_lat;
        } else if (op.compare("fadd.s")==0){
            dest = words[1];
            src = words[2];
            trgt = words[3];
            ins1.op=OPS::FADD;
            ins1.period = preturn->fp_add_lat;
        }else if (op.compare("fmul.s")==0){
            dest = words[1];
            src =words[2];
            trgt = words[3];
            ins1.op=OPS::FMUL;
            ins1.period = preturn->fp_mul_lat;
        }
        else if (op.compare("fdiv.s")==0){
            dest = words[1];
            src =words[2];
            trgt = words[3];
            ins1.op=OPS::FDIV;
            ins1.period = preturn->fp_div_lat;
        }
        else{
            printf("Instruction set contains invalid opcode\n");
            continue;
        }
        ins1.rd = atoi(&dest.c_str()[1]);
        ins1.rs = atoi(&src.c_str()[1]);
        ins1.rt = atoi(&trgt.c_str()[1]);
        inss.push(ins1);
    }
}

//Function to print out pipeline cycles for each instruction
void print1()
{
	for(int j=0;j< ori_ins.size();j++){
		printf("%-21s %6d %3d -%3d ", ori_ins[j].c_str(), issue_counter[j], execute_st_counter[j], execute_counter[j]);
		if(memory_counter[j]!=0) 
			printf("%6d ", memory_counter[j]);
		else
			printf("       ");
		if(writeback_counter[j]!=0) 
			printf("%6d %7d\n", writeback_counter[j], commit_counter[j]);
		else
			printf("       %7d\n", commit_counter[j]);
	}
}
