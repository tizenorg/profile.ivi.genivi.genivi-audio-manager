/*
Copyright (c) 2012, Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.

    * Neither the name of Intel Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __MURPHY_PLUGIN_UDEV_INI_PARSER_H__
#define __MURPHY_PLUGIN_UDEV_INI_PARSER_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <murphy/common.h>

#define CHARACTER_BUFFER_SIZE 16384
#define MAX_BUFFERS 10

typedef struct {
    mrp_list_hook_t hook;
    char *modifier;
    char *key;
    char *value;
} ini_parser_keyvaluepair_t;

typedef struct {
    mrp_list_hook_t hook;

    char *name;
    /* list of key-value-pairs */
    mrp_list_hook_t pairs;

    ini_parser_keyvaluepair_t *current;
} ini_parser_section_t;

typedef struct {
    /* list of sections */
    mrp_list_hook_t sections;

    ini_parser_section_t *current;
} ini_parser_result_t;

typedef struct {
    char *buffers[MAX_BUFFERS];
    char *current;
    char *end;
    int buffer_i;
    void *user_data;
    int error;
    ini_parser_result_t *r;
} ini_parser_context_t;

char *new_parser_str(ini_parser_context_t *ctx, char *input);
int initialize_parser_buffer(ini_parser_context_t *ctx);
void delete_parser_buffer(ini_parser_context_t *ctx);

bool mrp_parse_ini_file(const char *filename, ini_parser_result_t *result);


void init_result(ini_parser_result_t *result);
void add_section(ini_parser_result_t *result, const char *name);
void add_key(ini_parser_result_t *result, const char *key);
void add_modifier(ini_parser_result_t *result, const char *modifier);
void add_value(ini_parser_result_t *result, const char *value);
void deinit_result(ini_parser_result_t *result);
void print_result(ini_parser_result_t *result);

#endif
