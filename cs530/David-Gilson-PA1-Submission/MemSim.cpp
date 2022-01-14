//COSC 530: PA1, Memory Hierarchy Simulator
//By: David Gilson
//Main Simulation File
#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <cstdlib>
#include <bitset>
#include "DC.h"
#include "L2.h"
#include "PT.h"
#include "DTLB.h"

//This struct holds the config info given from trace.config
typedef struct
{
	int dtlb_set_num;
	int dtlb_set_size;
	int dtlb_index_bits_num;

	int virtual_page_num;
	int virtual_page_bits_num;
	int physical_page_num;
	int physical_page_bits_num;
	int page_size_bytes;
	int page_table_index_bits_num;
	int page_offset_bits_num;

	int DC_set_num;
	int DC_set_size;
	int DC_line_size_bytes;
	bool DC_wt_nwa;
	int DC_offset_bits_num;
	int DC_index_bits_num;

	int L2_set_num; 
	int L2_set_size;
	int L2_line_size_bytes;
	bool L2_wt_nwa;
	int L2_offset_bit_num;
	int L2_index_bits_num;

	bool virtual_addresses;
	bool dtlb_enabled;
	bool L2_enabled;

} Config;

//This cache record information is used when a page eviction is needed and we need to deal 
//with the lines currently in the caches associated with that page.
typedef struct
{
	int tag = 0;
	int index = 0;
	std::string addr = "";

} CacheRecord;

//Simulator Function declarations
Config setup();
void showOptions(Config options);
bool isPowerOfTwo(int n);
std::string cleanAddr(std::string original, Config conf);
int getPhysicalPage(std::string addr, Config conf);
void showLine(Config conf, std::string addr, int VPN, int pageOffset, int dtlbTag, int dtlbIndex, std::string dtlbRes,
		std::string ptRes, int PFN, int DCTag, int DCIndex, std::string DCRes, int L2Tag, int L2Index, std::string L2Res);
void simulate_w_L2(Config options, std::vector<std::vector<CacheRecord>>& DC_rec, std::vector<std::vector<CacheRecord>>& L2_rec);
void simulate_no_L2(Config options, std::vector<std::vector<CacheRecord>>& DC_rec);
int getPageOffset(std::string addr, Config options);
int getVirtualPage(std::string addr, Config options);

void invalidate_WB_WB(DC& DC_Cache, L2& L2_Cache, std::vector<std::vector<CacheRecord>>& DC_rec, std::vector<std::vector<CacheRecord>>& L2_rec, PageTable& PT, int PFN, float& main_mem_refs);
void invalidate_WT_WT(DC& DC_Cache, L2& L2_Cache, std::vector<std::vector<CacheRecord>>& DC_rec, std::vector<std::vector<CacheRecord>>& L2_rec, PageTable& PT, int PFN, float& main_mem_refs);
void invalidate_WB_WT(DC& DC_Cache, L2& L2_Cache, std::vector<std::vector<CacheRecord>>& DC_rec, std::vector<std::vector<CacheRecord>>& L2_rec, PageTable& PT, int PFN, float& main_mem_refs);
void invalidate_WT_WB(DC& DC_Cache, L2& L2_Cache, std::vector<std::vector<CacheRecord>>& DC_rec, std::vector<std::vector<CacheRecord>>& L2_rec, PageTable& PT, int PFN, float& main_mem_refs);
void invalidate_WB(DC& DC_Cache, std::vector<std::vector<CacheRecord>>& DC_rec, PageTable& PT, int PFN, float& main_mem_refs);
void invalidate_WT(DC& DC_Cache, std::vector<std::vector<CacheRecord>>& DC_rec, PageTable& PT, int PFN, float& main_mem_refs);

int main()
{
	//Function to get options from trace.config
	Config options = setup();                                                

	//Function to show data given from trace.config
	showOptions(options);

	std::string line1 = "Virtual  Virt.  Page TLB    TLB TLB  PT   Phys        DC  DC          L2  L2";
	std::string line2 = "Address  Page # Off  Tag    Ind Res. Res. Pg # DC Tag Ind Res. L2 Tag Ind Res.";
	std::string line3 = "-------- ------ ---- ------ --- ---- ---- ---- ------ --- ---- ------ --- ----";

	std::cout << line1 << std::endl;
	std::cout << line2 << std::endl;
	std::cout << line3 << std::endl;

	//Record which cache lines for DC are associated with each PFN.
	std::vector<std::vector<CacheRecord>> DC_rec;

	//resize to number of physical pages
	DC_rec.resize(options.physical_page_num);

	//Start out 0 entries
	for(int i = 0; i < options.physical_page_num; i++)
	{
		DC_rec.at(i).resize(0);
	}

	//Repeat for L2
	std::vector<std::vector<CacheRecord>> L2_rec;
	L2_rec.resize(options.physical_page_num);
	for(int i = 0; i < options.physical_page_num; i++)
	{
		L2_rec.at(i).resize(0);
	}

	//Choosing which simulation to run based on the given config values.
	if(!options.virtual_addresses)
	{
		if(options.L2_enabled)
		{
			simulate_w_L2(options, DC_rec, L2_rec); 
		}
		else if(!options.L2_enabled) 
		{
			simulate_no_L2(options, DC_rec);
		}
	}
	else if(options.virtual_addresses)
	{
		if(options.L2_enabled) 
		{
			simulate_w_L2(options, DC_rec, L2_rec);
		}
		else if(!options.L2_enabled)
		{
			simulate_no_L2(options, DC_rec); 
		}
	}

	return 0;
}

//sets up the simulation based on the trace.config file
Config setup()
{
	//Create config structure and open up trace.config
	Config conf;
	std::ifstream fd("trace.config");

	//Buffer Variables
	std::string line;
	std::string substring;
	int substring_index;
	std::stringstream ss;

	//Go through and check each line to get needed data.
	while(std::getline(fd, line))
	{
		if(!line.empty())
		{
			if(line.find("Data TLB configuration") == 0)
			{
				for(int i = 0; i < 2; i++)
				{
					std::getline(fd, line);
					if(line.find("Number of sets") == 0)
					{
						substring_index = line.find(':');
						substring = line.substr(substring_index + 2); 
						ss << substring;  
						ss >> conf.dtlb_set_num;
						substring.clear();       
						ss.clear();    
					}
					else if(line.find("Set size") == 0)  
					{
						substring_index = line.find(':');   
						substring = line.substr(substring_index + 2); 
						ss << substring;   
						ss >> conf.dtlb_set_size; 
						substring.clear(); 
						ss.clear();  
					}
				}
				//Config Error checking for DTLB
				if((conf.dtlb_set_num * conf.dtlb_set_size) > 64) 
				{
					std::cout << "DTLB can have a maximum of 64 entries!" << std::endl;
					exit(1);
				}
				if(!isPowerOfTwo(conf.dtlb_set_num))  
				{
					//if not fully associative
					if(conf.dtlb_set_num != 1)  
					{
						std::cout << "DTLB set quantity must be 1 or a power of 2!" << std::endl;
						exit(1);
					}
				}
				if(!isPowerOfTwo(conf.dtlb_set_size))   
				{
					//if not direct mapped
					if(conf.dtlb_set_size != 1)                               
					{                                                      
						std::cout << "DTLB set size must be a power of 2!" << std::endl;
						exit(1);
					}
				}
				if(!isPowerOfTwo(conf.dtlb_set_num * conf.dtlb_set_size)) 
				{
					std::cout << "Number of entries in DTLB must be a power of 2!" << std::endl;
					exit(1);
				}
			}
			else if(line.find("Page Table configuration") == 0)   
			{
				for(int i = 0; i < 3; i++)
				{
					//Get next lines
					std::getline(fd, line);
					if(line.find("Number of virtual pages") == 0)
					{
						substring_index = line.find(':');   
						substring = line.substr(substring_index + 2); 
						ss << substring; 
						ss >> conf.virtual_page_num; 
						substring.clear(); 
						ss.clear(); 
					}
					else if(line.find("Number of physical pages") == 0) 
					{
						substring_index = line.find(':');   
						substring = line.substr(substring_index + 2);   
						ss << substring;  
						ss >> conf.physical_page_num;    
						substring.clear();         
						ss.clear();   
					}
					else if(line.find("Page size") == 0)     
					{
						substring_index = line.find(':');  
						substring = line.substr(substring_index + 2); 
						ss << substring; 
						ss >> conf.page_size_bytes; 
						substring.clear();     
						ss.clear();  
					}
				}
				//Config Error Checking for Page Table data
				if(conf.virtual_page_num > 8192)  
				{
					std::cout << "The maximum number of virtual pages is 8192!" << std::endl;
					exit(1);
				}
				if(conf.physical_page_num > 2048)   
				{
					std::cout << "The maximum number of physical pages is 2048!" << std::endl;
					exit(1);
				}
				if(!isPowerOfTwo(conf.virtual_page_num))  
				{
					std::cout << "Virtual page quantity must be a power of 2!" << std::endl;
					exit(1);
				}
				if(!isPowerOfTwo(conf.physical_page_num))  
				{
					std::cout << "Physical page quantity must be a power of 2!" << std::endl;
					exit(1);
				}
				if(!isPowerOfTwo(conf.page_size_bytes)) 
				{
					std::cout << "Page size must be a power of 2!" << std::endl;
					exit(1);
				}
			}
			else if(line.find("Data Cache configuration") == 0)   
			{
				for(int i = 0; i < 4; i++)  
				{
					std::getline(fd, line); 
					if(line.find("Number of sets") == 0) 
					{
						substring_index = line.find(':'); 
						substring = line.substr(substring_index + 2);
						ss << substring; 
						ss >> conf.DC_set_num; 
						substring.clear(); 
						ss.clear();  
					}
					else if(line.find("Set size") == 0)
					{
						substring_index = line.find(':'); 
						substring = line.substr(substring_index + 2);
						ss << substring;
						ss >> conf.DC_set_size; 
						substring.clear();  
						ss.clear();      
					}
					else if(line.find("Line size") == 0) 
					{
						substring_index = line.find(':');  
						substring = line.substr(substring_index + 2);
						ss << substring;
						ss >> conf.DC_line_size_bytes; 
						substring.clear();
						ss.clear();   
					}
					else if(line.find("Write through/no write allocate") == 0)
					{                        
						substring_index = line.find(':'); 
						substring = line.substr(substring_index + 2);
						if(substring == "n" or substring == "no")
						{
							conf.DC_wt_nwa = false; 
						}
						else if(substring == "y" or substring == "yes")
						{
							conf.DC_wt_nwa = true;
						}
						else
						{
							std::cout << "Invalid configuration option" << std::endl;
							exit(1);
						}
						substring.clear();
					}
				}
				//Error Checking DC Config Data
				if(conf.DC_set_size > 8) 
				{
					std::cout << "Data Cache (DC) can have a maximum set associativity of 8!" << std::endl;
					exit(1);
				}
				if((conf.DC_set_num * conf.DC_set_size) > 128)              
				{
					std::cout << "Data Cache (DC) can have a maximum of 128 entries!" << std::endl;
					exit(1);
				}
				if(conf.DC_line_size_bytes < 8) 
				{
					std::cout << "Data Cache (DC) has a minimum line size of 8 bytes!" << std::endl;
					exit(1);
				}
				if(!isPowerOfTwo(conf.DC_set_num))  
				{
					std::cout << "Data Cache (DC) set quantity must be a power of 2!" << std::endl;
					exit(1);
				}
				if(!isPowerOfTwo(conf.DC_set_size))
				{
					//if not direct mapped
					if(conf.DC_set_size != 1) 
					{                               
						std::cout << "Data Cache (DC) set size must be 1 for direct mapped or a power of 2"
							" for set associative!" << std::endl;
						exit(1);
					}
				}
				if(!isPowerOfTwo(conf.DC_line_size_bytes))   
				{
					std::cout << "Data Cache (DC) line size must be a power of 2!" << std::endl;
					exit(1);
				}
				if(!isPowerOfTwo(conf.DC_set_num * conf.DC_set_size))  
				{
					std::cout << "Number of entries in Data Cache (DC) must be a power of 2!" << std::endl;
					exit(1);
				}
			}
			else if(line.find("L2 Cache configuration") == 0) 
			{
				for(int i = 0; i < 4; i++) 
				{
					std::getline(fd, line);     
					if(line.find("Number of sets") == 0) 
					{
						substring_index = line.find(':'); 
						substring = line.substr(substring_index + 2); 
						ss << substring; 
						ss >> conf.L2_set_num; 
						substring.clear(); 
						ss.clear(); 
					}
					else if(line.find("Set size") == 0)  
					{
						substring_index = line.find(':');  
						substring = line.substr(substring_index + 2);
						ss << substring;  
						ss >> conf.L2_set_size;  
						substring.clear();  
						ss.clear();  
					}
					else if(line.find("Line size") == 0) 
					{
						substring_index = line.find(':'); 
						substring = line.substr(substring_index + 2); 
						ss << substring; 
						ss >> conf.L2_line_size_bytes;  
						substring.clear(); 
						ss.clear();   
					}
					else if(line.find("Write through/no write allocate") == 0)
					{                                       
						substring_index = line.find(':');  
						substring = line.substr(substring_index + 2);
						if(substring == "n" or substring == "no")
						{
							conf.L2_wt_nwa = false;  
						}
						else if(substring == "y" or substring == "yes")
						{
							conf.L2_wt_nwa = true;
						}
						else
						{
							std::cout << "Invalid configuration option" << std::endl;
							exit(1);
						}
						substring.clear();          
					}
				}
				//Error Checking L2 Config Data
				if(conf.L2_set_size > 8)
				{
					std::cout << "L2 Cache can have a maximum set associativity of 8!" << std::endl;
					exit(1);
				}
				if((conf.L2_set_num * conf.L2_set_size) > 1024) 
				{
					std::cout << "L2 Cache can have a maximum of 1024 entries!" << std::endl;
					exit(1);
				}

				if(!isPowerOfTwo(conf.L2_set_num)) 
				{
					std::cout << "L2 Cache set quantity must be a power of 2!" << std::endl;
					exit(1);
				}
				if(!isPowerOfTwo(conf.L2_set_size))
				{
					//if not direct mapped
					if(conf.L2_set_size != 1)  
					{                              
						std::cout << "L2 Cache set size must be 1 for direct mapped or a power of 2 for set"
							" associative!" << std::endl;
						exit(1);
					}
				}
				if(!isPowerOfTwo(conf.L2_line_size_bytes)) 
				{
					std::cout << "L2 Cache line size must be a power of 2!" << std::endl;
					exit(1);
				}
				if(!isPowerOfTwo(conf.L2_set_num * conf.L2_set_size))  
				{
					std::cout << "Number of entries in L2 Cache must be a power of 2!" << std::endl;
					exit(1);
				}
			}
			//Toggle for Virtualization
			else if(line.find("Virtual addresses") == 0) 
			{
				substring_index = line.find(':');   
				substring = line.substr(substring_index + 2); 
				if(substring == "n" or substring == "no")
				{
					conf.virtual_addresses = false; 
				}
				else if(substring == "y" or substring == "yes")
				{
					conf.virtual_addresses = true;   
				}
				else
				{
					std::cout << "Invalid configuration option" << std::endl;
					exit(1);
				}
				substring.clear();  
			}
			//Toggle for DTLB
			else if(line.find("TLB") == 0) 
			{
				substring_index = line.find(':'); 
				substring = line.substr(substring_index + 2);  
				if(substring == "n" or substring == "no")
				{
					conf.dtlb_enabled = false;      
				}
				else if(substring == "y" or substring == "yes")
				{
					conf.dtlb_enabled = true;  
				}
				else
				{
					std::cout << "Invalid configuration option" << std::endl;
					exit(1);
				}
				substring.clear(); 
			}
			// Toggle for L2
			else if(line.find("L2 cache") == 0) 
			{
				substring_index = line.find(':');  
				substring = line.substr(substring_index + 2);  
				if(substring == "n" or substring == "no")
				{
					conf.L2_enabled = false; 
				}
				else if(substring == "y" or substring == "yes")
				{
					conf.L2_enabled = true;  
				}
				else
				{
					std::cout << "Invalid configuration option" << std::endl;
					exit(1);
				}
				substring.clear();
			}
		}
	}

	//Checking for invalid options together
	if(conf.virtual_addresses && conf.dtlb_enabled)
	{
		if(conf.dtlb_set_num < 1 || conf.dtlb_set_size < 1)
		{
			std::cout << "Invalid configuration option" << std::endl;
			exit(1);
		}
	}
	if(conf.virtual_page_num < 2 || conf.physical_page_num < 2 || conf.page_size_bytes < 2)
	{
		std::cout << "Invalid configuration option" << std::endl;
		exit(1);
	}
	if(conf.DC_set_num < 2 || conf.DC_set_size < 1 || conf.DC_line_size_bytes < 2)
	{
		std::cout << "Invalid configuration option" << std::endl;
		exit(1);
	}
	if(conf.L2_set_num < 2 || conf.L2_set_size < 1 || conf.L2_line_size_bytes < 2)
	{
		std::cout << "Invalid configuration option" << std::endl;
		exit(1);
	}
	if((conf.L2_line_size_bytes < conf.DC_line_size_bytes) && conf.L2_enabled)
	{
		std::cout << "L2 Cache's line size must be the same as or larger than DC Cache's line size!" << std::endl;
		exit(1);
	}

	//Using log base 2 to get the number of bits in each value
	conf.page_offset_bits_num = (int)log2(conf.page_size_bytes);
	conf.page_table_index_bits_num = (int)log2(conf.virtual_page_num);
	conf.physical_page_bits_num = (int)log2(conf.physical_page_num); 
	conf.virtual_page_bits_num = (int)log2(conf.virtual_page_num);

	conf.dtlb_index_bits_num = (int)log2(conf.dtlb_set_num);

	conf.DC_offset_bits_num = (int)log2(conf.DC_line_size_bytes);
	conf.DC_index_bits_num = (int)log2(conf.DC_set_num);

	conf.L2_offset_bit_num = (int)log2(conf.L2_line_size_bytes);   
	conf.L2_index_bits_num = (int)log2(conf.L2_set_num);
	if(!conf.virtual_addresses)
	{
		conf.dtlb_enabled = false;  
	}

	//Close and return file
	fd.close(); 
	return conf; 
}

//Function to display options
void showOptions(Config options)
{	
	if(!options.virtual_addresses)
	{
		std::cout << "Data TLB contains " << options.dtlb_set_num << " sets." << std::endl;
		std::cout << "Each set contains " << options.dtlb_set_size << " entries." << std::endl;
		std::cout << "Number of bits used for the index is " << options.dtlb_index_bits_num << "." << std::endl;
		std::cout << std::endl << std::endl;
	}

	else if(options.virtual_addresses && !options.dtlb_enabled)
	{
		std::cout << "Data TLB contains " << options.dtlb_set_num << " sets." << std::endl;
		std::cout << "Each set contains " << options.dtlb_set_size << " entries." << std::endl;
		std::cout << "Number of bits used for the index is " << options.dtlb_index_bits_num << "." << std::endl;
		std::cout << std::endl << std::endl;
	}

	else if(options.virtual_addresses && options.dtlb_enabled)
	{
		std::cout << "Data TLB contains " << options.dtlb_set_num << " sets." << std::endl;
		std::cout << "Each set contains " << options.dtlb_set_size << " entries." << std::endl;
		std::cout << "Number of bits used for the index is " << options.dtlb_index_bits_num << "." << std::endl <<
			std::endl;
	}

	std::cout << "Number of virtual pages is " << options.virtual_page_num << "." << std::endl;
	std::cout << "Number of physical pages is " << options.physical_page_num << "." << std::endl;
	std::cout << "Each page contains " << options.page_size_bytes << " bytes." << std::endl;
	std::cout << "Number of bits used for the page table index is " << options.page_table_index_bits_num << "." <<
		std::endl;
	std::cout << "Number of bits used for the page offset is " << options.page_offset_bits_num << "." << std::endl <<
		std::endl;

	std::cout << "D-Cache contains " << options.DC_set_num << " sets." << std::endl;
	std::cout << "Each set contains " << options.DC_set_size << " entries." << std::endl;
	std::cout << "Each line is " << options.DC_line_size_bytes << " bytes." << std::endl;
	if(options.DC_wt_nwa)
	{
		std::cout << "The cache uses a no-write-allocate and write-through policy." << std::endl;
	}
	else if(!options.DC_wt_nwa)
	{
		std::cout << "The cache uses a write-allocate and write-back policy." << std::endl;
	}
	std::cout << "Number of bits used for the index is " << options.DC_index_bits_num << "." << std::endl;
	std::cout << "Number of bits used for the offset is " << options.DC_offset_bits_num << "." << std::endl <<
		std::endl;

	std::cout << "L2-cache contains " << options.L2_set_num << " sets." << std::endl;
	std::cout << "Each set contains " << options.L2_set_size << " entries." << std::endl;
	std::cout << "Each line is " << options.L2_line_size_bytes << " bytes." << std::endl;
	if(options.L2_wt_nwa)
	{
		std::cout << "The cache uses a no-write-allocate and write-through policy." << std::endl;
	}
	else if(!options.L2_wt_nwa)
	{
		std::cout << "The cache uses a write-allocate and write-back policy." << std::endl;
	}
	std::cout << "Number of bits used for the index is " << options.L2_index_bits_num << "." << std::endl;

	std::cout << "Number of bits used for the offset is " << options.L2_offset_bit_num << "." << std::endl << std::endl;

	if(options.virtual_addresses)
	{
		std::cout << "The addresses read in are virtual addresses." << std::endl;
	}
	else if(!options.virtual_addresses)
	{
		std::cout << "The addresses read in are physical addresses." << std::endl;
	}

	if(!options.dtlb_enabled)
	{
		std::cout << "DTLB is disabled in this configuration." << std::endl;
	}
	if(!options.L2_enabled){
		std::cout << "L2 cache is disabled in this configuration." << std::endl;
	}

	std::cout << std::endl;
}

//Function to check if a given int is a power of 2
bool isPowerOfTwo(int n)
{
	return ((n & (n - 1)) == 0);
}

//Clean address: If it is an illegal address, convert it to a legal address.
std::string cleanAddr(std::string original, Config conf)
{
	std::string cleaned_addr_hex;
	if(!conf.virtual_addresses)
	{
		std::string orig = "0x" + original;

		std::stringstream ss;
		std::stringstream ss2;

		ss << std::hex << orig;

		unsigned long orig_addr;
		ss >> orig_addr;

		std::string mask = ""; 

		for(int i = 0; i < conf.physical_page_bits_num + conf.page_offset_bits_num; i++)
		{
			mask.append("1");
		}
		std::bitset<32> original_bits(orig_addr); 
		std::bitset<32> mask_bits(mask);

		unsigned long cleaned_addr = (original_bits & mask_bits).to_ulong();

		ss2 << std::hex << cleaned_addr; 
		cleaned_addr_hex = ss2.str();
	}
	else if(conf.virtual_addresses)
	{
		std::string orig = "0x" + original; 

		std::stringstream ss;
		std::stringstream ss2;

		ss << std::hex << orig; 

		unsigned long orig_addr; 
		ss >> orig_addr;

		std::string mask = ""; 

		for(int i = 0; i < conf.virtual_page_bits_num + conf.page_offset_bits_num; i++)
		{
			mask.append("1");
		}   

		std::bitset<32> original_bits(orig_addr);
		std::bitset<32> mask_bits(mask);

		unsigned long cleaned_addr = (original_bits & mask_bits).to_ulong();

		ss2 << std::hex << cleaned_addr; 
		cleaned_addr_hex = ss2.str(); 
	}

	return cleaned_addr_hex;                                       
}

//Function to determine the physical page number for an address
int getPhysicalPage(std::string addr, Config conf)
{
	std::string orig_hex = "0x" + addr; 

	std::stringstream ss;
	ss << std::hex << orig_hex; 

	unsigned long address; 
	ss >> address; 

	std::string mask = ""; 

	for(int i = 0; i < conf.physical_page_bits_num; i++)
	{
		mask.append("1");
	} 

	for(int i = 0; i < conf.page_offset_bits_num; i++)
	{
		mask.append("0");
	} 

	std::bitset<32> original_bits(address);
	std::bitset<32> mask_bits(mask); 

	auto physical_page_number_bits = (original_bits & mask_bits);
	physical_page_number_bits >>= conf.page_offset_bits_num;

	int physical_page_number = (int)physical_page_number_bits.to_ulong();

	return physical_page_number;
}

//Function called to process and show a line on standard output.
void showLine(Config conf, std::string addr, int VPN, int pageOffset, int dtlbTag, int dtlbIndex,
		std::string dtlbRes, std::string ptRes, int PFN, int DCTag, int DCIndex, std::string DCRes, int L2Tag, int L2Index, std::string L2Res)
{
	//If using physical addresses
	if(!conf.virtual_addresses)
	{
		//Convert integer to hex
		std::stringstream ssOff;
		std::string offsetStr;
		ssOff << std::hex << pageOffset;
		offsetStr = ssOff.str();

		std::stringstream ss0;
		std::string hex = "0x";
		hex.append(addr);
		ss0 << std::hex << hex;
		unsigned long addrInt;
		ss0 >> addrInt;

		//In actual cache entries, I am storing tag and index as integers.
		//Here, I convert them to a hex string for display
		std::stringstream ss1;
		ss1 << std::hex << PFN;
		std::string ppnStr(ss1.str());
		std::stringstream ss2;
		std::string DCTagStr;

		if(DCTag == -1)
		{
			DCTagStr = "";
		}
		else
		{
			ss2 << std::hex << DCTag;
			DCTagStr = ss2.str();
		}

		std::stringstream ss3;
		std::string DCIndexStr;
		if(DCIndex == -1)
		{
			DCIndexStr = "";
		}
		else
		{
			ss3 << std::hex << DCIndex;
			DCIndexStr = ss3.str();
		}

		//If L2 is disabled
		if(!conf.L2_enabled)
		{
			printf("%08lx %6s %4s %6s %3s %4s %4s %4s %6s %3s %4s %6s %3s %4s\n", addrInt, "", offsetStr.c_str(), "",
					"", "", "", ppnStr.c_str(), DCTagStr.c_str(), DCIndexStr.c_str(), DCRes.c_str(), "", "", "");
		}
		else if(conf.L2_enabled)
		{
			std::stringstream ss4;
			std::string L2TagStr;
			if(L2Tag == -1)
			{
				L2TagStr = "";
			}
			else
			{
				ss4 << std::hex << L2Tag;
				L2TagStr = ss4.str();
			}

			std::stringstream ss5;
			std::string L2IndexStr;
			if(L2Index == -1)
			{
				L2IndexStr = "";
			}
			else
			{
				ss5 << std::hex << L2Index;
				L2IndexStr = ss5.str();
			}

			printf("%08lx %6s %4s %6s %3s %4s %4s %4s %6s %3s %4s %6s %3s %4s\n", addrInt, "", offsetStr.c_str(), "",
					"", "", "", ppnStr.c_str(), DCTagStr.c_str(), DCIndexStr.c_str(), DCRes.c_str(), L2TagStr.c_str(),
					L2IndexStr.c_str(), L2Res.c_str());
		}
	}
	//If using virtual addresses
	else if(conf.virtual_addresses)
	{
		//If the DTLB is enabled
		if(conf.dtlb_enabled) 
		{
			//Convert int to hex for display
			std::stringstream ssVPN;
			std::string vpnStr;
			ssVPN << std::hex << VPN;
			vpnStr = ssVPN.str();

			std::stringstream ssDTLBTag;
			std::string DTLB_Tag_Str;
			ssDTLBTag << std::hex << dtlbTag;
			DTLB_Tag_Str = ssDTLBTag.str();

			std::stringstream ssDTLBInd;
			std::string DTLB_Ind_Str;
			ssDTLBInd << std::hex << dtlbIndex;
			DTLB_Ind_Str = ssDTLBInd.str();

			std::stringstream ssOff;
			std::string offsetStr;
			ssOff << std::hex << pageOffset;
			offsetStr = ssOff.str();

			std::stringstream ss0;
			std::string hex = "0x";
			hex.append(addr);
			ss0 << std::hex << hex;
			unsigned long addrInt;
			ss0 >> addrInt;

			std::stringstream ss1;
			ss1 << std::hex << PFN;
			std::string ppnStr(ss1.str());

			std::stringstream ss2;
			std::string DCTagStr;
			if(DCTag == -1)
			{
				DCTagStr = "";
			}
			else
			{
				ss2 << std::hex << DCTag;
				DCTagStr = ss2.str();
			}

			std::stringstream ss3;
			std::string DCIndexStr;
			if(DCIndex == -1)
			{
				DCIndexStr = "";
			}
			else
			{
				ss3 << std::hex << DCIndex;
				DCIndexStr = ss3.str();
			}
			//If L2 disabled
			if(!conf.L2_enabled)
			{
				printf("%08lx %6s %4s %6s %3s %4s %4s %4s %6s %3s %4s %6s %3s %4s\n", addrInt, vpnStr.c_str(),
						offsetStr.c_str(), DTLB_Tag_Str.c_str(), DTLB_Ind_Str.c_str(), dtlbRes.c_str(), ptRes.c_str(),
						ppnStr.c_str(), DCTagStr.c_str(), DCIndexStr.c_str(), DCRes.c_str(), "", "", "");
			}
			else if(conf.L2_enabled)
			{
				std::stringstream ss4;
				std::string L2TagStr;
				if(L2Tag == -1)
				{
					L2TagStr = "";
				}
				else
				{
					ss4 << std::hex << L2Tag;
					L2TagStr = ss4.str();
				}

				std::stringstream ss5;
				std::string L2IndexStr;
				if(L2Index == -1)
				{
					L2IndexStr = "";
				}
				else
				{
					ss5 << std::hex << L2Index;
					L2IndexStr = ss5.str();
				}

				printf("%08lx %6s %4s %6s %3s %4s %4s %4s %6s %3s %4s %6s %3s %4s\n", addrInt, vpnStr.c_str(),
						offsetStr.c_str(), DTLB_Tag_Str.c_str(), DTLB_Ind_Str.c_str(), dtlbRes.c_str(), ptRes.c_str(),
						ppnStr.c_str(), DCTagStr.c_str(), DCIndexStr.c_str(), DCRes.c_str(), L2TagStr.c_str(),
						L2IndexStr.c_str(), L2Res.c_str());
			}
		}
		//If DTLB is disabled
		else if(!conf.dtlb_enabled)
		{
			//Convert int to hex for display
			std::stringstream ssVPN;
			std::string vpnStr;
			ssVPN << std::hex << VPN;
			vpnStr = ssVPN.str();

			std::stringstream ssOff;
			std::string offsetStr;
			ssOff << std::hex << pageOffset;
			offsetStr = ssOff.str();

			std::stringstream ss0;
			std::string hex = "0x";
			hex.append(addr);
			ss0 << std::hex << hex;
			unsigned long addrInt;
			ss0 >> addrInt;

			std::stringstream ss1;
			ss1 << std::hex << PFN;
			std::string ppnStr(ss1.str());

			std::stringstream ss2;
			std::string DCTagStr;

			if(DCTag == -1)
			{
				DCTagStr = "";
			}
			else
			{
				ss2 << std::hex << DCTag;
				DCTagStr = ss2.str();
			}

			std::stringstream ss3;
			std::string DCIndexStr;
			if(DCIndex == -1)
			{
				DCIndexStr = "";
			}
			else
			{
				ss3 << std::hex << DCIndex;
				DCIndexStr = ss3.str();
			}
			//If L2 disabled
			if(!conf.L2_enabled)                           
			{
				printf("%08lx %6s %4s %6s %3s %4s %4s %4s %6s %3s %4s %6s %3s %4s\n", addrInt, vpnStr.c_str(),
						offsetStr.c_str(), "", "", "", ptRes.c_str(), ppnStr.c_str(), DCTagStr.c_str(),
						DCIndexStr.c_str(), DCRes.c_str(), "", "", "");
			}
			else if(conf.L2_enabled)
			{
				std::stringstream ss4;
				std::string L2TagStr;
				if(L2Tag == -1)
				{
					L2TagStr = "";
				}
				else
				{
					ss4 << std::hex << L2Tag;
					L2TagStr = ss4.str();
				}

				std::stringstream ss5;
				std::string L2IndexStr;
				if(L2Index == -1)
				{
					L2IndexStr = "";
				}
				else
				{
					ss5 << std::hex << L2Index;
					L2IndexStr = ss5.str();
				}

				printf("%08lx %6s %4s %6s %3s %4s %4s %4s %6s %3s %4s %6s %3s %4s\n", addrInt, vpnStr.c_str(),
						offsetStr.c_str(), "", "", "", ptRes.c_str(), ppnStr.c_str(), DCTagStr.c_str(),
						DCIndexStr.c_str(), DCRes.c_str(), L2TagStr.c_str(), L2IndexStr.c_str(), L2Res.c_str());
			}
		}
	}
}

//Run simulation with L2 enabled
void simulate_w_L2(Config options, std::vector<std::vector<CacheRecord>>& DC_rec,
		std::vector<std::vector<CacheRecord>>& L2_rec)
{
	//Setup DC, L2, Page table (optional), and DTLB (optional)
	DC DC_Cache {options.DC_set_num, options.DC_set_size, options.DC_line_size_bytes, options.DC_wt_nwa,
		options.DC_offset_bits_num, options.DC_index_bits_num};
	L2 L2_Cache {options.L2_set_num, options.L2_set_size, options.L2_line_size_bytes, options.L2_wt_nwa,
		options.L2_offset_bit_num, options.L2_index_bits_num};

	PageTable Page_Table = {options.virtual_page_num};

	DTLB dtlb = {options.dtlb_set_num, options.dtlb_set_size, options.dtlb_index_bits_num, options.page_offset_bits_num};

	//Setup Cache Reference variables
	float DC_hits = 0;
	float DC_misses = 0;

	float L2_hits = 0;
	float L2_misses = 0;

	float read_total = 0; 
	float write_total = 0;
	float main_mem_refs = 0;

	//If using virtual addresses, we will need pt and dtlb variables
	float disk_refs = 0; 
	float pt_refs = 0; 
	float pt_hits = 0;
	float pt_faults = 0;
	float page_evictions = 0;

	float dtlb_hits = 0;
	float dtlb_misses = 0;

	//Open file and make some buffer variables
	std::ifstream fd("trace.dat");
	std::string line;
	std::string addr;

	//Read and process in each line from trace.dat
	while(std::getline(fd, line))
	{
		if(!line.empty())
		{
			//Get address, as well as 'R' or 'W'
			std::string substring = line.substr(2);
			char R_W = line[0];
			//Change illegal addresses into usable ones
			std::string newAddr = cleanAddr(substring, options);

			//If using virtual addresses, then newAddrDisp will be used for displaying the virtual address and
			//newAddr will later be assigned the physical address below. This is what is passed through the cache
			//hierarchy. Otherwise, they will stay the same.
			std::string newAddrDisp = newAddr;
			int PFN = -1;
			PFN = getPhysicalPage(newAddr, options);
			int size = -1; 
			int DC_evict_given_L2_evict_PFN = -1;

			//To be used if using virtual addresses
			int VPN = -1; 
			std::string pt_result = "";  
			std::vector<int> DTLB_result; 
			DTLB_result.resize(2); 
			std::string dtlb_result = "";
			int DTLB_found = -1; 

			//If using virtual addressing
			if(options.virtual_addresses)
			{
				VPN = getVirtualPage(newAddr, options); 
				//If DTLB enabled
				if(options.dtlb_enabled) 
				{
					DTLB_result = dtlb.getAddrTagIndex(newAddr); 
					DTLB_found = dtlb.check_match(DTLB_result.at(0), DTLB_result.at(1));

					//DTLB Miss
					if(DTLB_found == -1) 
					{
						dtlb_misses++;
						pt_refs++;
						dtlb_result = "miss";
						//If the PT has a valid translation
						if(Page_Table.isValid(VPN)) 
						{
							PFN = Page_Table.getPFN(VPN); 
							pt_result = "hit ";
							pt_hits++;
							Page_Table.incRefCounter(VPN);
							dtlb.allocate(newAddr, DTLB_result.at(0), DTLB_result.at(1), VPN, PFN);
							newAddr = Page_Table.virtualToPhysical(newAddr, PFN, options.page_offset_bits_num);
						}
						else
						{
							disk_refs++;
							pt_result = "miss";
							pt_faults++;

							std::vector<std::string> assignResult; 
							assignResult.resize(2);
							//0 - are all physical frames filled? 0 - no; 1 - yes
							//1 - physical frame for virtual page to go into

							assignResult = Page_Table.isFull(options.virtual_page_num, options.physical_page_num);
							PFN = std::stoi(assignResult.at(1));

							//If all physical frames are filled
							if(assignResult.at(0) == "1")
							{
								page_evictions++;
								//If DC and L2 both write-back
								if(!options.DC_wt_nwa && !options.L2_wt_nwa)
								{
									invalidate_WB_WB(DC_Cache, L2_Cache, DC_rec, L2_rec, Page_Table, PFN,
											main_mem_refs);
									if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
									{
										disk_refs++;
									}

								}
								//If DC and L2 both write-through
								else if(options.DC_wt_nwa && options.L2_wt_nwa)
								{
									invalidate_WT_WT(DC_Cache, L2_Cache, DC_rec, L2_rec, Page_Table, PFN,
											main_mem_refs);
									if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
									{
										disk_refs++;
									}
								}
								//If DC is write-back and L2 is write-through
								else if(!options.DC_wt_nwa && options.L2_wt_nwa)
								{
									invalidate_WB_WT(DC_Cache, L2_Cache, DC_rec, L2_rec, Page_Table, PFN,
											main_mem_refs);
									if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
									{
										disk_refs++;
									}
								}
								//If DC is write-through and L2 write-back
								else if(options.DC_wt_nwa && !options.L2_wt_nwa)
								{
									invalidate_WT_WB(DC_Cache, L2_Cache, DC_rec, L2_rec, Page_Table, PFN,
											main_mem_refs);
									if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
									{
										disk_refs++; 
									}
								}
								//Invalidate DTLB
								dtlb.invalidateByPFN(PFN);
							}
							//If there are free physical frames
							else if(assignResult.at(0) == "0")
							{
								Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, false);
							}
							dtlb.allocate(newAddr, DTLB_result.at(0), DTLB_result.at(1), VPN, PFN);
							newAddr = Page_Table.virtualToPhysical(newAddr, PFN, options.page_offset_bits_num);
						}
					}
					else
					{
						//DTLB Hit
						dtlb_hits++;
						dtlb_result = "hit ";
						dtlb.incRefCounter(DTLB_result.at(0),DTLB_result.at(1));
						PFN = dtlb.retrievePFN(DTLB_result.at(0), DTLB_result.at(1));
						newAddr = Page_Table.virtualToPhysical(newAddr, PFN, options.page_offset_bits_num);
					}
				}
				else if(!options.dtlb_enabled)
				{
					//DTLB Not Enabled
					pt_refs++;
					if(Page_Table.isValid(VPN))  
					{
						//Page Table Hit
						PFN = Page_Table.getPFN(VPN);
						newAddr = Page_Table.virtualToPhysical(newAddr, PFN, options.page_offset_bits_num);
						pt_result = "hit ";
						pt_hits++;
						Page_Table.incRefCounter(VPN);
					}
					else 
					{
						//Page Table Miss
						disk_refs++;
						pt_result = "miss";
						pt_faults++;

						std::vector<std::string> assignResult;
						assignResult.resize(2);
						//0 - are all physical frames filled? 0 - no; 1 - yes
						//1 - physical frame for virtual page to go into

						assignResult = Page_Table.isFull(options.virtual_page_num, options.physical_page_num);
						PFN = std::stoi(assignResult.at(1));

						if(assignResult.at(0) == "1")
						{
							page_evictions++;
							//If DC and L2 both write-back
							if(!options.DC_wt_nwa && !options.L2_wt_nwa)
							{
								invalidate_WB_WB(DC_Cache, L2_Cache, DC_rec, L2_rec, Page_Table, PFN, main_mem_refs);
								if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
								{
									disk_refs++;
								}

							}
							//If DC and L2 both write-through
							else if(options.DC_wt_nwa && options.L2_wt_nwa)
							{
								invalidate_WT_WT(DC_Cache, L2_Cache, DC_rec, L2_rec, Page_Table, PFN, main_mem_refs);
								if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
								{
									disk_refs++;
								} 
							}
							//If DC is write-back and L2 is write-through
							else if(!options.DC_wt_nwa && options.L2_wt_nwa)
							{
								invalidate_WB_WT(DC_Cache, L2_Cache, DC_rec, L2_rec, Page_Table, PFN, main_mem_refs);
								if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
								{
									disk_refs++;
								} 
							}
							//If DC is write-through and L2 write-back
							else if(options.DC_wt_nwa && !options.L2_wt_nwa)
							{
								invalidate_WT_WB(DC_Cache, L2_Cache, DC_rec, L2_rec, Page_Table, PFN, main_mem_refs);
								if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
								{
									disk_refs++;
								} 
							}
						}
						else if(assignResult.at(0) == "0")
						{
							Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, false);
						}
						newAddr = Page_Table.virtualToPhysical(newAddr, PFN, options.page_offset_bits_num);
					}
				}
			}

			//If Given a Read
			if(R_W == 'R')
			{
				read_total++;
				std::vector<int> DC_result;
				DC_result.resize(2); 
				DC_result = DC_Cache.getAddrTagIndex(newAddr);
				bool DC_found = DC_Cache.check_match(false, DC_result.at(0), DC_result.at(1));

				//If Found in DC
				if(DC_found)
				{
					DC_hits++;
					DC_Cache.incRefCounter(DC_result.at(0), DC_result.at(1));
					//CHECK L2. IF NOT IN L2, INSTALL IN L2 B/C OF *INCLUSIVE POLICY*. EVICT IF NECESSARY

					std::vector<int> L2_result;  
					L2_result.resize(2); 
					L2_result = L2_Cache.getAddrTagIndex(newAddr); 
					bool L2_found = L2_Cache.check_match(false, L2_result.at(0), L2_result.at(1));

					if(L2_found)
					{
						//Don't count as an L2 hit
						L2_Cache.incRefCounter(L2_result.at(0), L2_result.at(1));

						if(options.virtual_addresses) 
						{
							if(options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
										DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
										DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
							}
							else if(!options.dtlb_enabled) 
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1,
										"", pt_result, PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
							}
						}
						else if(!options.virtual_addresses)
						{
							showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
									PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
						}
					}
					//Found in DC but not found in L2
					else if(!L2_found)
					{
						//INSTALL IN L2 BECAUSE OF INCLUSIVE CACHING POLICY => IF DC HAS IT, SO SHOULD L2
						std::vector<std::string> L2_alloc_result; 
						L2_alloc_result.resize(8); 
						L2_alloc_result = L2_Cache.allocate(false, newAddr, L2_result.at(0), L2_result.at(1), PFN, VPN);

						//result:
						//position 0 - did an eviction happen (y/n). If allocate calls evict method then y
						//position 1 - what address is associated with the evicted line? -1 if did not evict
						//position 2 - was the line dirty? -1 if did not evict
						//position 3 - VPN associated with evicted line, -1 if did not evict
						//position 4 - PFN associated with evicted line, -1 if did not evict
						//position 5 - tag associated with evicted line, -1 if did not evict
						//position 6 - index associated with evicted line, -1 if did not evict
						//position 7 - new entry number in set

						//put in record
						size = L2_rec.at(PFN).size();
						L2_rec.at(PFN).resize(size + 1); 
						size = L2_rec.at(PFN).size(); 
						L2_rec.at(PFN).at(size - 1).tag = L2_result.at(0); 
						L2_rec.at(PFN).at(size - 1).index = L2_result.at(1);
						L2_rec.at(PFN).at(size - 1).addr = newAddr;

						//If evicted
						if(L2_alloc_result.at(0) == "y")
						{
							//If L2 is write-back and evicted line was dirty
							if(!L2_Cache.get_wt_nwa() && L2_alloc_result.at(2) == "y")
							{
								main_mem_refs++; 
								if(options.virtual_addresses)
								{  
									Page_Table.setPageDirty(std::stoi(L2_alloc_result.at(3)));
								}  
							}

							//if did evict, remove from page record
							size = L2_rec.at(std::stoi(L2_alloc_result.at(4))).size();

							for(int i = 0; i < size; i++)
							{
								if(L2_rec.at(std::stoi(L2_alloc_result.at(4))).at(i).tag ==
										std::stoi(L2_alloc_result.at(5)) &&
										L2_rec.at(std::stoi(L2_alloc_result.at(4))).at(i).index ==
										std::stoi(L2_alloc_result.at(6)))
								{
									//if cache record tag matches evicted line's tag and cache record index matches
									//evicted line's index, erase record
									L2_rec.at(std::stoi(L2_alloc_result.at(4))).erase(L2_rec.at
											(std::stoi(L2_alloc_result.at(4))).begin() + i);
									break;                                       //break out of loop when found
								}
							}

							//When we evict in L2, we evict the corresponding line in DC

							std::vector<int> DC_evict_given_L2_evict;
							DC_evict_given_L2_evict.resize(2); 
							DC_evict_given_L2_evict = DC_Cache.getAddrTagIndex(L2_alloc_result.at(1));
							DC_evict_given_L2_evict_PFN = DC_Cache.evictLine(DC_evict_given_L2_evict.at(0),
									DC_evict_given_L2_evict.at(1));

							if(DC_evict_given_L2_evict_PFN != -1)
							{
								size = DC_rec.at(DC_evict_given_L2_evict_PFN).size();

								//to remove from record
								for(int i = 0; i < size; i++)                    //for cache entries for that page for DC
								{
									if(DC_rec.at(DC_evict_given_L2_evict_PFN).at(i).tag ==
											DC_evict_given_L2_evict.at(0) &&
											DC_rec.at(DC_evict_given_L2_evict_PFN).at(i).index ==
											DC_evict_given_L2_evict.at(1))
									{
										//if cache record tag matches evicted line's tag and cache record index matches
										//evicted line's index, erase record
										DC_rec.at(DC_evict_given_L2_evict_PFN).erase(DC_rec.at
												(DC_evict_given_L2_evict_PFN).begin() + i);
										break;                                   //break out of loop when found
									}
								}
							}

							//If write-back, we don't need to write back to L2 since we just evicted that line from L2
						}
						//If using virtual addresses
						if(options.virtual_addresses)                            
						{
							if(options.dtlb_enabled)                              
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
										DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
										DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
							}
							else if(!options.dtlb_enabled)                        
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1,
										"", pt_result, PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
							}
						}
						else if(!options.virtual_addresses)                      
						{
							showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
									PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
						}
					}
				}
				//If not found in DC
				else if(!DC_found)
				{
					DC_misses++;

					std::vector<int> L2_result;                                        
					L2_result.resize(2);                                               
					L2_result = L2_Cache.getAddrTagIndex(newAddr);                     
					bool L2_found = L2_Cache.check_match(false, L2_result.at(0), L2_result.at(1));

					//If Found in L2                                                              
					if(L2_found)
					{
						L2_hits++;
						L2_Cache.incRefCounter(L2_result.at(0), L2_result.at(1));      
						std::vector<std::string> DC_alloc_result;                      
						DC_alloc_result.resize(8);                                     
						DC_alloc_result = DC_Cache.allocate(false, newAddr, DC_result.at(0), DC_result.at(1), PFN, VPN);

						size = DC_rec.at(PFN).size();                                  

						DC_rec.at(PFN).resize(size + 1);                               
						size = DC_rec.at(PFN).size();                                  
						DC_rec.at(PFN).at(size - 1).tag = DC_result.at(0);             
						DC_rec.at(PFN).at(size - 1).index = DC_result.at(1);           
						DC_rec.at(PFN).at(size - 1).addr = newAddr;                    


						if(DC_alloc_result.at(0) == "y")
						{

							size = DC_rec.at(std::stoi(DC_alloc_result.at(4))).size();

							for(int i = 0; i < size; i++)                              
							{
								if(DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).tag ==
										std::stoi(DC_alloc_result.at(5)) &&
										DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).index ==
										std::stoi(DC_alloc_result.at(6)))
								{
									DC_rec.at(std::stoi(DC_alloc_result.at(4))).erase(DC_rec.at
											(std::stoi(DC_alloc_result.at(4))).begin() + i);
									break;                                             
								}
							}
						}

						//If did evict from DC and DC is write-back and evicted line was dirty
						if(DC_alloc_result.at(0) == "y" && !DC_Cache.get_wt_nwa() && DC_alloc_result.at(2) == "y")
						{
							std::vector<int> L2_result_wb;                             
							L2_result_wb.resize(2);                                    
							L2_result_wb = L2_Cache.getAddrTagIndex(DC_alloc_result.at(1));
							bool L2_found_wb = false;
							//If L2 write-back
							if(!L2_Cache.get_wt_nwa())                                 
							{
								L2_found_wb = L2_Cache.check_match(true, L2_result_wb.at(0), L2_result_wb.at(1));
							}                                                          
							//If L2 write-through
							else if(L2_Cache.get_wt_nwa())                             
							{
								L2_found_wb = L2_Cache.check_match(false, L2_result_wb.at(0), L2_result_wb.at(1));
							}                                                          

							if(L2_found_wb)
							{
								L2_hits++;
								if(L2_Cache.get_wt_nwa())                              
								{
									main_mem_refs++;                                   
									if(options.virtual_addresses)
									{                                                  
										Page_Table.setPageDirty(L2_Cache.VPN_From_Tag_Index(L2_result_wb.at(0),
													L2_result_wb.at(1)));
									}
								}
							}
						}

						if(options.virtual_addresses)
						{
							if(options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
										DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
										DC_result.at(0), DC_result.at(1), "miss", L2_result.at(0), L2_result.at(1),
										"hit ");
							}
							else if(!options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1,
										"", pt_result, PFN, DC_result.at(0), DC_result.at(1), "miss",
										L2_result.at(0), L2_result.at(1), "hit ");
							}
						}
						else if(!options.virtual_addresses)
						{
							showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
									PFN, DC_result.at(0), DC_result.at(1), "miss", L2_result.at(0), L2_result.at(1),
									"hit ");
						}
					}
					//If found in neither
					else if(!L2_found)
					{
						L2_misses++;
						main_mem_refs++;

						std::vector<std::string> DC_alloc_result;                      
						DC_alloc_result.resize(8);                                     
						DC_alloc_result = DC_Cache.allocate(false, newAddr, DC_result.at(0), DC_result.at(1), PFN, VPN);

						size = DC_rec.at(PFN).size();                                  

						DC_rec.at(PFN).resize(size + 1);                               
						size = DC_rec.at(PFN).size();                                  
						DC_rec.at(PFN).at(size - 1).tag = DC_result.at(0);             
						DC_rec.at(PFN).at(size - 1).index = DC_result.at(1);           
						DC_rec.at(PFN).at(size - 1).addr = newAddr;                    

						if(DC_alloc_result.at(0) == "y")
						{          
							size = DC_rec.at(std::stoi(DC_alloc_result.at(4))).size();

							for(int i = 0; i < size; i++)                              
							{
								if(DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).tag ==
										std::stoi(DC_alloc_result.at(5)) &&
										DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).index ==
										std::stoi(DC_alloc_result.at(6)))
								{

									DC_rec.at(std::stoi(DC_alloc_result.at(4))).erase(DC_rec.at
											(std::stoi(DC_alloc_result.at(4))).begin() + i);
									break;                                             
								}
							}
						}
						//If did evict from DC and DC is write-back and the evicted line was dirty
						if(DC_alloc_result.at(0) == "y" && !DC_Cache.get_wt_nwa() && DC_alloc_result.at(2) == "y")
						{
							std::vector<int> L2_result_wb;                             
							L2_result_wb.resize(2);                                    
							L2_result_wb = L2_Cache.getAddrTagIndex(DC_alloc_result.at(1));

							bool L2_found_wb = false;
							if(!L2_Cache.get_wt_nwa())                                 
							{
								L2_found_wb = L2_Cache.check_match(true, L2_result_wb.at(0), L2_result_wb.at(1));
							}                                                          

							else if(L2_Cache.get_wt_nwa())                             
							{
								L2_found_wb = L2_Cache.check_match(false, L2_result_wb.at(0), L2_result_wb.at(1));                                                   
							}                                                          

							if(L2_found_wb)
							{
								L2_hits++;
								if(L2_Cache.get_wt_nwa())                              
								{
									main_mem_refs++;                                   
									if(options.virtual_addresses)
									{                                                  
										Page_Table.setPageDirty(L2_Cache.VPN_From_Tag_Index(L2_result_wb.at(0),
													L2_result_wb.at(1)));                                                
									}
								}
							}
						}
						//Allocate in L2
						std::vector<std::string> L2_alloc_result;                      
						L2_alloc_result.resize(8);                                     
						L2_alloc_result = L2_Cache.allocate(false, newAddr, L2_result.at(0), L2_result.at(1), PFN, VPN);

						size = L2_rec.at(PFN).size();                                  

						L2_rec.at(PFN).resize(size + 1);                               
						size = L2_rec.at(PFN).size();                                  
						L2_rec.at(PFN).at(size - 1).tag = L2_result.at(0);             
						L2_rec.at(PFN).at(size - 1).index = L2_result.at(1);           
						L2_rec.at(PFN).at(size - 1).addr = newAddr;                    

						if(L2_alloc_result.at(0) == "y")                               
						{
							if(!L2_Cache.get_wt_nwa() && L2_alloc_result.at(2) == "y")
							{
								main_mem_refs++;                                       

								if(options.virtual_addresses)
								{                                                      
									Page_Table.setPageDirty(std::stoi(L2_alloc_result.at(3)));
								}                                                      
							}          
							size = L2_rec.at(std::stoi(L2_alloc_result.at(4))).size();

							for(int i = 0; i < size; i++)                              
							{
								if(L2_rec.at(std::stoi(L2_alloc_result.at(4))).at(i).tag ==
										std::stoi(L2_alloc_result.at(5)) &&
										L2_rec.at(std::stoi(L2_alloc_result.at(4))).at(i).index ==
										std::stoi(L2_alloc_result.at(6)))
								{          
									L2_rec.at(std::stoi(L2_alloc_result.at(4))).erase(L2_rec.at
											(std::stoi(L2_alloc_result.at(4))).begin() + i);
									break;                                             
								}
							}               
							//When we evict in L2, we evict the corresponding line in DC
							std::vector<int> DC_evict_given_L2_evict;                  

							DC_evict_given_L2_evict.resize(2);                         

							DC_evict_given_L2_evict = DC_Cache.getAddrTagIndex(L2_alloc_result.at(1));
							DC_evict_given_L2_evict_PFN = DC_Cache.evictLine(DC_evict_given_L2_evict.at(0),
									DC_evict_given_L2_evict.at(1));

							//If Found
							if(DC_evict_given_L2_evict_PFN != -1)                      
							{
								size = DC_rec.at(DC_evict_given_L2_evict_PFN).size();

								for(int i = 0; i < size; i++)                          
								{
									if(DC_rec.at(DC_evict_given_L2_evict_PFN).at(i).tag ==
											DC_evict_given_L2_evict.at(0) &&
											DC_rec.at(DC_evict_given_L2_evict_PFN).at(i).index ==
											DC_evict_given_L2_evict.at(1))
									{    
										DC_rec.at(DC_evict_given_L2_evict_PFN).erase(DC_rec.at
												(DC_evict_given_L2_evict_PFN).begin() + i);
										break;                                         
									}
								}
							}
						}

						if(options.virtual_addresses)
						{
							if(options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
										DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
										DC_result.at(0), DC_result.at(1), "miss", L2_result.at(0), L2_result.at(1),
										"miss");
							}
							else if(!options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1,
										"", pt_result, PFN, DC_result.at(0), DC_result.at(1), "miss",
										L2_result.at(0), L2_result.at(1), "miss");
							}
						}
						else if(!options.virtual_addresses)
						{
							showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
									PFN, DC_result.at(0), DC_result.at(1), "miss", L2_result.at(0), L2_result.at(1),
									"miss");
						}
					}
				}
			}
			//Else if Write
			else if(R_W == 'W')
			{
				write_total++;
				std::vector<int> DC_result;                                            
				DC_result.resize(2);                                                   
				DC_result = DC_Cache.getAddrTagIndex(newAddr);                         
				bool DC_found = false;
				if(!DC_Cache.get_wt_nwa())                                             
				{
					DC_found = DC_Cache.check_match(true, DC_result.at(0), DC_result.at(1));
				}
				else if(DC_Cache.get_wt_nwa())                                         
				{
					DC_found = DC_Cache.check_match(false, DC_result.at(0), DC_result.at(1));
				}                                                                      

				//If found in DC
				if(DC_found)
				{
					DC_hits++;                                                         
					DC_Cache.incRefCounter(DC_result.at(0), DC_result.at(1));          

					std::vector<int> L2_result;                                        
					L2_result.resize(2);                                               
					L2_result = L2_Cache.getAddrTagIndex(newAddr);                     

					bool L2_found = false;
					if(!L2_Cache.get_wt_nwa())                                         
					{
						L2_found = L2_Cache.check_match(true, L2_result.at(0), L2_result.at(1));                                                           
					}
					else if(L2_Cache.get_wt_nwa())                                     
					{
						L2_found = L2_Cache.check_match(false, L2_result.at(0), L2_result.at(1));                                                               
					}                                                                  

					//If Found in L2
					if(L2_found)                                                       
					{
						L2_Cache.incRefCounter(L2_result.at(0), L2_result.at(1));

						if(DC_Cache.get_wt_nwa())                                      
						{
							L2_hits++;                                                 
							if(L2_Cache.get_wt_nwa())                                  
							{                                                          
								main_mem_refs++;
								if(options.virtual_addresses)
								{
									Page_Table.setPageDirty(VPN);                      
								}
							}
						}

						if(options.virtual_addresses)
						{
							if(options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
										DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
										DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
							}
							else if(!options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1,
										"", pt_result, PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
							}
						}
						else if(!options.virtual_addresses)
						{
							showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
									PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
						}
					}
					//If found in DC but not found in L2
					else if(!L2_found)
					{
						if(!L2_Cache.get_wt_nwa())                                     
						{                                                              
							std::vector<std::string> L2_alloc_result;                  
							L2_alloc_result.resize(8);                                 
							L2_alloc_result = L2_Cache.allocate(true, newAddr, L2_result.at(0), L2_result.at(1), PFN, VPN);
							size = L2_rec.at(PFN).size();                              

							L2_rec.at(PFN).resize(size + 1);                           
							size = L2_rec.at(PFN).size();                              
							L2_rec.at(PFN).at(size - 1).tag = L2_result.at(0);         
							L2_rec.at(PFN).at(size - 1).index = L2_result.at(1);       
							L2_rec.at(PFN).at(size - 1).addr = newAddr;                

							//If did evict from L2
							if(L2_alloc_result.at(0) == "y")                           
							{ 
								//If L2 is write-back and evicted line was dirty
								if(!L2_Cache.get_wt_nwa() && L2_alloc_result.at(2) == "y")
								{
									main_mem_refs++;                                   

									if(options.virtual_addresses)
									{                                                  
										Page_Table.setPageDirty(std::stoi(L2_alloc_result.at(3)));
									}                                                  
								}

								size = L2_rec.at(std::stoi(L2_alloc_result.at(4))).size();

								for(int i = 0; i < size; i++)                          
								{                                                      
									if(L2_rec.at(std::stoi(L2_alloc_result.at(4))).at(i).tag ==
											std::stoi(L2_alloc_result.at(5)) &&
											L2_rec.at(std::stoi(L2_alloc_result.at(4))).at(i).index ==
											std::stoi(L2_alloc_result.at(6)))
									{

										L2_rec.at(std::stoi(L2_alloc_result.at(4))).erase(L2_rec.at
												(std::stoi(L2_alloc_result.at(4))).begin() + i);
										break;                                         
									}
								}
								std::vector<int> DC_evict_given_L2_evict;                                                           
								DC_evict_given_L2_evict.resize(2);                     

								DC_evict_given_L2_evict = DC_Cache.getAddrTagIndex(L2_alloc_result.at(1));
								DC_evict_given_L2_evict_PFN = DC_Cache.evictLine(DC_evict_given_L2_evict.at(0),
										DC_evict_given_L2_evict.at(1));

								if(DC_evict_given_L2_evict_PFN != -1)                  
								{
									size = DC_rec.at(DC_evict_given_L2_evict_PFN).size();

									for(int i = 0; i < size; i++)                      
									{                                                  
										if(DC_rec.at(DC_evict_given_L2_evict_PFN).at(i).tag ==
												DC_evict_given_L2_evict.at(0) &&
												DC_rec.at(DC_evict_given_L2_evict_PFN).at(i).index ==
												DC_evict_given_L2_evict.at(1))
										{
											DC_rec.at(DC_evict_given_L2_evict_PFN).erase(DC_rec.at
													(DC_evict_given_L2_evict_PFN).begin() + i);
											break;                                     
										}
									}
								}
							}
						}

						if(options.virtual_addresses)
						{
							if(options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
										DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
										DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
							}
							else if(!options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1,
										"", pt_result, PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
							}
						}
						else if(!options.virtual_addresses)
						{
							showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
									PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
						}
					}
				}
				//If not found in DC
				else if(!DC_found)
				{
					DC_misses++;

					std::vector<int> L2_result;                                        
					L2_result.resize(2);                                               
					L2_result = L2_Cache.getAddrTagIndex(newAddr);                     

					bool L2_found = false;
					if(!L2_Cache.get_wt_nwa())                                         
					{
						L2_found = L2_Cache.check_match(true, L2_result.at(0), L2_result.at(1));                                                           
					}
					else if(L2_Cache.get_wt_nwa())                                     
					{
						L2_found = L2_Cache.check_match(false, L2_result.at(0), L2_result.at(1));                                                              
					}                                                                                                                                                  
					//If found in L2 but not DC
					if(L2_found)
					{
						L2_hits++;
						L2_Cache.incRefCounter(L2_result.at(0), L2_result.at(1));      
						if(L2_Cache.get_wt_nwa())                                      
						{                                                              
							main_mem_refs++;                                           
							if(options.virtual_addresses)
							{
								Page_Table.setPageDirty(VPN);                          
							}
						}
						if(!DC_Cache.get_wt_nwa())                                     
						{
							std::vector<std::string> DC_alloc_result;                  
							DC_alloc_result.resize(8);                                 
							DC_alloc_result = DC_Cache.allocate(true, newAddr, DC_result.at(0), DC_result.at(1), PFN, VPN);
							size = DC_rec.at(PFN).size();                              

							DC_rec.at(PFN).resize(size + 1);                           
							size = DC_rec.at(PFN).size();                              
							DC_rec.at(PFN).at(size - 1).tag = DC_result.at(0);         
							DC_rec.at(PFN).at(size - 1).index = DC_result.at(1);       
							DC_rec.at(PFN).at(size - 1).addr = newAddr;                

							//If did evict in DC
							if(DC_alloc_result.at(0) == "y")
							{
								size = DC_rec.at(std::stoi(DC_alloc_result.at(4))).size();

								for(int i = 0; i < size; i++)                          
								{                                                      
									if(DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).tag ==
											std::stoi(DC_alloc_result.at(5)) &&
											DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).index ==
											std::stoi(DC_alloc_result.at(6)))
									{
										DC_rec.at(std::stoi(DC_alloc_result.at(4))).erase(DC_rec.at
												(std::stoi(DC_alloc_result.at(4))).begin() + i);
										break;                                         
									}
								}
							}
							//If did evict from DC && DC is write-back and evicted line was dirty
							if(DC_alloc_result.at(0) == "y" && !DC_Cache.get_wt_nwa() && DC_alloc_result.at(2) == "y")
							{
								std::vector<int> L2_result_wb;                         
								L2_result_wb.resize(2);                                
								L2_result_wb = L2_Cache.getAddrTagIndex(DC_alloc_result.at(1));

								bool L2_found_wb = false;
								if(!L2_Cache.get_wt_nwa())                             
								{
									L2_found_wb = L2_Cache.check_match(true, L2_result_wb.at(0), L2_result_wb.at(1));                                                  
								}                                                      

								else if(L2_Cache.get_wt_nwa())                         
								{
									L2_found_wb = L2_Cache.check_match(false, L2_result_wb.at(0), L2_result_wb.at(1));                                                         
								}                                                      
								if(L2_found_wb)
								{
									L2_hits++;
									if(L2_Cache.get_wt_nwa())                          
									{
										main_mem_refs++;                               
										if(options.virtual_addresses)
										{                                              
											Page_Table.setPageDirty(L2_Cache.VPN_From_Tag_Index(L2_result_wb.at(0),
														L2_result_wb.at(1)));                                                                   
										}
									}
								}
							}
						}

						if(options.virtual_addresses)
						{
							if(options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
										DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
										DC_result.at(0), DC_result.at(1), "miss", L2_result.at(0), L2_result.at(1),
										"hit ");
							}
							else if(!options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1,
										"", pt_result, PFN, DC_result.at(0), DC_result.at(1), "miss",
										L2_result.at(0), L2_result.at(1), "hit ");
							}
						}
						else if(!options.virtual_addresses)
						{
							showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
									PFN, DC_result.at(0), DC_result.at(1), "miss", L2_result.at(0), L2_result.at(1),
									"hit ");
						}
					}
					//If found in neither, go to main mem
					else if(!L2_found)
					{
						L2_misses++;
						main_mem_refs++;                                               

						if(!DC_Cache.get_wt_nwa())                                     
						{
							std::vector<std::string> DC_alloc_result;                  
							DC_alloc_result.resize(8);                                 
							DC_alloc_result = DC_Cache.allocate(true, newAddr, DC_result.at(0), DC_result.at(1), PFN, VPN);
							size = DC_rec.at(PFN).size();                              

							DC_rec.at(PFN).resize(size + 1);                           
							size = DC_rec.at(PFN).size();                              
							DC_rec.at(PFN).at(size - 1).tag = DC_result.at(0);         
							DC_rec.at(PFN).at(size - 1).index = DC_result.at(1);       
							DC_rec.at(PFN).at(size - 1).addr = newAddr;                

							if(DC_alloc_result.at(0) == "y")
							{
								size = DC_rec.at(std::stoi(DC_alloc_result.at(4))).size();

								for(int i = 0; i < size; i++)                          
								{                                                      
									if(DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).tag ==
											std::stoi(DC_alloc_result.at(5)) &&
											DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).index ==
											std::stoi(DC_alloc_result.at(6)))
									{ 
										DC_rec.at(std::stoi(DC_alloc_result.at(4))).erase(DC_rec.at
												(std::stoi(DC_alloc_result.at(4))).begin() + i);
										break;                                         
									}
								}
							}

							//If did evict from DC && DC is write-back and evicted line was dirty
							if(DC_alloc_result.at(0) == "y" && !DC_Cache.get_wt_nwa() && DC_alloc_result.at(2) == "y")
							{
								std::vector<int> L2_result_wb;                                 
								L2_result_wb.resize(2);                                        
								L2_result_wb = L2_Cache.getAddrTagIndex(DC_alloc_result.at(1));

								bool L2_found_wb = false;
								if(!L2_Cache.get_wt_nwa())                                     
								{
									L2_found_wb = L2_Cache.check_match(true, L2_result_wb.at(0), L2_result_wb.at(1));

								}
								else if(L2_Cache.get_wt_nwa())                                 
								{
									L2_found_wb = L2_Cache.check_match(false, L2_result_wb.at(0), L2_result_wb.at(1));                                                             
								}
								if(L2_found_wb)
								{
									L2_hits++;
									if(L2_Cache.get_wt_nwa())                                  
									{
										main_mem_refs++;                                       
										if(options.virtual_addresses)
										{                                                      
											Page_Table.setPageDirty(L2_Cache.VPN_From_Tag_Index(L2_result_wb.at(0),
														L2_result_wb.at(1)));
										}
									}
								}
							}
						}
						if(!L2_Cache.get_wt_nwa())                                             
						{

							std::vector<std::string> L2_alloc_result;                          
							L2_alloc_result.resize(8);                                         
							L2_alloc_result = L2_Cache.allocate(true, newAddr, L2_result.at(0), L2_result.at(1), PFN,
									VPN);
							size = L2_rec.at(PFN).size();                                      

							L2_rec.at(PFN).resize(size + 1);                                   
							size = L2_rec.at(PFN).size();                                      
							L2_rec.at(PFN).at(size - 1).tag = L2_result.at(0);                 
							L2_rec.at(PFN).at(size - 1).index = L2_result.at(1);               
							L2_rec.at(PFN).at(size - 1).addr = newAddr;                        

							if(L2_alloc_result.at(0) == "y")                                   
							{
								if(!L2_Cache.get_wt_nwa() && L2_alloc_result.at(2) == "y")
								{
									main_mem_refs++;                                           

									if(options.virtual_addresses)
									{                                                          
										Page_Table.setPageDirty(std::stoi(L2_alloc_result.at(3)));
									}                                                          
								}

								size = L2_rec.at(std::stoi(L2_alloc_result.at(4))).size();

								for(int i = 0; i < size; i++)                                  
								{                                                              
									if(L2_rec.at(std::stoi(L2_alloc_result.at(4))).at(i).tag ==
											std::stoi(L2_alloc_result.at(5)) &&
											L2_rec.at(std::stoi(L2_alloc_result.at(4))).at(i).index ==
											std::stoi(L2_alloc_result.at(6)))
									{
										L2_rec.at(std::stoi(L2_alloc_result.at(4))).erase(L2_rec.at
												(std::stoi(L2_alloc_result.at(4))).begin() + i);
										break;                                                 
									}
								}

								std::vector<int> DC_evict_given_L2_evict;                      

								DC_evict_given_L2_evict.resize(2);                             

								DC_evict_given_L2_evict = DC_Cache.getAddrTagIndex(L2_alloc_result.at(1));

								DC_evict_given_L2_evict_PFN = DC_Cache.evictLine(DC_evict_given_L2_evict.at(0),
										DC_evict_given_L2_evict.at(1));

								if(DC_evict_given_L2_evict_PFN != -1)
								{
									size = DC_rec.at(DC_evict_given_L2_evict_PFN).size();

									for(int i = 0; i < size; i++)                              
									{                                                          
										if(DC_rec.at(DC_evict_given_L2_evict_PFN).at(i).tag ==
												DC_evict_given_L2_evict.at(0) &&
												DC_rec.at(DC_evict_given_L2_evict_PFN).at(i).index ==
												DC_evict_given_L2_evict.at(1))
										{
											DC_rec.at(DC_evict_given_L2_evict_PFN).erase(DC_rec.at
													(DC_evict_given_L2_evict_PFN).begin() + i);
											break;                                             
										}
									}
								}
							}
						}

						if(DC_Cache.get_wt_nwa() && L2_Cache.get_wt_nwa())                     
						{                                                                      
							main_mem_refs++;                                                   
							if(options.virtual_addresses)                                      
							{
								Page_Table.setPageDirty(VPN);                                  
							}
						}

						if(options.virtual_addresses)
						{
							if(options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
										DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
										DC_result.at(0), DC_result.at(1), "miss", L2_result.at(0), L2_result.at(1),
										"miss");
							}
							else if(!options.dtlb_enabled)
							{
								showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1,
										"", pt_result, PFN, DC_result.at(0), DC_result.at(1), "miss",
										L2_result.at(0), L2_result.at(1), "miss");
							}
						}
						else if(!options.virtual_addresses)
						{
							showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
									PFN, DC_result.at(0), DC_result.at(1), "miss", L2_result.at(0), L2_result.at(1),
									"miss");
						}
					}
				}
			}
		}
	}

	//Now display the statistics
	std::cout << std::endl;
	std::cout << "Simulation statistics" << std::endl;
	std::cout << std::endl;

	if(options.dtlb_enabled)
	{
		std::cout << "dtlb hits        : " << (int)dtlb_hits <<  std::endl;
		std::cout << "dtlb misses      : " << (int)dtlb_misses <<  std::endl;
		std::cout << "dtlb hit ratio   : " << (float)(dtlb_hits / (dtlb_hits+dtlb_misses)) <<  std::endl << std::endl;
	}
	else{
		std::cout << "dtlb hits        : " << 0 <<  std::endl;
		std::cout << "dtlb misses      : " << 0 <<  std::endl;
		std::cout << "dtlb hit ratio   : N/A"  <<  std::endl << std::endl;
	}

	if(options.virtual_addresses){
		std::cout << "pt hits          : " << (int)pt_hits <<  std::endl;
		std::cout << "pt faults        : " << (int)pt_faults <<  std::endl;
		std::cout << "pt hit ratio     : " << (float)(pt_hits / (pt_hits+pt_faults)) <<  std::endl << std::endl;
	}
	else{
		std::cout << "pt hits          : " << 0 <<  std::endl;
		std::cout << "pt faults        : " << 0 <<  std::endl;
		std::cout << "pt hit ratio     : N/A" << std::endl << std::endl;
	}

	std::cout << "dc hits          : " << (int)DC_hits <<  std::endl;
	std::cout << "dc misses        : " << (int)DC_misses <<  std::endl;
	std::cout << "dc hit ratio     : " << (float)(DC_hits / (DC_hits+DC_misses)) <<  std::endl << std::endl;

	std::cout << "L2 hits          : " << (int)L2_hits <<  std::endl;
	std::cout << "L2 misses        : " << (int)L2_misses <<  std::endl;
	std::cout << "L2 hit ratio     : " << (float)(L2_hits / (L2_hits+L2_misses)) <<  std::endl << std::endl;

	std::cout << "Total reads      : " << (int)read_total << std::endl;
	std::cout << "Total writes     : " << (int)write_total << std::endl;
	std::cout << "Ratio of reads   : " << (float)(read_total / (read_total+write_total)) << std::endl << std::endl;

	std::cout << "main memory refs : " << (int)main_mem_refs << std::endl;
	std::cout << "page table refs  : " << (int)pt_refs << std::endl;
	std::cout << "disk refs        : " << (int)disk_refs << std::endl;

	//Close fd
	fd.close();                                  
}

//Function to simulate with No L2
void simulate_no_L2(Config options, std::vector<std::vector<CacheRecord>>& DC_rec)
{
	//Initialize DC, PT, and dtlb, as well as simulation 
	DC DC_Cache {options.DC_set_num, options.DC_set_size, options.DC_line_size_bytes, options.DC_wt_nwa,
		options.DC_offset_bits_num, options.DC_index_bits_num};

	PageTable Page_Table = {options.virtual_page_num};                           

	DTLB dtlb = {options.dtlb_set_num, options.dtlb_set_size, options.dtlb_index_bits_num, options.page_offset_bits_num};

	float DC_hits = 0;                                                           
	float DC_misses = 0;                                                         

	float read_total = 0;                                                        
	float write_total = 0;                                                       
	float main_mem_refs = 0;                                                     

	float disk_refs = 0;                                                         
	float pt_refs = 0;                                                           
	float pt_hits = 0;                                                           
	float pt_faults = 0;                                                         
	float page_evictions = 0;                                                    

	float dtlb_hits = 0;                                                          
	float dtlb_misses = 0;                                                        
	
	//Open up trace.dat
	std::ifstream fd("trace.dat");                                               

	std::string line;                                                            
	std::string addr;                                                            
	
	//Read each line
	while(std::getline(fd, line))                                                
	{
		if(!line.empty())                                                        
		{
			//Get R or W, then get address
			std::string substring = line.substr(2);                              
			char R_W = line[0];                                                  
			std::string newAddr = cleanAddr(substring, options);

			int PFN = -1;                                                        
			PFN = getPhysicalPage(newAddr, options);                       

			int size = -1;

			std::string newAddrDisp = newAddr;                                   
			int VPN = -1;                                                        
			std::string pt_result = "";                                          
			std::vector<int> DTLB_result;                                         
			DTLB_result.resize(2);                                                
			std::string dtlb_result = "";                                         
			int DTLB_found = -1;                                                  
			
			//If using virtual addresses
			if(options.virtual_addresses)                                        
			{
				VPN = getVirtualPage(newAddr, options);                     
				
				//If using dtlb
				if(options.dtlb_enabled)                                          
				{
					DTLB_result = dtlb.getAddrTagIndex(newAddr);                   
					DTLB_found = dtlb.check_match(DTLB_result.at(0), DTLB_result.at(1));
					//If dtlb miss
					if(DTLB_found == -1)                                          
					{
						dtlb_misses++;
						pt_refs++;
						dtlb_result = "miss";
						//If PT hit
						if(Page_Table.isValid(VPN))                              
						{
							PFN = Page_Table.getPFN(VPN);                        
							pt_result = "hit ";
							pt_hits++;                                           
							Page_Table.incRefCounter(VPN);                       
							dtlb.allocate(newAddr, DTLB_result.at(0), DTLB_result.at(1), VPN, PFN);

							newAddr = Page_Table.virtualToPhysical(newAddr, PFN, options.page_offset_bits_num);
						}
						else                                                     
						{
							//PT miss
							disk_refs++;                                         
							pt_result = "miss";                                  
							pt_faults++;                                         

							std::vector<std::string> assignResult;               
							assignResult.resize(2);

							assignResult = Page_Table.isFull(options.virtual_page_num, options.physical_page_num);
							PFN = std::stoi(assignResult.at(1));                 
							
							//If PT is full, evict
							if(assignResult.at(0) == "1")                        
							{
								page_evictions++;
								//If wb_wa
								if(!options.DC_wt_nwa)
								{
									invalidate_WB(DC_Cache, DC_rec, Page_Table, PFN, main_mem_refs);
									if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
									{
										disk_refs++;                             
									}                                            
								}
								//If wt_nwa
								else if(options.DC_wt_nwa)
								{
									invalidate_WT(DC_Cache, DC_rec, Page_Table, PFN, main_mem_refs);
									if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
									{
										disk_refs++;                             
									}                                            
								}
								dtlb.invalidateByPFN(PFN);                        
							}
							//If PT is not full, assign frame
							else if(assignResult.at(0) == "0")                   
							{

								Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, false);
							}
							dtlb.allocate(newAddr, DTLB_result.at(0), DTLB_result.at(1), VPN, PFN);

							newAddr = Page_Table.virtualToPhysical(newAddr, PFN, options.page_offset_bits_num);
						}
					}
					else                                                         
					{
						//DTLB hit
						dtlb_hits++;
						dtlb_result = "hit ";
						dtlb.incRefCounter(DTLB_result.at(0),DTLB_result.at(1));    
						PFN = dtlb.retrievePFN(DTLB_result.at(0), DTLB_result.at(1));
						newAddr = Page_Table.virtualToPhysical(newAddr, PFN, options.page_offset_bits_num);
					}
				}
				//If DTLB is not enabled
				else if(!options.dtlb_enabled)                                    
				{
					pt_refs++;
					//If Valid PTE, PT hit
					if(Page_Table.isValid(VPN))                                  
					{
						PFN = Page_Table.getPFN(VPN);                            

						newAddr = Page_Table.virtualToPhysical(newAddr, PFN, options.page_offset_bits_num);

						pt_result = "hit ";
						pt_hits++;                                               
						Page_Table.incRefCounter(VPN);                           
					}
					else                                                         
					{
						//PT miss
						disk_refs++;                                             
						pt_result = "miss";                                      
						pt_faults++;                                             

						std::vector<std::string> assignResult;                   
						assignResult.resize(2);

						assignResult = Page_Table.isFull(options.virtual_page_num, options.physical_page_num);
						PFN = std::stoi(assignResult.at(1));                     
						//If PT full, evict page
						if(assignResult.at(0) == "1")                            
						{
							page_evictions++;
							//If wb_wa
							if(!options.DC_wt_nwa)
							{
								invalidate_WB(DC_Cache, DC_rec, Page_Table, PFN, main_mem_refs);
								if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
								{
									disk_refs++;                                 
								}                                                
							}
							//If wt_nwa
							else if(options.DC_wt_nwa)
							{
								invalidate_WT(DC_Cache, DC_rec, Page_Table, PFN, main_mem_refs);
								if(Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, true))
								{
									disk_refs++;                                 
								}                                                
							}
						}
						//If PT not full, assign frame
						else if(assignResult.at(0) == "0")                       
						{
							Page_Table.assignToPhysFrame(options.virtual_page_num, VPN, PFN, false);
						}
						newAddr = Page_Table.virtualToPhysical(newAddr, PFN, options.page_offset_bits_num);
					}
				}
			}
			//If reading
			if(R_W == 'R')
			{
				read_total++;
				std::vector<int> DC_result;                                      
				DC_result.resize(2);                                             
				DC_result = DC_Cache.getAddrTagIndex(newAddr);                    
				bool DC_found = DC_Cache.check_match(false, DC_result.at(0), DC_result.at(1));
				//If DC hit
				if(DC_found)                                                     
				{
					DC_hits++;                                                   
					DC_Cache.incRefCounter(DC_result.at(0), DC_result.at(1));    
					//If using virtual addressing
					if(options.virtual_addresses)
					{
						//If DTLB enabled, show line
						if(options.dtlb_enabled)
						{
							showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
									DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
									DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
						}
						//IF DTLB disabled, show line
						else if(!options.dtlb_enabled)
						{
							showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1, "",
									pt_result, PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
						}
					}
					//If using physical addresses, show line
					else if(!options.virtual_addresses)
					{
						showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
								PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
					}
				}
				//If DC miss
				else if(!DC_found)
				{
					DC_misses++;
					main_mem_refs++;
					std::vector<std::string> DC_alloc_result;                    
					DC_alloc_result.resize(8);                                   
					DC_alloc_result = DC_Cache.allocate(false, newAddr, DC_result.at(0), DC_result.at(1), PFN, VPN);

					size = DC_rec.at(PFN).size();                                

					DC_rec.at(PFN).resize(size + 1);                             
					size = DC_rec.at(PFN).size();                                
					DC_rec.at(PFN).at(size - 1).tag = DC_result.at(0);           
					DC_rec.at(PFN).at(size - 1).index = DC_result.at(1);         
					DC_rec.at(PFN).at(size - 1).addr = newAddr;                  
					
					//If an eviction happened
					if(DC_alloc_result.at(0) == "y")
					{
						size = DC_rec.at(std::stoi(DC_alloc_result.at(4))).size();

						for(int i = 0; i < size; i++)                            
						{
							if(DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).tag ==
									std::stoi(DC_alloc_result.at(5)) &&
									DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).index ==
									std::stoi(DC_alloc_result.at(6)))
							{
								DC_rec.at(std::stoi(DC_alloc_result.at(4))).erase(DC_rec.at
										(std::stoi(DC_alloc_result.at(4))).begin() + i);
								break;                                           
							}
						}
					}
					//If an eviction happened and DC has wb_wa and if the line was dirty
					if(DC_alloc_result.at(0) == "y" && !DC_Cache.get_wt_nwa() && DC_alloc_result.at(2) == "y")
					{
						main_mem_refs++;                                         
						if(options.virtual_addresses)
						{                                                        
							Page_Table.setPageDirty(std::stoi(DC_alloc_result.at(3)));
						}                                                        
					}
					//If using virtual addresses
					if(options.virtual_addresses)
					{
						//If DTLB enabled, showline
						if(options.dtlb_enabled)
						{
							showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
									DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
									DC_result.at(0), DC_result.at(1), "miss", -1, -1, "");
						}
						//IF DTLB disabled, showline
						else if(!options.dtlb_enabled)
						{
							showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1, "",
									pt_result, PFN, DC_result.at(0), DC_result.at(1), "miss", -1, -1, "");
						}
					}
					//If using physical addresses
					else if(!options.virtual_addresses)
					{
						showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
								PFN, DC_result.at(0), DC_result.at(1), "miss", -1, -1, "");
					}
				}
			}
			//If writing
			else if(R_W == 'W')
			{
				write_total++;
				std::vector<int> DC_result;                                      
				DC_result.resize(2);                                             
				DC_result = DC_Cache.getAddrTagIndex(newAddr);                   
				bool DC_found = false;
				//If using wb_wa
				if(!DC_Cache.get_wt_nwa())                                       
				{
					DC_found = DC_Cache.check_match(true, DC_result.at(0), DC_result.at(1));
				}
				//If using wt_nwa
				else if(DC_Cache.get_wt_nwa())                                   
				{
					DC_found = DC_Cache.check_match(false, DC_result.at(0), DC_result.at(1));
				}                                                                
				
				//If DC hit
				if(DC_found)
				{
					DC_hits++;                                                   
					DC_Cache.incRefCounter(DC_result.at(0), DC_result.at(1));    
					
					//If using virtual addresses
					if(options.virtual_addresses)
					{
						//If DTLB is enabled, showline
						if(options.dtlb_enabled)
						{
							showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
									DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
									DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
						}
						//If DTLB is disabled, showline
						else if(!options.dtlb_enabled)
						{
							showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1, "",
									pt_result, PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
						}
					}
					//If using physical addresses
					else if(!options.virtual_addresses)
					{
						showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
								PFN, DC_result.at(0), DC_result.at(1), "hit ", -1, -1, "");
					}
				}
				//If DC miss
				else if(!DC_found)
				{
					DC_misses++;
					main_mem_refs++;                                             
					
					//If wb_wa
					if(!DC_Cache.get_wt_nwa())                                   
					{
						std::vector<std::string> DC_alloc_result;                
						DC_alloc_result.resize(8);                               
						DC_alloc_result = DC_Cache.allocate(true, newAddr, DC_result.at(0), DC_result.at(1), PFN, VPN);

						size = DC_rec.at(PFN).size();                            

						DC_rec.at(PFN).resize(size + 1);                         
						size = DC_rec.at(PFN).size();                            
						DC_rec.at(PFN).at(size - 1).tag = DC_result.at(0);       
						DC_rec.at(PFN).at(size - 1).index = DC_result.at(1);     
						DC_rec.at(PFN).at(size - 1).addr = newAddr;              
						
						//If a DC eviction happened
						if(DC_alloc_result.at(0) == "y")
						{
							size = DC_rec.at(std::stoi(DC_alloc_result.at(4))).size();

							for(int i = 0; i < size; i++)                        
							{
								if(DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).tag ==
										std::stoi(DC_alloc_result.at(5)) &&
										DC_rec.at(std::stoi(DC_alloc_result.at(4))).at(i).index ==
										std::stoi(DC_alloc_result.at(6)))
								{
									DC_rec.at(std::stoi(DC_alloc_result.at(4))).erase(DC_rec.at
											(std::stoi(DC_alloc_result.at(4))).begin() + i);
									break;                                       
								}
							}
						}
						//If a DC eviction happend and using wb_wa and the line was dirty
						if(DC_alloc_result.at(0) == "y" && !DC_Cache.get_wt_nwa() && DC_alloc_result.at(2) == "y")
						{
							main_mem_refs++;                                     
							if(options.virtual_addresses)
							{                                                    
								Page_Table.setPageDirty(std::stoi(DC_alloc_result.at(3)));
							}                                                    
						}
					}
					//If DC is wt_nwa
					if(DC_Cache.get_wt_nwa())                                    
					{                                                            
						if(options.virtual_addresses)
						{
							Page_Table.setPageDirty(VPN);                        
						}
					}
					//If using virtual addressee
					if(options.virtual_addresses)
					{
						//If DTLB is enabled, showline
						if(options.dtlb_enabled)
						{
							showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options),
									DTLB_result.at(0), DTLB_result.at(1), dtlb_result, pt_result, PFN,
									DC_result.at(0), DC_result.at(1), "miss", -1, -1, "");
						}
						//If DTLB is disabled, showline
						else if(!options.dtlb_enabled)
						{
							showLine(options, newAddrDisp, VPN, getPageOffset(newAddr, options), -1, -1, "",
									pt_result, PFN, DC_result.at(0), DC_result.at(1), "miss", -1, -1, "");
						}
					}
					//If using physical addresses, showline
					else if(!options.virtual_addresses)
					{
						showLine(options, newAddrDisp, -1, getPageOffset(newAddr, options), -1, -1, "", "",
								PFN, DC_result.at(0), DC_result.at(1), "miss", -1, -1, "");
					}
				}
			}
		}
	}

	//Show simulation statistics
	std::cout << std::endl;
	std::cout << "Simulation statistics" << std::endl;
	std::cout << std::endl;

	if(options.dtlb_enabled)
	{
		std::cout << "dtlb hits        : " << (int)dtlb_hits <<  std::endl;
		std::cout << "dtlb misses      : " << (int)dtlb_misses <<  std::endl;
		std::cout << "dtlb hit ratio   : " << (float)(dtlb_hits / (dtlb_hits+dtlb_misses)) <<  std::endl << std::endl;
	}
	else{
		std::cout << "dtlb hits        : " << 0 <<  std::endl;
		std::cout << "dtlb misses      : " << 0 <<  std::endl;
		std::cout << "dtlb hit ratio   : N/A"  <<  std::endl << std::endl;
	}

	if(options.virtual_addresses){
		std::cout << "pt hits          : " << (int)pt_hits <<  std::endl;
		std::cout << "pt faults        : " << (int)pt_faults <<  std::endl;
		std::cout << "pt hit ratio     : " << (float)(pt_hits / (pt_hits+pt_faults)) <<  std::endl << std::endl;
	}
	else{
		std::cout << "pt hits          : " << 0 <<  std::endl;
		std::cout << "pt faults        : " << 0 <<  std::endl;
		std::cout << "pt hit ratio     : N/A" << std::endl << std::endl;
	}

	std::cout << "dc hits          : " << (int)DC_hits <<  std::endl;
	std::cout << "dc misses        : " << (int)DC_misses <<  std::endl;
	std::cout << "dc hit ratio     : " << (float)(DC_hits / (DC_hits+DC_misses)) <<  std::endl << std::endl;

	std::cout << "L2 hits          : " << 0 <<  std::endl;
	std::cout << "L2 misses        : " << 0 <<  std::endl;
	std::cout << "L2 hit ratio     : N/A" <<  std::endl << std::endl;

	std::cout << "Total reads      : " << (int)read_total << std::endl;
	std::cout << "Total writes     : " << (int)write_total << std::endl;
	std::cout << "Ratio of reads   : " << (float)(read_total / (read_total+write_total)) << std::endl << std::endl;

	std::cout << "main memory refs : " << (int)main_mem_refs << std::endl;
	std::cout << "page table refs  : " << (int)pt_refs << std::endl;
	std::cout << "disk refs        : " << (int)disk_refs << std::endl;


	fd.close();                                  
}

//This function finds the page offset from a given address and config options
int getPageOffset(std::string addr, Config options)
{
	std::string addr_hex = "0x" + addr;                              

	std::stringstream ss;
	ss << std::hex << addr_hex;                                      

	unsigned long address;                                           
	ss >> address;                                                   

	std::string mask = "";                                           

	for(int i = 0; i < options.page_offset_bits_num; i++)
	{
		mask.append("1");                                            
	}                                                                

	std::bitset<32> original_bits(address);                          
	std::bitset<32> offset_mask_bits(mask);                          

	auto offset_bits = (original_bits & offset_mask_bits);           

	return (int)offset_bits.to_ulong();                              
}

//This function finds the virtual page from the given virtual address and config options
int getVirtualPage(std::string addr, Config options)
{
	std::string addr_hex = "0x" + addr;                              

	std::stringstream ss;
	ss << std::hex << addr_hex;                                      

	unsigned long address;                                           
	ss >> address;                                                   

	std::string mask = "";                                           

	for(int i = 0; i < options.virtual_page_bits_num; i++)
	{
		mask.append("1");                                            
	}                                                                

	for(int i = 0; i < options.page_offset_bits_num; i++)
	{
		mask.append("0");                                            
	}                                                                

	std::bitset<32> original_bits(address);                          
	std::bitset<32> index_mask_bits(mask);                           

	auto VPN_bits = (original_bits & index_mask_bits);               
	VPN_bits >>= options.page_offset_bits_num;                       

	return (int)VPN_bits.to_ulong();                                 

}

//This function invalidates if DC is write back and L2 is write back
void invalidate_WB_WB(DC& DC_Cache, L2& L2_Cache, std::vector<std::vector<CacheRecord>>& DC_rec,
		std::vector<std::vector<CacheRecord>>& L2_rec, PageTable& PT, int PFN, float& main_mem_refs)
{
	int size = DC_rec.at(PFN).size();
	for(int i = 0; i < size; i++)
	{
		if(DC_Cache.check_dirty(DC_rec.at(PFN).at(i).tag, DC_rec.at(PFN).at(i).index))
		{
			std::vector<int> L2_result;                                              
			L2_result.resize(2);                                                     
			L2_result = L2_Cache.getAddrTagIndex(DC_rec.at(PFN).at(i).addr);         
			L2_Cache.check_match(true, L2_result.at(0), L2_result.at(1));               
		}                                                                            
	}

	DC_rec.at(PFN).resize(0);                                                        
	DC_Cache.invalidateByPFN(PFN);                                                   

	size = L2_rec.at(PFN).size();
	bool dirty = false;                                                              
	for(int i = 0; i < size; i++)
	{
		if(L2_Cache.check_dirty(L2_rec.at(PFN).at(i).tag, L2_rec.at(PFN).at(i).index))
		{                                                                            
			main_mem_refs++;                                                         
			dirty = true;                                                            
		}
	}
	L2_rec.at(PFN).resize(0);                                                        
	L2_Cache.invalidateByPFN(PFN);                                                   
	if(dirty)                                                                        
	{
		PT.setPageDirtyByPFN(PFN);                                                   
	}
}

//This function invalidates if DC is write through and L2 is write through
void invalidate_WT_WT(DC& DC_Cache, L2& L2_Cache, std::vector<std::vector<CacheRecord>>& DC_rec,
		std::vector<std::vector<CacheRecord>>& L2_rec, PageTable& PT, int PFN, float& main_mem_refs)
{
	DC_Cache.invalidateByPFN(PFN);                                                   
	DC_rec.at(PFN).resize(0);                                                        
	L2_Cache.invalidateByPFN(PFN);                                                   
	L2_rec.at(PFN).resize(0);                                                        
}

//This function invalidates if DC is write back and L2 is write through
void invalidate_WB_WT(DC& DC_Cache, L2& L2_Cache, std::vector<std::vector<CacheRecord>>& DC_rec,
		std::vector<std::vector<CacheRecord>>& L2_rec, PageTable& PT, int PFN, float& main_mem_refs)
{
	int size = DC_rec.at(PFN).size();
	bool dirty = false;                                                              
	for(int i = 0; i < size; i++)
	{
		if(DC_Cache.check_dirty(DC_rec.at(PFN).at(i).tag, DC_rec.at(PFN).at(i).index))
		{
			dirty = true;
			std::vector<int> L2_result;                                              
			L2_result.resize(2);                                                     
			L2_result = L2_Cache.getAddrTagIndex(DC_rec.at(PFN).at(i).addr);         
			bool found = L2_Cache.check_match(false, L2_result.at(0), L2_result.at(1));

			if(found)                                                                
			{
				main_mem_refs++;                                                     
			}                                                                        
		}
	}
	DC_rec.at(PFN).resize(0);                                                        
	DC_Cache.invalidateByPFN(PFN);                                                   
	L2_rec.at(PFN).resize(0);                                                        
	L2_Cache.invalidateByPFN(PFN);                                                   
	if(dirty)                                                                        
	{
		PT.setPageDirtyByPFN(PFN);                                                   
	}
}

//This function invalidates if L2 is write through and L2 is write back
void invalidate_WT_WB(DC& DC_Cache, L2& L2_Cache, std::vector<std::vector<CacheRecord>>& DC_rec,
		std::vector<std::vector<CacheRecord>>& L2_rec, PageTable& PT, int PFN, float& main_mem_refs)
{
	DC_rec.at(PFN).resize(0);                                                        
	DC_Cache.invalidateByPFN(PFN);                                                   

	int size = L2_rec.at(PFN).size();
	bool dirty = false;                                                              
	for(int i = 0; i < size; i++)
	{
		if(L2_Cache.check_dirty(L2_rec.at(PFN).at(i).tag, L2_rec.at(PFN).at(i).index))
		{                                                                            
			main_mem_refs++;                                                         
			dirty = true;                                                            
		}
	}
	L2_rec.at(PFN).resize(0);                                                        
	L2_Cache.invalidateByPFN(PFN);                                                   
	if(dirty)                                                                        
	{
		PT.setPageDirtyByPFN(PFN);                                                   
	}
}

//This function invalidates if DC is write back and L2 is disabled
void invalidate_WB(DC& DC_Cache, std::vector<std::vector<CacheRecord>>& DC_rec, PageTable& PT, int PFN,
		float& main_mem_refs)
{
	bool dirty = false;
	int size = DC_rec.at(PFN).size();
	for(int i = 0; i < size; i++)
	{
		if(DC_Cache.check_dirty(DC_rec.at(PFN).at(i).tag, DC_rec.at(PFN).at(i).index))
		{
			dirty = true;
			main_mem_refs++;
		}
	}
	DC_rec.at(PFN).resize(0);                                                        
	DC_Cache.invalidateByPFN(PFN);                                                   
	if(dirty)                                                                        
	{
		PT.setPageDirtyByPFN(PFN);                                                   
	}
}

//This function invalidates if DC is write through and L2 is disabled
void invalidate_WT(DC& DC_Cache, std::vector<std::vector<CacheRecord>>& DC_rec, PageTable& PT, int PFN,
		float& main_mem_refs)
{
	DC_rec.at(PFN).resize(0);                                                        
	DC_Cache.invalidateByPFN(PFN);                                                   
}
