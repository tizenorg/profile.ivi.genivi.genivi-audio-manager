/*
Copyright (c) 2014, Intel Corporation

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

#include <murphy/common.h>

#include "ini-parser.h"


int initialize_parser_buffer(ini_parser_context_t *ctx)
{
    /* buffers points to the array of buffers. End points to the last
     * character position in the buffer, and current points to the
     * buffer position that will be written next. */

    int i;

    for (i = 0; i < 10; i++) {
        ctx->buffers[i] = NULL;
    }

    ctx->buffers[0] = mrp_alloc(CHARACTER_BUFFER_SIZE * sizeof(char));
    if (!ctx->buffers[0]) {
        return -1;
    }

    ctx->end = ctx->buffers[0] + CHARACTER_BUFFER_SIZE-1;
    ctx->current = ctx->buffers[0];
    ctx->buffer_i = 0;

    ctx->error = 0;

    return 0;
}


char *new_parser_str(ini_parser_context_t *ctx, char *input)
{
    int len = strlen(input);
    char *newend, *p;

    if (ctx->buffers[ctx->buffer_i] == NULL) {
        return NULL;
    }

    /* newend points to the last character position required for the
     * input */

    newend = ctx->current + len;

    if (newend > ctx->end) {
        /* OOM, let's allocate a new buffer */
        if (++(ctx->buffer_i) >= MAX_BUFFERS) {
            return NULL;
        }

        ctx->buffers[ctx->buffer_i] = mrp_alloc(CHARACTER_BUFFER_SIZE * sizeof(char));
        if (ctx->buffers[ctx->buffer_i] == NULL) {
            return NULL;
        }
        ctx->end = ctx->buffers[ctx->buffer_i] + CHARACTER_BUFFER_SIZE-1;
        ctx->current = ctx->buffers[ctx->buffer_i];

        newend = ctx->current + len;

        if (newend > ctx->end) {
            /* input is longer than the MAX buffer size */
            return NULL;
        }
    }

    strncpy(ctx->current, input, len);

    p = ctx->current;
    *newend = '\0';
    ctx->current = newend + 1;

    return p;
}


void delete_parser_buffer(ini_parser_context_t *ctx)
{
    int i;

    for (i = 0; i < MAX_BUFFERS; i++) {
        mrp_free(ctx->buffers[i]);
        ctx->buffers[i] = NULL;
    }
    ctx->buffer_i = 0;
    ctx->current = NULL;
    ctx->end = NULL;
}


void init_result(ini_parser_result_t *result)
{
    /* printf("> init_result (%p)\n", result); */
    mrp_list_init(&result->sections);
    result->current = NULL;
}


void add_section(ini_parser_result_t *result, const char *name)
{
    ini_parser_section_t *section;

    if (!result)
        goto error;

    section = mrp_allocz(sizeof(ini_parser_section_t));
    if (!section)
        goto error;

    section->name = strdup(name);
    if (!section->name)
        goto error;

    mrp_list_init(&section->pairs);
    mrp_list_init(&section->hook);
    mrp_list_append(&result->sections, &section->hook);

    result->current = section;

    return;

error:
    mrp_log_error("add_section ERROR (%s)", name);
    return;
}


void add_key(ini_parser_result_t *result, const char *key)
{
    ini_parser_section_t *section;
    ini_parser_keyvaluepair_t *pair;

    if (!result)
        goto error;

    section = result->current;

    if (!section || section->current) {
        mrp_log_error("add_key section error %p", section);
        goto error;
    }

    pair = mrp_allocz(sizeof(ini_parser_keyvaluepair_t));
    if (!pair)
        goto error;

    mrp_list_init(&pair->hook);
    mrp_list_append(&section->pairs, &pair->hook);

    pair->key = mrp_strdup(key);
    if (!pair->key)
        goto error;

    section->current = pair;

    return;

error:
    mrp_log_error("add_key ERROR (%s)", key);
    return;
}


void add_modifier(ini_parser_result_t *result, const char *modifier)
{
    ini_parser_section_t *section;
    ini_parser_keyvaluepair_t *pair;

    /* printf("> add_modifier (%s)\n", modifier); */

    if (!result)
        goto error;

    section = result->current;
    if (!section)
        goto error;

    pair = section->current;
    if (!pair)
        goto error;

    pair->modifier = mrp_strdup(modifier);
    if (!pair->modifier)
        goto error;

    return;

error:
    mrp_log_error("add_modifier ERROR (%s)", modifier);
    return;
}


void add_value(ini_parser_result_t *result, const char *value)
{
    ini_parser_section_t *section;
    ini_parser_keyvaluepair_t *pair;

    /* printf("> add_value (%s)\n", value); */

    if (!result)
        goto error;

    section = result->current;
    if (!section)
        goto error;

    pair = section->current;
    if (!pair)
        goto error;

    pair->value = mrp_strdup(value);
    if (!pair->value)
        goto error;

    section->current = NULL;

    return;

error:
    mrp_log_error("add_value ERROR (%s)", value);
    return;
}


void deinit_result(ini_parser_result_t *result)
{
    mrp_list_hook_t *sp, *sn, *kp, *kn;

    if (!result)
        return;

    mrp_list_foreach(&result->sections, sp, sn) {
        ini_parser_section_t *section;

        section = mrp_list_entry(sp, typeof(*section), hook);
        mrp_list_delete(&section->hook);

        mrp_list_foreach(&section->pairs, kp, kn) {
            ini_parser_keyvaluepair_t *pair;

            pair = mrp_list_entry(kp, typeof(*pair), hook);
            mrp_list_delete(&pair->hook);

            mrp_free(pair->key);
            mrp_free(pair->modifier);
            mrp_free(pair->value);
            mrp_free(pair);
        }

        mrp_free(section->name);
        mrp_free(section);
    }

    return;
}


static void print_pair(ini_parser_keyvaluepair_t *pair)
{
    mrp_debug("%s[%s]=%s", pair->key, pair->modifier ? pair->modifier : "",
        pair->value);
}


void print_result(ini_parser_result_t *result)
{
    mrp_list_hook_t *sp, *sn, *kp, *kn;

    if (!result)
        return;

    mrp_list_foreach(&result->sections, sp, sn) {

        ini_parser_section_t *section;

        section = mrp_list_entry(sp, typeof(*section), hook);

        mrp_debug("[%s]", section->name);

        mrp_list_foreach(&section->pairs, kp, kn) {
            ini_parser_keyvaluepair_t *pair;

            pair = mrp_list_entry(kp, typeof(*pair), hook);
            print_pair(pair);
        }
        mrp_debug("");
    }

    return;
}
