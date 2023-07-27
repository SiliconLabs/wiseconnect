#include "console_types.h"
#include "console_constants.h"
#include "nvm_ids.h"


extern void*  si91x_init_configuration;
extern const structure_descriptor_entry_t rs91x_chip_config[];

const unsigned int first_group_end=1-1;
const console_variable_node_t nodes[] = {

  [0] = 
SL_CONSOLE_VARIABLE_GROUP("wifi", 1, 1),

  [1] = 
SL_CONSOLE_VARIABLE_RAM_STRUCTURE("config", rs91x_chip_config, 10, &si91x_init_configuration),
};
