/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "using_sam3n.h"
#include "cmsis_os.h"
/*
 * main: initialize and start the system
 */
 
int main (void) {
	// initialize CMSIS-RTOS
  osKernelInitialize ();
  // initialize peripherals here
	device_init();
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	begin();
	// start thread execution
	osKernelStart ();
}
