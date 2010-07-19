//Trailer ???

#include <iostream>
#include <string>
#include <sstream>
#include <cstdint>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	int fd = -1;
	int spill = 0;
	if(argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <prefix>_[spill_id].gtk" << std::endl;
		return -1;
	}
	std::string seed(argv[1]);

	struct file_header_t {
		uint32_t magic;
		uint32_t run_id;
		uint32_t spill_id;
	};
	
	//Dummy hit block of data
	struct dummy_hit_t {
		uint16_t lsb;
		uint16_t msb;
	};
	
	int spill_nbr = 1000;


	for(int j= 0; j < spill_nbr; j++) {
		//----
		dummy_hit_t hit;
		hit.lsb = 0xffff;
		hit.msb = 0x000f;
		//1048575
		
		//File header
		file_header_t fh;
		fh.magic = 0x47544B30; //ASCII: GTK0 
		fh.run_id = 0;
		fh.spill_id = spill;
		fh.hit_count = 1024;
	
		std::string filename = seed;
 		std::ostringstream out;
 		out << "_" << spill;
		filename.append(out.str()+".gtk");
		fd = open(filename.c_str(),O_CREAT|O_WRONLY,S_IRUSR);
		if(fd == -1) {
			std::cerr << argv[0] <<" error: Failed to create " << filename << std::endl; 
			perror(NULL);
			return -1;				
		}
		write(fd,&fh,sizeof(file_header_t));
		int ret;
		int i;
		for(i=0; i < 1024; i++) { //1024 hit per spill
			ret = write(fd,&hit,sizeof(dummy_hit_t));		
		
		if(ret < 0) {
			std::cerr << argv[0] <<" error: Failed to write hit " << i << " (spill " << spill << ")"<< std::endl;
			close(fd);	
			return -1;
	}
	}
			close(fd);
			spill++;
	}
	return 0;
}
