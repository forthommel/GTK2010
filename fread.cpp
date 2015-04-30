#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef enum {
  global_header = 0x8,
  tdc_header = 0x1,
  tdc_measur = 0x0,
  tdc_trailer = 0x3,
  tdc_error = 0x4,
  ettt = 0x11,
  global_trailer = 0x10,
  filler = 0x18,
} word_type;
uint32_t event_nb;
uint32_t hit_nb[16]; // 16 channels
void wordDisplay(uint32_t word);

int main(int argc, char *argv[]) {
  event_nb = 0;
  for(int i=0;i<16;i++) hit_nb[i] = 0;
	if (argc != 2) {
		std::cerr << "Usage: "<< argv[0] << " <filename>" << std::endl;
		return -1;
	}
	
	struct file_header_t {
		uint32_t magic;
		uint32_t run_id;
		uint32_t spill_id;
		//uint32_t buffer_size; //bytes
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
	int data_payload_size = s.st_size - sizeof(file_header_t);

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
	
	std::cout << "===== Header =====" << std::endl;
	std::cout << "Run ID: " << fh.run_id << std::endl;	
	std::cout << "Spill ID: " << fh.spill_id << std::endl;
	std::cout << "==================" << std::endl;
	int block_nb = data_payload_size/sizeof(uint32_t); 
	uint32_t *buffer = new uint32_t[data_payload_size];
	if(input_file.good()) {
		input_file.read((char*)buffer,data_payload_size);
	} else {
		std::cerr << std::endl;
		input_file.close();
		return -1;
	}
	input_file.close();
	
	//
	for(int i = 0; i < block_nb; i++) {
	  //std::cout << "buffer pos " << i << std::endl;
		wordDisplay(buffer[i]);
	}
	//
	
  std::cout << "Total in file: " << event_nb << " events" << std::endl;
  for (int i=0;i<16;i++) if (hit_nb[i]!=0) std::cout << hit_nb[i] << " hits in channel " << i << std::endl;
	
	delete [] buffer;
	return 0;
}

void wordDisplay(uint32_t word) {
  int id_word = word >> 27;
  //std::cout << "id_word: " << id_word << std::endl;
  switch((word_type)id_word) {
    case global_header: event_nb++; break;
    case tdc_measur: if (((word&0x4000000) >> 26)==0) hit_nb[(word&0x3e00000) >> 21]++; break; // Only the leading edge
  }
  
  /*bool detailed_display=false;
  switch((word_type)id_word) {
    case global_header: //01000
      event_nb++;
      if(detailed_display) std::cout << "[ event count: " << ((word&0x7FFFFE0) >> 5) << std::endl;
      else std::cout << "[";
     break;
    case tdc_header: // 00001
      if(detailed_display) std::cout << "\t ( event id:" << (word&0xfff000 >> 12) << " bunch id (trigger time tag): " << (word&0xfff) << std::endl;
      else //std::cout << "(";
      break;
    case tdc_measur: //00000
      hit_nb[(word&0x3e00000) >> 21]++;
      if(detailed_display) { 
        std::cout << "\t\t * channel: " << std::setw(2) << ((word&0x3e00000) >> 21) << " measurement: " << std::setw(8) << (word&0x1fffff) << " ";
        switch((word&0x4000000) >> 26) {
          case 1: // TRAILING
            std::cout << "\e[1;31mTRAILING\e[0m";
            break;
          case 0: // LEADING
            std::cout << "\e[0;32m LEADING\e[0m";
            break;
        }
        std::cout << std::endl;
      }
      else {
        switch((word&0x4000000) >> 26) {
        case 1: // TRAILING
          //std::cout << "\e[1;31mT\e[0m"; // BASH colour display
          std::cout << "T";
          break;
        case 0: // LEADING
          //std::cout << "\e[0;32mL\e[0m"; // BASH colour display
          std::cout << "L";
          break;
        }
      }
      break;
    case tdc_trailer: //00011
      if(detailed_display) std::cout << "\tevent id: " << (word&0xfff000 >> 12) << " word count: " << (word&0xfff) << " )" << std::endl;
      else std::cout << ")";
      break;
    case tdc_error: // 00100
      if(!detailed_display) std::cout << "!";
      break;
    case ettt: // 10001
      break;
    case global_trailer:
      if(detailed_display) //std::cout << "word count: " << ((word&0x1FFFE0) >> 5) << " ]" << std::endl;
      else std::cout << "]" << std::endl;
    break;
    case filler: // 11000
       if(!detailed_display) //std::cout << "~";
      break;
    default:
      break;
  }*/
}