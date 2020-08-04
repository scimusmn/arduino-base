#include <libserialport.h>
#include <stdio.h>
#include "arduino_util.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum sp_return show_config(struct sp_port* port) {
  enum sp_return err;

  // load port configuration
  struct sp_port_config* config;
  sp_new_config(&config);
  sp_get_config(port, config);

  // get configuration variables
  int baudrate;
  int bits;
  enum sp_parity parity;
  int stop_bits;
  enum sp_rts rts;
  enum sp_cts cts;
  enum sp_dtr dtr;
  enum sp_dsr dsr;
  enum sp_xonxoff xon_xoff;

  err = sp_get_config_baudrate(config, &baudrate);
  if ( err != SP_OK ) {
    printf("error encountered in getting baudrate\n");
    print_error(err);
    return err;
  }
  
  sp_get_config_bits    (config, &bits);
  if ( err != SP_OK ) {
    printf("error encountered in getting data bits\n");
    print_error(err);
    return err;
  }
  
  sp_get_config_parity  (config, &parity);
  if ( err != SP_OK ) {
    printf("error encountered in getting parity\n");
    print_error(err);
    return err;
  }
  
  sp_get_config_stopbits(config, &stop_bits);
  if ( err != SP_OK ) {
    printf("error encountered in getting stop bits\n");
    print_error(err);
    return err;
  }
  
  sp_get_config_rts     (config, &rts);
  if ( err != SP_OK ) {
    printf("error encountered in getting RTS\n");
    print_error(err);
    return err;
  }
  
  sp_get_config_cts     (config, &cts);
  if ( err != SP_OK ) {
    printf("error encountered in getting CTS\n");
    print_error(err);
    return err;
  }
  
  sp_get_config_dtr     (config, &dtr);
  if ( err != SP_OK ) {
    printf("error encountered in getting DTR\n");
    print_error(err);
    return err;
  }
  
  sp_get_config_dsr     (config, &dsr);
  if ( err != SP_OK ) {
    printf("error encountered in getting DSR\n");
    print_error(err);
    return err;
  }
  
  sp_get_config_xon_xoff(config, &xon_xoff);
  if ( err != SP_OK ) {
    printf("error encountered in getting XON/XOFF\n");
    print_error(err);
    return err;
  }

  // print configuration variables
  printf("Baud: %d\n", baudrate);
  printf("Data bits: %d\n", bits);
  printf("Parity: ");
  switch(parity) {
  case SP_PARITY_INVALID:
    printf("INVALID\n");
    break;
  case SP_PARITY_NONE:
    printf("None\n");
    break;
  case SP_PARITY_ODD:
    printf("Odd\n");
    break;
  case SP_PARITY_EVEN:
    printf("Even\n");
    break;
  case SP_PARITY_MARK:
    printf("Mark\n");
    break;
  case SP_PARITY_SPACE:
    printf("Space\n");
    break;
  }

  printf("Stop bits: %d\n", stop_bits);

  printf("RTS: ");
  switch(rts) {
  case SP_RTS_INVALID:
    printf("INVALID\n");
    break;
  case SP_RTS_OFF:
    printf("Off\n");
    break;
  case SP_RTS_ON:
    printf("On\n");
    break;
  case SP_RTS_FLOW_CONTROL:
    printf("Flow control\n");
    break;
  }

  printf("CTS: ");
  switch(cts) {
  case SP_CTS_INVALID:
    printf("INVALID\n");
    break;
  case SP_CTS_IGNORE:
    printf("Ignore\n");
    break;
  case SP_CTS_FLOW_CONTROL:
    printf("Flow control\n");
    break;
  }

  printf("DTR: ");
  switch(dtr) {
  case SP_DTR_INVALID:
    printf("Invalid\n");
    break;
  case SP_DTR_OFF:
    printf("Off\n");
    break;
  case SP_DTR_ON:
    printf("On\n");
    break;
  case SP_DTR_FLOW_CONTROL:
    printf("Flow control\n");
    break;
  }

  printf("DSR: ");
  switch(dsr) {
  case SP_DSR_INVALID:
    printf("Invalid\n");
    break;
  case SP_DSR_IGNORE:
    printf("Ignore\n");
    break;
  case SP_DSR_FLOW_CONTROL:
    printf("Flow control\n");
    break;
  }

  printf("XON/XOFF: ");
  switch(xon_xoff) {
  case SP_XONXOFF_INVALID:
    printf("Invalid\n");
    break;
  case SP_XONXOFF_DISABLED:
    printf("Disabled\n");
    break;
  case SP_XONXOFF_IN:
    printf("In\n");
    break;
  case SP_XONXOFF_OUT:
    printf("Out\n");
    break;
  case SP_XONXOFF_INOUT:
    printf("In/Out\n");
    break;
  }

  fflush(stdout);
  
  sp_free_config(config);

  return SP_OK;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum sp_return set_arduino_config(struct sp_port* port, int baudrate) {
  enum sp_return err;
  err = sp_set_baudrate(port, baudrate);
  if ( err != SP_OK ) {
    printf("error encountered in setting baudrate\n");
    print_error(err);
    return err;
  }
  
  err = sp_set_bits(port, 8);
  if ( err != SP_OK ) {
    printf("error encountered in setting baudrate\n");
    print_error(err);
    return err;
  }
  
  err = sp_set_parity(port, SP_PARITY_NONE);
  if ( err != SP_OK ) {
    printf("error encountered in setting baudrate\n");
    print_error(err);
    return err;
  }
  
  err = sp_set_stopbits(port, 1);
  if ( err != SP_OK ) {
    printf("error encountered in setting baudrate\n");
    print_error(err);
    return err;
  }
  
  err = sp_set_rts(port, SP_RTS_ON);
  if ( err != SP_OK ) {
    printf("error encountered in setting baudrate\n");
    print_error(err);
    return err;
  }
  
  err = sp_set_cts(port, SP_CTS_IGNORE);
  if ( err != SP_OK ) {
    printf("error encountered in setting baudrate\n");
    print_error(err);
    return err;
  }
  
  err = sp_set_dtr(port, SP_DTR_ON);
  if ( err != SP_OK ) {
    printf("error encountered in setting baudrate\n");
    print_error(err);
    return err;
  }
  
  err = sp_set_dsr(port, SP_DSR_IGNORE);
  if ( err != SP_OK ) {
    printf("error encountered in setting baudrate\n");
    print_error(err);
    return err;
  }
  
  err = sp_set_xon_xoff(port, SP_XONXOFF_DISABLED);
  if ( err != SP_OK ) {
    printf("error encountered in setting baudrate\n");
    print_error(err);
    return err;
  }

  return SP_OK;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum sp_return enumerate_ports(struct sp_port*** ports, int* n_ports) {
    int n = 0;

  enum sp_return err = sp_list_ports(ports);
  for (; (*ports)[n]; n++);
  if (err != SP_OK) {
    printf("ERROR: could not get list of ports\n");
  }
  printf("\n");
  (*n_ports) = n;
  return err;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void print_error(enum sp_return err) {
  switch (err) {
  case SP_ERR_ARG:
    printf("ERROR: invalid arguments!\n");
    break;
  case SP_ERR_FAIL:
    printf("ERROR: system error occured!\n");
    break;
  case SP_ERR_MEM:
    printf("ERROR: memory allocation error\n");
    break;
  case SP_ERR_SUPP:
    printf("ERROR: the requested operation is not supported by this device\n");
    break;
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
