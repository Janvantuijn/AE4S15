
/* Define whether this build is for a satellite or ground station */
#define GROUNDSTATION
//#define SATELLITE


#ifdef GROUNDSTATION
	#define I2C_MASTER
#else
	#define I2C_SLAVE
#endif
