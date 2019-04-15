#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */


 // this implementation of token analysis make me uncomfortable

#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_EQ, TK_LB, TK_RB, TK_DEC,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", TK_PLUS},         // plus
  {"-", TK_MINUS},      // minus
  {"\\*", TK_MUL},      // multiple
  {"/", TK_DIV},        // division
  {"==", TK_EQ},        // equal
  {"\\(", TK_LB},       // left bracket
  {"\\)", TK_RB},       // right bracket
  {"[0-9]+", TK_DEC},   // decimal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        tokens[nr_token].type = rules[i].token_type;
        switch (rules[i].token_type) {

          case TK_DEC:
            if (substr_len > 32) {
              printf("decimal too long\n");
              return false;
            }
            memcpy(tokens[nr_token].str, substr_start, substr_len);


            for (int j = 0; j < substr_len; j++) {
              printf("%c", tokens[nr_token].str[j]);
            }
            printf("\n");
            break;



          default:
            //panic("it should never reach here\n");
            break;
        }
        nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();
  //return eval(0, nr_token);
  return 0;
}
