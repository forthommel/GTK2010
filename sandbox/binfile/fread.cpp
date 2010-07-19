#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: "<< argv[0] << " <filename>" << std::endl;
		return -1;
	}

	struct dummy_hit_t {
		uint16_t lsb;
		uint16_t msb;
	};

	struct file_header_t {
		uint32_t magic;
		uint32_t run_id;
		uint32_t spill_id;
		uint32_t hit_count;
	};

	struct stat s;

	std::ifstream input_file;
	//FIXME Need safety checks on argv[1]
	input_file.open(argv[1],std::ios::in | std::ios::binary);
	if(!input_file.is_open()) {
		std::cerr << argv[0] << ": error opening file " << argv[1] << std::endl;
		return -1;
	}
	//Get the file size
	if(stat(argv[1],&s) == -1) {
		std::cerr << argv[0] << ": error retrieving size of " << argv[1] << std::endl;
		input_file.close();
		return -1;
	}	
	unsigned int block_nbr = ((s.st_size-sizeof(file_header_t))/sizeof(dummy_hit_t));
	file_header_t fh;
	if(input_file.good()) {
		input_file.read((char*)&fh,sizeof(file_header_t));
	} else {
		std::cerr << argv[0] << ": error can not read file header" << std::endl;
		input_file.close();
		return -1;
	}	
	if(fh.magic != 0x47544B30) {
		std::cerr << "Wrong magic number !" << std::endl;
		input_file.close();
		return -1;
	}
	if(fh.hit_count != block_nbr) {
		std::cerr << "File size not consistent with the hit count !" << std::endl;
		input_file.close();
		return -1;
	}
	std::cout << "===== Header =====" << std::endl;
	std::cout << "Run ID: " << fh.run_id << std::endl;	
	std::cout << "Spill ID: " << fh.spill_id << std::endl;
	std::cout << "Hit count: " << fh.hit_count << std::endl;	
	std::cout << "==================" << std::endl;
	dummy_hit_t *buffer = new dummy_hit_t[block_nbr];
	while (input_file.good()) {
		input_file.read((char *)buffer, s.st_size);
	}
	input_file.close();
	//
	std::cout << "LSB: 0x" << std::hex << std::setw(4) << buffer[0].lsb << std::endl; 
	std::cout << "MSB: 0x" << std::hex << std::setw(4) <<buffer[0].msb << std::endl;
	//
	delete [] buffer;
	return 0;
}

