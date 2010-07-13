/* Interface for CAEN V1718 VME - USB 2.0 Bridge */

#ifndef BRIDGEV1718_H 
#define BRIDGEV1718_H

#include "CAENVMElib.h"
#include <iostream>

/*! \class Bridge
 * \brief class defining the VME bridge
 *  This class initializes the CAEN V1718 VME bridge in order to control the crate.
 */
class bridgeV1718 {
    public:
        /* device : /dev/xxx */
        /*!
         *  \brief Constructor
         *
         *  Bridge class constructor
         *  \param[in] Device identifier on the VME crate
         */
        bridgeV1718(const char *device);
        /*!
         *  \brief Destructor
         *
         *  Bridge class destructor
         */
        ~bridgeV1718();
        /*!
         *  \brief Gets bhandle
         *
         *  Gives bhandle value
         *  \return bhandle value
         */ 
        int32_t getBHandle();
	
		/*
			FIXME: Make an object ? (
				output_t out1 = new output(0)
				out1->on()
				out1->off()
		*/
		int outputConf(CVOutputSelect output);
		int outputOn(CVOutputSelect output);
		int outputOff(CVOutputSelect output);

    private:
       
	int32_t bhandle; /*!< Device handle */
};

#endif /* BRIDGEV1718_H */
