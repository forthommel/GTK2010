//! \file main.h
//! \brief main file for VME Process 
//! \author
//! \version
//! \date 15 july 2009

#include "VME.h"

int main() {
	VME *vme;
	vme = new VME();
	vme->Run();
	delete vme;
	return 0;
}
