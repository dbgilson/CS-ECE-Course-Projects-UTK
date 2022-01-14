//COSC 530: PA1, Memory Hierarchy Simulator
//By: David Gilson
//Header File for L2 component
#ifndef P3_L2_H
#define P3_L2_H
#include <vector>
#include <cmath>
#include <string>
#include <bitset>
#include <sstream>

//-1 is a place holder value until the entry is actually used.
typedef struct
{
    int tag = -1;
    int index = -1;
    bool valid = false;
    int reference_counter = 0;
    bool dirty = false;
    std::string addr = ""; 
    int PFN = -1;
    int VPN = -1;
} L2_Entry;

class L2
{
private:
    //For type: 0 - direct mapped, 1 - set associative
    int _type;
    int set_num; 
    int set_size; 
    int line_size; 
    bool wt_nwa;
    int bits_offset;
    int bits_index;
    std::vector<std::vector<L2_Entry>> cache;

public:
    L2(int sn, int ssz, int lsz, bool wn, int L2_offset_bit_num, int L2_index_bits_num);
    bool check_match(bool dirty, int tag, int index);
    std::vector<std::string> LRU_Evict(int index);
    std::vector<std::string> allocate(bool dirty, std::string addr, int tag, int index, int PFN, int VPN);
    std::vector<int> getAddrTagIndex(std::string addr);
    bool get_wt_nwa();
    void incRefCounter(int tag, int index);
    void invalidateByPFN(int PFN);
    int VPN_From_Tag_Index(int tag, int index);
    bool check_dirty(int tag, int index);
};

#endif
