//COSC 530: PA1, Memory Hierarchy Simulator
//By: David Gilson
//File for creating DC functions
#include "DC.h"

//Config Setup for DC
DC::DC(int sn, int ssz, int lsz, bool wn, int DC_offset_bits_num, int DC_index_bits_num)
{
	//Set size and bit values
    set_num = sn;
    set_size = ssz;
    line_size = lsz;
    wt_nwa = wn;
    bits_offset = DC_offset_bits_num;
    bits_index = DC_index_bits_num;
    cache.resize(set_num); 

	//Set num of entries per set, and set type (direct or set associative)
    for(int i = 0; i < set_num; i++)
    {
        cache.at(i).resize(set_size);
    } 
    if(set_size == 1)
    {
        _type = 0;
    }
    else
    {
        _type = 1; 
    }
}

//Function to see if the cache has the specified entry.
//If so and dirty is set, then it will set the line dirty.
bool DC::check_match(bool dirty, int tag, int index)
{
    bool found = false;
	//Search set, check valid and tag, set dirty if necessary
    for(int i = 0; i < set_size; i++)
    {
        if(tag == cache.at(index).at(i).tag && cache.at(index).at(i).valid)
        { 
            found = true;

            if(dirty)
            {
                cache.at(index).at(i).dirty = true;
            }
            else
            {
                cache.at(index).at(i).dirty = false;
            }
            break;
        }
    }
    return found;
}

//Function to evict the specified line. Will return PFN associated with evicted line
int DC::evictLine(int tag, int index)
{
    int PFN = -1;
    for(int i = 0; i < set_size; i++)
    {
        if(tag == cache.at(index).at(i).tag && cache.at(index).at(i).valid)
        {
            cache.at(index).at(i).index = -1;
            cache.at(index).at(i).tag = -1;
            cache.at(index).at(i).valid = false;
            cache.at(index).at(i).reference_counter = 0;
            cache.at(index).at(i).addr = "";
            PFN = cache.at(index).at(i).PFN; 
            cache.at(index).at(i).PFN = -1;
            cache.at(index).at(i).VPN = -1;
            cache.at(index).at(i).dirty = false;
            break;
        }
    }
    return PFN;
}

//Function to evict LRU entry from given set. Will return vector of strings with details of operation. 
std::vector<std::string> DC::LRU_Evict(int index)
{
    std::vector<std::string> result;
	//How the result vector is mapped:
    //position 0 - what address is associated with the evicted line?
    //position 1 - was the line dirty?
    //position 2 - VPN associated with evicted line
    //position 3 - PFN associated with evicted line
    //position 4 - tag associated with evicted line
    //position 5 - index associated with evicted line
    //position 6 - new entry number (current_LRU_Entry below, or 0 for direct mapped)
	
    result.resize(7);

	//If direct mapped, map in values
    if(_type == 0) 
    {
        result.at(4) = std::to_string(cache.at(index).at(0).tag); 
        cache.at(index).at(0).tag = -1; 
        result.at(5) = std::to_string(cache.at(index).at(0).index); 
        cache.at(index).at(0).index = -1;
        cache.at(index).at(0).valid = false;
        cache.at(index).at(0).reference_counter = 0;
        result.at(0) = cache.at(index).at(0).addr; 
        cache.at(index).at(0).addr = ""; 
        result.at(2) = std::to_string(cache.at(index).at(0).VPN); 
        cache.at(index).at(0).VPN = -1; 
        result.at(3) = std::to_string(cache.at(index).at(0).PFN); 
        cache.at(index).at(0).PFN = -1; 
        if(cache.at(index).at(0).dirty) 
        {
            result.at(1) = "y";
        }
        else 
        {
            result.at(1) = "n";
        }
        cache.at(index).at(0).dirty = false;
        result.at(6) = "0"; 
    }
	//If set associative
    else if (_type == 1)
    {
        int current_LRU_Entry = -1; 
        int current_LRU_Counter = -1;
		//Find first valid entry
        for(int i = 0; i < set_size; i++)
        { 
            if(cache.at(index).at(i).valid)
            {
                current_LRU_Counter = cache.at(index).at(i).reference_counter;
                current_LRU_Entry = i;
                break;
            }
        }
        //Then search the set for the entry with the smallest reference counter
        for(int i = current_LRU_Entry + 1; i < set_size; i++)
        {
            if(cache.at(index).at(i).valid && cache.at(index).at(i).reference_counter < current_LRU_Counter)
            {
                current_LRU_Counter = cache.at(index).at(i).reference_counter;
                current_LRU_Entry = i;
            }
        }                                                                
		//Map Results accordingly
        result.at(4) = std::to_string(cache.at(index).at(current_LRU_Entry).tag);
        cache.at(index).at(current_LRU_Entry).tag = -1;
        result.at(5) = std::to_string(cache.at(index).at(current_LRU_Entry).index);
        cache.at(index).at(current_LRU_Entry).index = -1;
        cache.at(index).at(current_LRU_Entry).valid = false;
        cache.at(index).at(current_LRU_Entry).reference_counter = 0;
        result.at(0) = cache.at(index).at(current_LRU_Entry).addr;
        cache.at(index).at(current_LRU_Entry).addr = "";
        result.at(2) = std::to_string(cache.at(index).at(current_LRU_Entry).VPN);
        cache.at(index).at(current_LRU_Entry).VPN = -1;
        result.at(3) = std::to_string(cache.at(index).at(current_LRU_Entry).PFN);
        cache.at(index).at(current_LRU_Entry).PFN = -1;
        if(cache.at(index).at(current_LRU_Entry).dirty)
        {
            result.at(1) = "y";
        }
        else 
        {
            result.at(1) = "n";
        }
        cache.at(index).at(current_LRU_Entry).dirty = false;
        result.at(6) = std::to_string(current_LRU_Entry);
    }
    return result; 

}

//Function to allocate a line in the cache. Will return a vector of strings with details of operation. 
std::vector<std::string> DC::allocate(bool dirty, std::string addr, int tag, int index, int PFN, int VPN)
{
    std::vector<std::string> result;
	//How the result vector is mapped:
    //position 0 - did an eviction happen (y/n). If allocate calls evict method then y
    //position 1 - what address is associated with the evicted line? (if a line was evicted, if not -1)
    //position 2 - was the line dirty? -1 if did not evict
    //position 3 - VPN associated with evicted line, -1 if did not evict
    //position 4 - PFN associated with evicted line, -1 if did not evict
    //position 5 - tag associated with evicted line, -1 if did not evict
    //position 6 - index associated with evicted line, -1 if did not evict
    //position 7 - new entry number in set
	
    result.resize(8);

    bool foundFreeEntry = false;
	
	//Check for invalid entries
    for(int i = 0; i < set_size; i++)
    {
        if(!cache.at(index).at(i).valid)
        { 
            cache.at(index).at(i).index = index;
            cache.at(index).at(i).tag = tag;
            cache.at(index).at(i).valid = true;
            cache.at(index).at(i).PFN = PFN;
            cache.at(index).at(i).VPN = VPN;
            cache.at(index).at(i).reference_counter = 1;
            cache.at(index).at(i).addr = addr;
            if(dirty)
            {
                cache.at(index).at(i).dirty = true;
            }
            else
            {
                cache.at(index).at(i).dirty = false;
            }
			//Free up entry
            foundFreeEntry = true;
            result.at(0) = "n"; 
            result.at(1) = "-1";
            result.at(2) = "-1";
            result.at(3) = "-1";
            result.at(4) = "-1";
            result.at(5) = "-1";
            result.at(6) = "-1";
            result.at(7) = std::to_string(i);
            break; 
        }
    }
    if(!foundFreeEntry)
    {
        std::vector<std::string> evict_DC;
        evict_DC.resize(7);
        evict_DC = LRU_Evict(index);
		
		//How the evict_DC vector is mapped:
        //position 0 - what address is associated with the evicted line?
        //position 1 - was the line dirty?
        //position 2 - VPN associated with evicted line
        //position 3 - PFN associated with evicted line
        //position 4 - tag associated with evicted line
        //position 5 - index associated with evicted line
        //position 6 - new entry number in set

        cache.at(index).at(std::stoi(evict_DC.at(6))).index = index;            
        cache.at(index).at(std::stoi(evict_DC.at(6))).tag = tag;                
        cache.at(index).at(std::stoi(evict_DC.at(6))).valid = true;             
        cache.at(index).at(std::stoi(evict_DC.at(6))).PFN = PFN;                
        cache.at(index).at(std::stoi(evict_DC.at(6))).VPN = VPN;                
        cache.at(index).at(std::stoi(evict_DC.at(6))).reference_counter = 1;    
        cache.at(index).at(std::stoi(evict_DC.at(6))).addr = addr;              
        if(dirty)
        {
            cache.at(index).at(std::stoi(evict_DC.at(6))).dirty = true;         
        }
        else
        {
            cache.at(index).at(std::stoi(evict_DC.at(6))).dirty = false;
        }
		
        result.at(0) = "y";                                                       
        result.at(1) = evict_DC.at(0);                                            
        result.at(2) = evict_DC.at(1);                                            
        result.at(3) = evict_DC.at(2);                                            
        result.at(4) = evict_DC.at(3);                                            
        result.at(5) = evict_DC.at(4);                                            
        result.at(6) = evict_DC.at(5);                                            
        result.at(7) = evict_DC.at(6);                                            
    }
    return result;                                                                
}

//Function which returns a vector of ints given the address. Return Vector: Position 0 is the tag and position 1 is the index
std::vector<int> DC::getAddrTagIndex(std::string addr)
{
    std::vector<int> tag_index; 
    tag_index.resize(2); 

	//First, we find the index
    std::string addr_hex = "0x" + addr;    

    std::stringstream ss;
    ss << std::hex << addr_hex;    

    unsigned long address;    
    ss >> address;                       

    std::string mask = "";     

    for(int i = 0; i < bits_index; i++)
    {
        mask.append("1");                                              
    }                       

    for(int i = 0; i < bits_offset; i++)
    {
        mask.append("0");                                              
    }                                     
    
    std::bitset<32> original_bits(address);     
    std::bitset<32> index_mask_bits(mask);                             

    auto index_bits = (original_bits & index_mask_bits);   
    index_bits >>= bits_offset;                                   
                                   

    int index = (int)index_bits.to_ulong();  
    tag_index.at(1) = index;                                           

	//Next, we find the tag
    mask = "";      

    for(int i = 0; i < (32 - (bits_index + bits_offset)); i++)
    {
        mask.append("1");   
    }                                                                  

    for(int i = 0; i < (bits_index + bits_offset); i++)
    {
        mask.append("0");    
    }     
    
    std::bitset<32> tag_mask_bits(mask);    

    auto tag_bits = (original_bits & tag_mask_bits);  
    tag_bits >>= bits_index + bits_offset; 

    int tag = (int)tag_bits.to_ulong();  
    tag_index.at(0) = tag;                                             

    return tag_index;
}

//Function to get cache policy
bool DC::get_wt_nwa()
{
    return wt_nwa;
}

//Function that increments reference counter for a line
void DC::incRefCounter(int tag, int index)
{
    for(int i = 0; i < set_size; i++)
    {
        if(tag == cache.at(index).at(i).tag && cache.at(index).at(i).valid)            
        {
            cache.at(index).at(i).reference_counter++;                                 
            break;                                                                     
        }
    }
}

//Function that nvalidates cache entries associated with a specific PFN (used with main memory).
void DC::invalidateByPFN(int PFN)
{
    for(int i = 0; i < set_num; i++)
    {
        for(int j = 0; j < set_size; j++)
        {
            if(cache.at(i).at(j).PFN == PFN && cache.at(i).at(j).valid)
            {
                cache.at(i).at(j).index = -1; 
                cache.at(i).at(j).tag = -1;   
                cache.at(i).at(j).valid = false;
                cache.at(i).at(j).reference_counter = 0;
                cache.at(i).at(j).addr = "";
                cache.at(i).at(j).PFN = -1;
                cache.at(i).at(j).dirty = false;
            }
        }
    }
}

//Function to check if the line specified by the tag and index is dirty
bool DC::check_dirty(int tag, int index)
{
    bool dirty = false;
    for(int i = 0; i < set_size; i++)
    {
        if(cache.at(index).at(i).valid && cache.at(index).at(i).tag == tag)
        {   
            if(cache.at(index).at(i).dirty)
            {
                dirty = true;
            }
            break;                                                                     
        }
    }
    return dirty;
}
