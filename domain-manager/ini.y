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


%{

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <murphy/common/macros.h>
#include <murphy/common/log.h>

#include "ini-parser.h"

static void yyerror(ini_parser_context_t *ctx, void *yyscanner, const char *err);

/* this has always to be of type yyscan_t */
#define YYLEX_PARAM scanner
%}

%error-verbose

%pure-parser

%parse-param { ini_parser_context_t *ctx }
%parse-param { void *scanner }

%token OBRACKET
%token EBRACKET
%token EQUAL

%union {
    int number;
    char *string;
}

%token <string> DATA
%token <string> WORD
%token <string> STRING
%token <string> BOOLEAN
/* %token <number> NUMBER */
%token <string> NUMBER

%%

conf            : sections
                ;

sections        : section sections
                | section
                ;

section         : sectionname sectionitems
                | sectionname
                ;

sectionname     : OBRACKET WORD EBRACKET  { add_section(ctx->r, $2); }
                ;

sectionitems    : sectionitem sectionitems
                | sectionitem
                ;

sectionitem     : keyvaluepair
                ;

keyvaluepair    : key EQUAL value
                ;

key             : keybase OBRACKET modifier EBRACKET
                | keybase
                ;

keybase         : WORD { add_key(ctx->r, $1); }
                ;

modifier        : WORD  { add_modifier(ctx->r, $1); }
                ;

value           : STRING  { add_value(ctx->r, $1); }
                | BOOLEAN { add_value(ctx->r, $1); }
                | NUMBER  { add_value(ctx->r, $1); }
                ;

%%

static void yyerror(ini_parser_context_t *ctx, void *yyscanner, const char *err)
{
    MRP_UNUSED(yyscanner);
    mrp_log_error("ini file parser error: '%s'", err);
    ctx->error = 1;
    return;
}

bool mrp_parse_ini_file(const char *filename, ini_parser_result_t *result) {

    /* contains the buffers required for saving the string data */
    ini_parser_context_t ctx;
    FILE *input;

    /* contains the flex-initialized parser data */
    void *scanner;

    input = fopen(filename, "r");

    if (!input)
        return false;

    initialize_parser_buffer(&ctx);

    ctx.r = result;

    ini_parser_lex_init(&scanner);

    /* set the input file */
    ini_parser_set_in(input, scanner);

    /* set the parameter to be passed to flex (yyextra) */
    ini_parser_set_extra(&ctx, scanner);

    ini_parser_parse(&ctx, scanner);

    if (ctx.error == 1)
        return false;

    ini_parser_lex_destroy(scanner);
    delete_parser_buffer(&ctx);

    return true;
};

