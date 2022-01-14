//COSC 530: PA1, Memory Hierarchy Simulator
//By: David Gilson
//File for creating Page Table functions
#include "PT.h"

//Config Setup to set PT size
PageTable::PageTable(int virtual_page_num)
{
    PT.resize(virtual_page_num); 
} 

//Function to assign a virtual page into a physical frame. It will evict a page if needed
bool PageTable::assignToPhysFrame(int virtual_page_num, int VPN, int PFN, bool full)
{
    bool dirty = false;
	//If there needs to be an eviction, check LRU and evict page
    if(full)
    { 
        for(int i = 0; i < virtual_page_num; i++)
        {
            if(PT.at(i).PFN == PFN && PT.at(i).valid)
            {
                PT.at(i).valid = false;
                PT.at(i).PFN = -1;
                PT.at(i).reference_counter = 0; 
                if(PT.at(i).dirty)
                {
                    dirty = true;
                }
                PT.at(i).dirty = false;
                break; 
            }
        }
    }
	
	//Assign PTE
    PT.at(VPN).valid = true; 
    PT.at(VPN).PFN = PFN; 
    PT.at(VPN).reference_counter = 1; 
    PT.at(VPN).dirty = false;

    return dirty;
}

//Function that will return a vector of strings that tell if the physical frames have 
//all been filled (and thus a page eviction will need to happen) as well as the PFN that 
//the virtual page will be put into. 
std::vector<std::string> PageTable::isFull(int virtual_page_num, int physical_page_num)
{
    std::vector<std::string> result;
    result.resize(2);
	//How the result vector is mapped:
    //0 - are all physical frames filled? 0 - no; 1 - yes
    //1 - physical frame for virtual page to go into

    result.at(0) = "0";  

    bool full = false;
	
	//Check if physical pages are full
    if(counter == physical_page_num)
    {
        full = true;
        result.at(0) = "1";
        int currentLeastRefEntry = -1; 
        int currentLeastRefCounter = -1;
		//Get first valid entry
        for(int i = 0; i < virtual_page_num; i++)
        {
            if(PT.at(i).valid)
            {
                currentLeastRefCounter = PT.at(i).reference_counter;
                currentLeastRefEntry = i;
                counter = PT.at(i).PFN;
                break;
            }
        }
        for(int i = currentLeastRefEntry + 1; i < virtual_page_num; i++)
        {
            if(PT.at(i).valid && PT.at(i).reference_counter < currentLeastRefCounter)
            { 
                currentLeastRefCounter = PT.at(i).reference_counter;
                counter = PT.at(i).PFN;
            }
        }
    }
	//Update frame number result
    result.at(1) = std::to_string(counter);
    if(full) 
    {
        counter = physical_page_num; 
    }
    else
    {
        counter++;
    }
    return result;
}

//Function to check if a page is valid
bool PageTable::isValid(int VPN)
{
    bool valid = false;
    if(PT.at(VPN).valid)
    {
        valid = true;
    }
    return valid;
}

//Function that translates a virtual address to a physical address given the PFN and 
//number of bits used for the page offset. 
std::string PageTable::virtualToPhysical(std::string addr, int PFN, int offset_bits_num)
{
    std::string addr_hex = "0x" + addr;

    std::stringstream ss;
    ss << std::hex << addr_hex; 

    unsigned long address;
    ss >> address;

    std::string mask = "";

    for(int i = 0; i < offset_bits_num; i++)
    {
        mask.append("1");
    } 

    std::bitset<32> original_bits(address);
    std::bitset<32> offset_mask_bits(mask); 

    auto offset_bits = (original_bits & offset_mask_bits); 

    std::bitset<32> PFN_Bits(PFN);
    std::string PFN_Bits_Str = PFN_Bits.to_string();

    //Had to look up how to remove leading zeros:
    //https://stackoverflow.com/questions/25726531/how-to-remove-leading-zeros-from-string-using-c
    PFN_Bits_Str.erase(0, std::min(PFN_Bits_Str.find_first_not_of('0'), PFN_Bits_Str.size() - 1));

    std::string PFN_Mask = "";
    PFN_Mask.append(PFN_Bits_Str); 
    for(int i = 0; i < offset_bits_num; i++)
    { 
        PFN_Mask.append("0");
    }

    std::bitset<32> PFN_BitMask(PFN_Mask); 

    auto physAddrBits = (PFN_BitMask | offset_bits);

    unsigned long physAddr = physAddrBits.to_ulong();
    std::stringstream ssPhys;
    ssPhys << std::hex << physAddr;

    return ssPhys.str();
}

//Function to get physical frame number from PT entry
int PageTable::getPFN(int VPN)
{
    if(PT.at(VPN).valid)
    {
        return PT.at(VPN).PFN;
    }
    else
    {
        return -1;
    }
}

//Function to set a page's dirty bit
void PageTable::setPageDirty(int VPN)
{
    if(PT.at(VPN).valid)
    {
        PT.at(VPN).dirty = true; 
    }
}

//Function to increment reference counter for a page
void PageTable::incRefCounter(int VPN)
{
    if(PT.at(VPN).valid)
    {
        PT.at(VPN).reference_counter++;
    }
}

//Function to set a page's dirty bit given the physical frame
void PageTable::setPageDirtyByPFN(int PFN)
{
    for(int i = 0; i < PT.size(); i++)
    {
        if(PT.at(i).valid && PT.at(i).PFN == PFN)
        {
            PT.at(i).dirty = true; 
            break;
        }
    }
}
