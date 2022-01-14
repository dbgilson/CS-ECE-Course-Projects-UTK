//COSC 530: PA1, Memory Hierarchy Simulator
//By: David Gilson
//Header File for DTLB component
#ifndef P3_DTLB_H
#define P3_DTLB_H
#include <vector>
#include <string>
#include <bitset>
#include <sstream>

//-1 is a place holder value until the entry is actually used.
typedef struct
{
    int tag = -1;                      
    int index = -1;                    
    int VPN;                           
    int PFN;                           
    bool valid = false;                
    int reference_counter = 0;         
    std::string addr = "";             
} DTLB_Entry;

class DTLB
{
private:
    //0 - direct mapped, 1 - set associative
    int _type;
    int set_num;                                          
    int set_size;                                         
    int bits_index;                                   
    int bits_offset;                                  
    std::vector<std::vector<DTLB_Entry>> cache;                                                    

public:
    DTLB(int sn, int ssz,int dtlb_index_bits_num, int offset_bits_num);
    int check_match(int tag, int index);
    int LRU_Evict(int index);
    int allocate(std::string addr, int tag, int index, int VPN, int PFN);
    std::vector<int> getAddrTagIndex(std::string addr);
    void incRefCounter(int tag, int index);
    int retrievePFN(int tag, int index);
    void invalidateByPFN(int PFN);
};

#endif
