//COSC 530: PA1, Memory Hierarchy Simulator
//By: David Gilson
//Header File for Page Table component
#ifndef P3_PAGETABLE_H
#define P3_PAGETABLE_H
#include <vector>
#include <string>
#include <sstream>
#include <bitset>

//-1 is a place holder value until the entry is actually used.
typedef struct
{
    bool valid = false;                
    int PFN = -1;                      
    int reference_counter = 0;         
    int dirty = false;                 
} PTE;

class PageTable
{
private:
    std::vector<PTE> PT;               
    int counter = 0;                   

public:
    PageTable(int virtual_page_num);
    bool assignToPhysFrame(int virtual_page_num, int VPN, int PFN, bool full);
    bool isValid(int VPN);
    std::string virtualToPhysical(std::string addr,int PFN, int offset_bits_num);
    void setPageDirty(int VPN);
    void setPageDirtyByPFN(int PFN);
    int getPFN(int VPN);
    void incRefCounter(int VPN);
    std::vector<std::string> isFull(int virtual_page_num, int physical_page_num);
};

#endif
