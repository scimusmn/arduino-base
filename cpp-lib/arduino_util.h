#include <libserialport.h>

#ifndef INCLUDE_ARDUINO_UTIL_H
#define INCLUDE_ARDUINO_UTIL_H

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum sp_return show_config(struct sp_port* port);
enum sp_return set_arduino_config(struct sp_port* port, int baudrate);
enum sp_return enumerate_ports(struct sp_port*** ports, int* n_ports); 
void print_error(enum sp_return err);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#endif
