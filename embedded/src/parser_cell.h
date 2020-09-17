#ifndef EMBEDDED_EX3_PARSER_H
#define EMBEDDED_EX3_PARSER_H

#include <stdbool.h>
#include "cellular.h"

bool parse_reg_status(char* modem_output, int* status);
bool parse_sig_quality(char* modem_output, int* csq);
bool extract_single_operator_info(OPERATOR_INFO* operator, int op_idx, char* buf);
void get_cell_str(OPERATOR_INFO* operator, char* buf);

#endif //EMBEDDED_EX3_PARSER_H
