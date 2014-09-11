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
#include "ini-parser.h"
#include "ini.tab.h"

#define YY_EXTRA_TYPE ini_parser_context_t *

%}

%option noyywrap
%option reentrant
%option bison-bridge
%option prefix="ini_parser_"

%%
\#.*\n                  /* comment */
\n                      /* newline */
[ \t]+                  /* whitespace */
\[                      return OBRACKET;
\]                      return EBRACKET;
\=                      return EQUAL;
\".*\"                  {
                            char *res = new_parser_str(yyextra, yytext);
                            if (!res) {
                                printf("Parser OOM error!\n");
                            }
                            yylval->string = res;
                            return STRING;
                        }
[-]?[0-9]+              {
                            char *res = new_parser_str(yyextra, yytext);
                            if (!res) {
                                printf("Parser OOM error!\n");
                            }
                            yylval->string = res;
                            return NUMBER;
                        }
TRUE|FALSE              {
                            char *res = new_parser_str(yyextra, yytext);
                            if (!res) {
                                printf("Parser OOM error!\n");
                            }
                            yylval->string = res;
                            return BOOLEAN;
                        }
[a-zA-Z0-9_]+           {
                            char *res = new_parser_str(yyextra, yytext);
                            if (!res) {
                                printf("Parser OOM error!\n");
                            }
                            yylval->string = res;
                            return WORD;
                        }
<*>.                    /* unrecognized input */
%%

