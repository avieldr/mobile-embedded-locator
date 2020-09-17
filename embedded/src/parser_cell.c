#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parser_cell.h"

bool parse_reg_status(char* modem_output, int* status)
{
    char* comm_prefix ="+CREG:";
    char *ptr_end;

    if (!modem_output)
    {
        return 0;
    }

    char* str = strstr(modem_output, comm_prefix);

    if (!str)
    {
        return 0;
    }

    str = strstr(str, ",");
    if (!str)
    {
        return 0;
    }

    char c_dig = str[1];
    int i_dig = c_dig - '0';
    if (i_dig < 0 || i_dig > 5)
        return 0;
    *status = i_dig;
    return 1;
}

bool parse_sig_quality(char* modem_output, int* csq) {
    char *comm_prefix = "+CSQ:";

    if (!modem_output) {
        return 0;
    }

    char *str = strstr(modem_output, comm_prefix);

    if (!str) {
        return 0;
    }

    str = strstr(str, " ");
    if (!str) {
        return 0;
    }

    char dig_1 = str[1];
    char dig_2 = str[2];

    int d_dig_1 = dig_1 - '0';

    if (dig_2 == ',')
        *csq = (int) d_dig_1;
    else
    {
        int d_dig_2 = dig_2 - '0';
        int sig_q = 10 * d_dig_1 + d_dig_2;
        if (sig_q < 0 || sig_q > 31)
            return 0;
        *csq = sig_q;
    }
    return 1;
}

bool extract_single_operator_info(OPERATOR_INFO* operator, int op_idx, char* buf)
{
    if (!buf)
    {
        return 0;
    }
    int op_status;
    char* op_name_long;
    char* op_name_short;
    int op_code;
    int op_tech;

    char delim[] = ",";
    char *ptr = strtok(buf, delim);
    if (!ptr)
    {
        return 0;
    }
    if (strlen(ptr) >= 2)
    {
        op_status = ptr[1] - '0';
    }
    else {return 0;}
    ptr = strtok(NULL, delim);
    if (!ptr)
    {
        return 0;
    }
    if (strlen(ptr) >= 3)
    {
        ptr++;
        ptr[strlen(ptr) - 1] = 0;
        op_name_long = ptr;
    }
    else {op_name_long = "";}
    ptr = strtok(NULL, delim);
    if (!ptr)
    {
        return 0;
    }
    if (strlen(ptr) >= 3)
    {
        ptr++;
        ptr[strlen(ptr) - 1] = 0;
        int ptr_len = strlen(ptr);
        int op_obj_array_len = 20;
        for (int j = 0; j < op_obj_array_len; ++j)
        {
            if (j < ptr_len)
                operator[op_idx].operatorName[j] = ptr[j];
            else
                operator[op_idx].operatorName[j] = 0;
        }
    }
    else
    {
        operator[op_idx].operatorName[0] = '\"';
        operator[op_idx].operatorName[1] = '\"';
        operator[op_idx].operatorName[2] = 0;
    }
    ptr = strtok(NULL, delim);
    if (!ptr)
    {
        return 0;
    }
    if (strlen(ptr) >= 3)
    {
        ptr++;
        ptr[strlen(ptr) - 1] = 0;
        char* endPtr;
        op_code = (int) strtod(ptr, &endPtr);
        operator[op_idx].operatorCode = op_code;
    }
    else {return 0;}
    ptr = strtok(NULL, delim);
    if (!ptr)
    {
        return 0;
    }
    if (strlen(ptr) >= 1)
    {
        op_tech = (int) ptr[0] - '0';
        if (op_tech == 0)
        {
            operator[op_idx].accessTechnology[0] = '2';
            operator[op_idx].accessTechnology[1] = 'G';
            operator[op_idx].accessTechnology[2] = 0;
            operator[op_idx].accessTechnology[3] = 0;
            return 1;
        }
        else if (op_tech == 2)
        {
            operator[op_idx].accessTechnology[0] = '3';
            operator[op_idx].accessTechnology[1] = 'G';
            operator[op_idx].accessTechnology[2] = 0;
            operator[op_idx].accessTechnology[3] = 0;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}


