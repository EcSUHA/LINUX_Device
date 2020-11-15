#include "ProjectConfig.h"

#if defined(ESP_PLATFORM)
#include <esp8266.h>
#endif

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <strings.h>

#include "SCDE_s.h"

#include "SCDE.h"

#include "ArgParse.h"


// flags die aus short/long opt Parse ins ins Detail weitergegeben werden
#define OPT_UNSET 1		// flag -> ?
#define OPT_LONG  (1 << 1)	// flag -> long opt is processed
























inline int 
ishex(int x)
{
  return (x >= '0' && x <= '9')	||
         (x >= 'a' && x <= 'f')	||
         (x >= 'A' && x <= 'F');
}




// ArgParse_SplitUriEncodedArgs
// char *argsText -> the uri-encoded, zero-terminated source-args to parse
// char *args -> the destination for the splitted args
// char **argv -> allocated memory to store the extacted arg[x]-start positions in destination string
static int 
ArgParse_SplitUriEncodedArgs(char *argsText
                 ,char *args
                 ,char **argv)
{
  int count = 0;

  // skip initial '&', if any
  while (*argsText == '&') ++argsText;

  // loop till '\0'
  while (*argsText) {

    // in 2nd run, when argv[x] is allocated -> store pos of arg[x]-start
    if (argv) argv[count] = (char *) args;	

    // loop till '\0' or '&'
    while (*argsText && *argsText != '&') {

      int c;

      // uri-decode char
      c = *argsText++;

      if (c == '+') c = ' ';

      else if (c == '%' && ( !ishex(*argsText++)	||
                             !ishex(*argsText++)	||
                             !sscanf(argsText - 2, "%2x", &c)))
        return -1; //????????????
 
      *args++ =  c;                                           // is not '&'!!
    }

    // in 2nd run, when argv[x] is allocated -> store an '\0' to split/zero terminate
    if (argv && *args) *args++ = '\0';

    // skip further '&', if any
    while (*argsText == '&') ++argsText;

    count++;
  }

  return count;
}










// get argc and argv

/*
             aa&bbb&cccc&dd+dd&ee&        <- original string
             ---------------------
             aa0bbb0cccc0dd dd0ee0        <- transformed string
             |  |   |    |     |
   argv[0] __/  /   /    /     /
   argv[1] ____/   /    /     /
   argv[2] _______/    /     /
   argv[3] ___________/     /
   argv[4] ________________/ 
   Note: 1 args inside '""' are not splitted !
*/




// int *argc -> ptr stores the count
// char *args -> ptr to the argument string

char **
ArgParse_SplitURLEncodedArgsToAllocatedMem(int *argc
                  ,char *argsText)
{
  char *args = NULL;
  char **argv = NULL;
  int argn = 0;

  if (argsText						// we have ptr to the source arguments ?
    && *argsText					// and is the argument at source read pos. not /0 ?
    && (args = malloc (strlen(argsText) + 1))		// allocated mem for destination string
    && (argn = ArgParse_SplitUriEncodedArgs(argsText, args, NULL))	// check if it contains >0 arg
    && (argv = malloc((argn+1) * sizeof(char *)))) {	// allocate char pointers for argn+1

    *argv++ = args;					// next
    argn = ArgParse_SplitUriEncodedArgs(argsText, args, argv);	// extract next+rest
  }

  if (args && !argv) free(args);			// ?

  // store the argument number
  *argc = argn;


//--
  printf("|SplitURLEncodedToArgsFn, got %d args (",argn);
  int i;
  for (i = 0; i < argn; i++)
    printf("[%s]",argv[i]);
  printf(")");
//--

  return argv;
}








	



































// get argc and argv

/*
             aa bbb ccc "dd d" ee         <- original string
             ---------------------
             aa0bbb0ccc00dd d00ee0        <- transformed string
             |  |   |    |     |
   argv[0] __/  /   /    /     /
   argv[1] ____/   /    /     /
   argv[2] _______/    /     /
   argv[3] ___________/     /
   argv[4] ________________/ 
   Note: 1 args inside '""' are not splitted !
*/

static int 
ArgParse_SetArgs(char *args
                ,char **argv)
{
  int count = 0;

  // skip leading spaces
  while (isspace((unsigned char)*args)) ++args;

  while (*args) {
    if (argv) argv[count] = args;
    while (*args && !isspace((unsigned char)*args)) ++args;
    if (argv && *args) *args++ = '\0';
    while (isspace((unsigned char)*args)) ++args;
    count++;
  }

  return count;
}


// int *argc -> ptr stores the count
// char *args -> ptr to the argument string
char **
ArgParse_SplitArgsToAllocatedMem(int *argc
                  ,uint8_t *argsText
                  ,size_t argsTextLen)
{
  char *args = (char *) argsText;
  char **argv = NULL;
  int argn = 0;

  if (args 						// we have ptr to the arguments
    && *args						// argument at read pos. is not /0
    && (args = strndup(args, argsTextLen))		// dublicate whole to zero termintaed string in allocated mem
    && (argn = ArgParse_SetArgs(args,NULL))	        // check if it contains >0 arg
    && (argv = malloc((argn+1) * sizeof(char *)))) {	// allocate char pointers for argn+1

    *argv++ = args;					// next
    argn = ArgParse_SetArgs(args,argv);			// extract next+rest
  }

  if (args && !argv) free(args);			// ?

  // store the argument number
  *argc = argn;


//--
  printf("|SplitArgsFn, got %d args (",argn);
  int i;
  for (i = 0; i < argn; i++)
    printf("[%s]",argv[i]);
  printf(")");
//--


  return argv;
}
























void
ArgParse_FreeSplittedArgsInAllocatedMem(char **argv)
{
  if (argv) {

    free(argv[-1]);
    free(argv-1);
  } 
}









//--------------------------------------------------------------------------------









 
static const char *
prefix_skip(const char *str, const char *prefix)
{

  size_t len = strlen(prefix);

  return strncmp(str, prefix, len) ? NULL : str + len;
}



static int
prefix_cmp(const char *str, const char *prefix)
{

  for (;; str++, prefix++)

    if (!*prefix) {

      return 0;

    } else if (*str != *prefix) {

    return (unsigned char)*prefix - (unsigned char)*str;
  }
}



/*
 * prepares an lead-in error-msg with given error-msg, caused while the detailed arguments are parsed
 */
void
ArgParse_PrepareLeadinErrorMsg(struct argparse *self, const struct argparse_option *opt,
               const char *reason, int flags)
{
  // allocate mem to prepare an concrete error msg (used as lead in of an complete retMsg)
  self->retMsg = malloc(sizeof(Entry_String_t));

  (void)self;

  if (flags & OPT_LONG) {

    // generate concrete error msg - long_name
    self->retMsg->string.len = asprintf(&self->retMsg->string.p_char
        ,"error: option `%s` %s\n" // was `--%s`
        ,opt->long_name, reason);

  } else {

    // generate concrete error msg - short_name
    self->retMsg->string.len = asprintf(&self->retMsg->string.p_char
        ,"error: option `-%c` %s\n"
        ,opt->short_name
        ,reason);
  }
}



// parses an implemented argument and calls the assigned callback. May prepare error retMsg.
// returns 0 = OK (alternativ vom callback) / -3 Fehler,aber spezifische msg schon gesetzt, keine ergänzung nötig
// (-1=break in command(hier nicht) und -2=unknown args error hier nicht)
static int //Argparse_GetValue
argparse_getvalue(struct argparse *self,
                  const struct argparse_option *opt,
                  int flags)
{
  const char *s = NULL;

  // temp ptr to temp stage1 values
  void *stage1Value;

  // temp stage1 values
//  bool stage1Bool;			// ARGPARSE_OPT_BOOL:	
  const char *stage1String = NULL;	// ARGPARSE_OPT_STRING:
  int stage1Integer;			// ARGPARSE_OPT_INTEGER:
  float stage1Float;			// ARGPARSE_OPT_FLOAT:


  // do we need to get a value? Else skip
  if (!opt->value) goto skipped;

  // which value-type should we get ?
  switch (opt->type) {

    // ?
    case ARGPARSE_OPT_BIT:

      if (flags & OPT_UNSET) {

        *(int *)opt->value &= ~opt->data;

      } else {

        *(int *)opt->value |= opt->data;
      }

      break;

    // ?
    case ARGPARSE_OPT_BOOLEAN:

      if (flags & OPT_UNSET) {

        *(int *)opt->value = *(int *)opt->value - 1;

      } else {

        *(int *)opt->value = *(int *)opt->value + 1;
      }

      if (*(int *)opt->value < 0) {

        *(int *)opt->value = 0;
      }

      break;

    // get 'char *' to zero-terminated string in input arguments
    case ARGPARSE_OPT_STRING:

      if (self->optvalue) {

       stage1String = self->optvalue;
       stage1Value = (void*) &stage1String;

//      *(const char **)opt->value = self->optvalue;
        self->optvalue             = NULL;

      } else if (self->argc > 1) {

        self->argc--;
        stage1String =  *++self->argv;
        stage1Value = (void*) &stage1String;

//        *(const char **)opt->value = *++self->argv;

      } else {

        ArgParse_PrepareLeadinErrorMsg(self, opt, "requires a value", flags);
        return -3;
      }

      break;

    // ?
    case ARGPARSE_OPT_INTEGER:

      errno = 0;

      if (self->optvalue) {

       stage1Integer = strtol(self->optvalue, (char **)&s, 0);
       stage1Value = (void*) &stage1Integer;

//      *(int *)opt->value = strtol(self->optvalue, (char **)&s, 0);

        self->optvalue     = NULL;

      } else if (self->argc > 1) {

        self->argc--;
        stage1Integer =  strtol(*++self->argv, (char **)&s, 0);
        stage1Value = (void*) &stage1Integer;

//      *(int *)opt->value = strtol(*++self->argv, (char **)&s, 0);

      } else {

        ArgParse_PrepareLeadinErrorMsg(self, opt, "requires a value", flags);
        return -3;
      }

      if (errno) {
        ArgParse_PrepareLeadinErrorMsg(self, opt, strerror(errno), flags);
        return -3;
      }

      if (s[0] != '\0') {

        ArgParse_PrepareLeadinErrorMsg(self, opt, "expects an integer value", flags);
        return -3;
      }

      break;

    // ?
    case ARGPARSE_OPT_FLOAT:

      errno = 0;

      if (self->optvalue) {

       stage1Float = strtof(self->optvalue, (char **)&s);
       stage1Value = (void*) &stage1Float;

//        *(float *)opt->value = strtof(self->optvalue, (char **)&s);

        self->optvalue       = NULL;

      } else if (self->argc > 1) {

        self->argc--;
        stage1Float =  strtof(*++self->argv, (char **)&s);
        stage1Value = (void*) &stage1Float;

//      *(float *)opt->value = strtof(*++self->argv, (char **)&s);

      } else {

        ArgParse_PrepareLeadinErrorMsg(self, opt, "requires a value", flags);
        return -3;
      }

      if (errno) {

        ArgParse_PrepareLeadinErrorMsg(self, opt, strerror(errno), flags);
        return -3;
      }

      if (s[0] != '\0') {

        ArgParse_PrepareLeadinErrorMsg(self, opt, "expects a numerical value", flags);
        return -3;
      }

      break;

    // ?
    case ARGPARSE_OPT_EXPERIMENTAL: //was string

      // at start of value string? (long: optkey=argument)
      if (self->optvalue) {

        *(const char **)opt->value = self->optvalue;
        self->optvalue             = NULL;

      // else we expect data in next argv (short opt -x argument)
      } else if (self->argc > 1) {

        self->argc--;
        *(const char **)opt->value = *++self->argv;

      } else {

        ArgParse_PrepareLeadinErrorMsg(self, opt, "requires a value", flags);
        return -3;
      }

      break;

    default:

      assert(0);
  }

skipped:

  // is an stage2 callback assigned? -> call with temporary stage1 value
  if (opt->callback) {

    return opt->callback(self, opt, flags, &stage1Value);

  // else store stage1 value as final value
  } else {

 // store value the right way
  switch (opt->type) {

   // ?
    case ARGPARSE_OPT_BIT:

      break;

    // ?
    case ARGPARSE_OPT_BOOLEAN:

//      *(bool *)opt->value =

      break;

    // keep stage1 string
    case ARGPARSE_OPT_STRING:

      *(const char **)opt->value = stage1String;

      break;

    // keep stage1 integer
     case ARGPARSE_OPT_INTEGER:

      *(int *)opt->value = stage1Integer;

      break;

    // keep stage1 float
    case ARGPARSE_OPT_FLOAT:

      *(float *)opt->value = stage1Float;

      break;

    // should not happen ...
    default:

      assert(0);
    } 
  }

  return 0;
}



static void
argparse_options_check(const struct argparse_option *options)
{
    for (; options->type != ARGPARSE_OPT_END; options++) {
        switch (options->type) {
        case ARGPARSE_OPT_END:
        case ARGPARSE_OPT_BOOLEAN:
        case ARGPARSE_OPT_BIT:
        case ARGPARSE_OPT_INTEGER:
        case ARGPARSE_OPT_FLOAT:
        case ARGPARSE_OPT_STRING:
        case ARGPARSE_OPT_GROUP:
            continue;
        default:
            printf("wrong option type: %d", options->type);
            break;
        }
    }
}





/**
 * -------------------------------------------------------------------------------------------------
 *  FName: argparse_short_opt
 *  Desc: Checks if given short opt is implemented by seeking trough the short options list.
 *        Calls getvalue if matched. Returns 'do error msg indicator' (-2) if not found.
 *  Info: 
 *  Para: struct argparse *self ->  
 *        const struct argparse_option *options -> 
 *  Rets: static int result -> 0=ok / -1=  break in command(here not used) / -2 = do unkn. args error
 * -------------------------------------------------------------------------------------------------
 */
static int
argparse_short_opt(struct argparse *self
                  ,const struct argparse_option *options)
{

  // seeking trough the short opts ...
  for (; options->type != ARGPARSE_OPT_END; options++) {

    // matching short opt ?
    if (options->short_name == *self->optvalue) {

      // inc ptr +1 to go to possible value (if any), else set ptr to NULL
      self->optvalue = self->optvalue[1] ? self->optvalue + 1 : NULL;

      // exec short opt + get value
      return argparse_getvalue(self, options, 0);
    }
  }

  return -2;	// unknown short opt error 
}




/**
 * -------------------------------------------------------------------------------------------------
 *  FName: argparse_long_opt
 *  Desc: Checks if given long opt is implemented by seeking trough the long options list.
 *        Calls getvalue if matched. Returns 'do error msg indicator' (-2) if not found.
 *  Info: 
 *  Para: struct argparse *self ->  
 *        const struct argparse_option *options -> 
 *  Rets: static int result -> 0=ok / -1=  break in command(here not used) / -2 = do unkn. args error
 * -------------------------------------------------------------------------------------------------
 */

// returns 0=ok / -1=  break in command(hier nicht) /-2 = unknown args error 
static int
argparse_long_opt(struct argparse *self
                 ,const struct argparse_option *options)
{

  // seeking trough the long opts ...
  for (; options->type != ARGPARSE_OPT_END; options++) {

    const char *rest;

    // this are different flags (NOT options->flags)
    int opt_flags = 0;

    // no long_opt assigned in table entry ? skip
    if (!options->long_name)
          continue;

    rest = prefix_skip(self->argv[0] + 0, options->long_name);

    if (!rest) {

      // negation disabled?
      if (options->flags & OPT_NONEG) {

        continue;
      }

      // only OPT_BOOLEAN/OPT_BIT supports negation
      if (options->type != ARGPARSE_OPT_BOOLEAN && options->type !=
            ARGPARSE_OPT_BIT) {

        continue;
      }

      if (prefix_cmp(self->argv[0] + 0, "no-")) {

        continue;
      }

      rest = prefix_skip(self->argv[0] + 0 + 3, options->long_name);

      if (!rest)
            continue;

      opt_flags |= OPT_UNSET;
    }

    if (*rest) {

      if (*rest != '=')
            continue;

      self->optvalue = rest + 1;
    }

    return argparse_getvalue(self, options, opt_flags | OPT_LONG);
  }

  return -2;	// unknown long opt error 
}




/* original
static int
argparse_long_opt(struct argparse *self
                 ,const struct argparse_option *options)
{

  // seeking trough the long opts ...
  for (; options->type != ARGPARSE_OPT_END; options++) {

    const char *rest;
    int opt_flags = 0;

    // no long_opt assigned in table entry ? skip
    if (!options->long_name)
          continue;

    rest = prefix_skip(self->argv[0] + 2, options->long_name);

    if (!rest) {

      // negation disabled?
      if (options->flags & OPT_NONEG) {

        continue;
      }

      // only OPT_BOOLEAN/OPT_BIT supports negation
      if (options->type != ARGPARSE_OPT_BOOLEAN && options->type !=
            ARGPARSE_OPT_BIT) {

        continue;
      }

      if (prefix_cmp(self->argv[0] + 2, "no-")) {

        continue;
      }

      rest = prefix_skip(self->argv[0] + 2 + 3, options->long_name);

      if (!rest)
            continue;

      opt_flags |= OPT_UNSET;
    }

    if (*rest) {

      if (*rest != '=')
            continue;

      self->optvalue = rest + 1;
    }

    return argparse_getvalue(self, options, opt_flags | OPT_LONG);
  }

  return -2;	// unknown long opt error 
}
*/














/* --------------------------------------------------------------------------------------------------
 *  FName: AnalyzeCommand
 *  Desc: Seeks command and commandArgs from given args and calls the commandFn from an matching 
 *        module name (if loaded / available)
 *  Info: 
 *  Para: const uint8_t *args  -> ptr to space seperated "command args" text
 *        const size_t argsLen -> length of command and args
 *  Rets: struct headRetMsgMultiple_s -> head from STAILQ, stores multiple RetMsg (errors), NULL=OK/None
 * --------------------------------------------------------------------------------------------------
 */



/*
Features
- handles both optional and positional arguments
- produces highly informative usage messages
- issues errors when given invalid arguments

There are basically three types of options:

- boolean options
- options with mandatory argument
- options with optional argument

There are basically two forms of options:

- short option consist of one dash (-) and one alphanumeric character.
- long option begin with two dashes (--) and some alphanumeric characters.
(Short options may be bundled, e.g. -a -b can be specified as -ab. Options are case-sensitive.)

Options and non-option arguments can clearly be separated using the -- option.
*/




/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ArgParse_Init
 *  Desc: Processes the device-specific command line arguments from the get command
 *  Info: Invoked by cmd-line 'Get ESP32Control_Definition.common.Name getArgs'
 *  Para: ESP32Control_Definition_t *ESP32Control_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *getArgsText -> the getArgsText
 *        size_t getArgsTextLen -> length of the getArgsText
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
Entry_String_t *
ArgParse_Init(struct argparse *self, struct argparse_option *options,
              const char *const *usages, int flags, const char *description,
                  const char *epilog)
{
  memset(self, 0, sizeof(*self));

  self->options     = options;
  self->usages      = usages;
  self->flags       = flags;
  self->description = description;
  self->epilog      = epilog;
  self->retMsg      = NULL;

  return NULL;
}






/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ArgParse_Parse
 *  Desc: Processes the device-specific command line arguments from the get command
 *  Info: Invoked by cmd-line 'Get ESP32Control_Definition.common.Name getArgs'
 *  Para: ESP32Control_Definition_t *ESP32Control_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *getArgsText -> the getArgsText
 *        size_t getArgsTextLen -> length of the getArgsText
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */

/*
   case  0: ok_end      -> no error occured till now          -> the ArgParse will be completed without error msg.
   case -1:		-> 
   case -2: unknown_end -> unknown option error occured in options parse  -> retMsg=the argument that causes the error + usage
   case -3: error_end   -> an error occured in detailed parse -> an detailed error msg must be stored to submit
*/

// returns 0=ok / -1=  break in command(hier nicht) /-2 = unknown args error 
Entry_String_t *
ArgParse_Parse(struct argparse *self, int argc, const char **argv)
{
  self->argc = argc;
  self->argv = argv;
  self->out  = argv;

  argparse_options_check(self->options);

  // loop through the arguments
  for (; self->argc; self->argc--, self->argv++) {

    const char *arg = self->argv[0];

    // short option leadin (-) + an option key -> start parse 
    if (arg[0] == '-' && arg[1]) {

      self->optvalue = arg + 1;

      switch (argparse_short_opt(self, self->options)) {

        case -1:

          break;

        case -2:

          goto unknown_end;

        case -3:

          goto error_end;
      }

      // more short options combined ?
      while (self->optvalue) {

        switch (argparse_short_opt(self, self->options)) {

          case -1:

            break;

          case -2:

            goto unknown_end;

          case -3:

            goto error_end;

        }
      }

      continue;
    }


    // short option leadin (-) + an NO option key -> next arg 
    if (arg[0] == '-' && !arg[1]) {

      continue;
    }


    // 3rd char in arg -> long option
    switch (argparse_long_opt(self, self->options)) {

      case -1:

        break;

      case -2:

        goto unknown_end;

      case -3:

        goto error_end;
    }

    continue;


    // an error occured in options parse. We have the argument that causes the error.
    // prepare an detailed error msg, and return it
    unknown_end: ;

    // allocate mem to prepare the error msg
    self->retMsg = malloc(sizeof(Entry_String_t));

    // generate concrete error msg - long_name
    self->retMsg->string.len = asprintf(&self->retMsg->string.p_char
        ,"error: unknown option `%s`"
        , self->argv[0]);

    ArgParse_Usage(self);

    return self->retMsg;


    // an error occured in detailed parse
    // we have already prepared an detailed error msg, return it
    error_end: ;

   return self->retMsg;
  }


  // ok, no error msg is prepared = return NULL. ArgParse ends here.
//  ok_end:

    // move the rest of arguments without processing (if any)
    memmove(self->out + self->cpidx, self->argv,
        self->argc * sizeof(*self->out));

  self->out[self->cpidx + self->argc] = NULL;

  // parsed ok, till now no retMsg, argc = self->cpidx + self->argc !
  return NULL;
}



/* original
Entry_String_t *
ArgParse_Parse(struct argparse *self, int argc, const char **argv)
{
  self->argc = argc;
  self->argv = argv;
  self->out  = argv;

  argparse_options_check(self->options);

  // loop through the arguments
  for (; self->argc; self->argc--, self->argv++) {

    const char *arg = self->argv[0];

    // non option (-) or only '-' -> stop parse (if flag is set)
    if (arg[0] != '-' || !arg[1]) {

      if (self->flags & ARGPARSE_STOP_AT_NON_OPTION) {

        goto ok_end;
      }

      // else non option or single char '-' -> return as arg[x] result for later use
      self->out[self->cpidx++] = self->argv[0];

      continue;
    }

    // short option, if not '--'
    if (arg[1] != '-') {

      self->optvalue = arg + 1;

      switch (argparse_short_opt(self, self->options)) {

        case -1:

          break;

        case -2:

          goto unknown_end;

        case -3:

          goto error_end;
      }

      // more short options combined ?
      while (self->optvalue) {

        switch (argparse_short_opt(self, self->options)) {

          case -1:

            break;

          case -2:

            goto unknown_end;

          case -3:

            goto error_end;

        }
      }

      continue;
    }

    // else '--' is present here, test for 3rd char in arg
    // if none -> do not store "--" + next arg
    if (!arg[2]) {

      // no 3rd char in arg, stop parse, return rest of elements
      self->argc--;
      self->argv++;

      break;
    }

    // 3rd char in arg -> long option
    switch (argparse_long_opt(self, self->options)) {

      case -1:

        break;

      case -2:

        goto unknown_end;

      case -3:

        goto error_end;
    }

    continue;


    // an error occured in options parse. We have the argument that causes the error.
    // prepare an detailed error msg, and return it
    unknown_end: ;

    // allocate mem to prepare the error msg
    self->retMsg = malloc(sizeof(Entry_String_t));

    // generate concrete error msg - long_name
    self->retMsg->string.len = asprintf(&self->retMsg->string.p_char
        ,"error: unknown option `%s`"
        , self->argv[0]);

    ArgParse_Usage(self);

    return self->retMsg;


    // an error occured in detailed parse
    // we have already prepared an detailed error msg, return it
    error_end: ;

   return self->retMsg;
  }


  // ok, no error msg is prepared = return NULL. ArgParse ends here.
  ok_end:

    // move the rest of arguments without processing (if any)
    memmove(self->out + self->cpidx, self->argv,
        self->argc * sizeof(*self->out));

  self->out[self->cpidx + self->argc] = NULL;

  // parsed ok, till now no retMsg, argc = self->cpidx + self->argc !
  return NULL;
}
*/










/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ArgParse_Usage
 *  Desc: Processes the device-specific command line arguments from the get command
 *  Info: Invoked by cmd-line 'Get ESP32Control_Definition.common.Name getArgs'
 *  Para: ESP32Control_Definition_t *ESP32Control_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *getArgsText -> the getArgsText
 *        size_t getArgsTextLen -> length of the getArgsText
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
void
ArgParse_Usage(struct argparse *self)
{
printf("entering usage...\n");


  // we will build an 'usage' return message
  Entry_String_t *retMsg = NULL;

  // the write offset -> will later be used to calculate required strBufferLen, too
  size_t strWriteOffset;
  size_t strBufferLen = 20000; // = 0;

  // set string position to NULL -> nothing will be written, because strBufferLen is 0 in 1st cycle
  char* p_strBuffer = NULL;

  do {

    // in 2nd cycle we will have the buffer-length here, to alloc
    if (strBufferLen) {

      // alloc mem for retMsg
      retMsg = malloc(sizeof(Entry_String_t));

      // the buffer for the answer string
      retMsg->string.len = strBufferLen;
      retMsg->string.p_char = malloc(strBufferLen+1);

      // finally set string position to write (2nd cycle)
      p_strBuffer  = (char *) retMsg->string.p_char;
    }

    // reset the string write offset
    strWriteOffset = 0;

    // retMsg: print starts here

    // inserting existing lead-in error msg (if any)
    if (self->retMsg) {

      strWriteOffset += snprintf(p_strBuffer + strWriteOffset
            ,strBufferLen
            ,"%.*s"
            ,self->retMsg->string.len
            ,self->retMsg->string.p_char);
    }

  // list the assigned usages, or simply Usage:
  if (self->usages) {

    strWriteOffset += snprintf(p_strBuffer + strWriteOffset
          ,strBufferLen
          ,"Usage: %s\n", *self->usages++);

    while (*self->usages && **self->usages)

      strWriteOffset += snprintf(p_strBuffer + strWriteOffset
            ,strBufferLen
            ,"   or: %s\n", *self->usages++);

  } else {

    strWriteOffset += snprintf(p_strBuffer + strWriteOffset
          ,strBufferLen
          ,"Usage:\n");
  }

  // print the assigned description, if any
  if (self->description)

    strWriteOffset += snprintf(p_strBuffer + strWriteOffset
          ,strBufferLen
          ,"%s\n", self->description);

//    fputc('\n', stdout);
    strWriteOffset += snprintf(p_strBuffer + strWriteOffset
          ,strBufferLen
          ,"\n");
//---

  const struct argparse_option *options;

  // figure out best width - calculate usage_opts_width ??
  size_t usage_opts_width = 0;
  size_t len;

  options = self->options;

  for (; options->type != ARGPARSE_OPT_END; options++) {

    len = 0;

    if ((options)->short_name) {

      len += 2;
    }

    if ((options)->short_name && (options)->long_name) {

      len += 2;           // separator ", "
    }

    if ((options)->long_name) {

      len += strlen((options)->long_name) + 2;
    }

    if (options->type == ARGPARSE_OPT_INTEGER) {

      len += strlen("=<int>");
    }

    if (options->type == ARGPARSE_OPT_FLOAT) {

      len += strlen("=<flt>");

    } else if (options->type == ARGPARSE_OPT_STRING) {

      len += strlen("=<str>");
    }

    len = (len + 3) - ((len + 3) & 3);

    if (usage_opts_width < len) {

      usage_opts_width = len;
    }
  }

  usage_opts_width += 4;      // 4 spaces prefix

  options = self->options;

  for (; options->type != ARGPARSE_OPT_END; options++) {

    size_t pos = 0;
    int pad    = 0;

    if (options->type == ARGPARSE_OPT_GROUP) {

//    putc('\n', stdout);
    strWriteOffset += snprintf(p_strBuffer + strWriteOffset
          ,strBufferLen
          ,"\n");
//---

      strWriteOffset += snprintf(p_strBuffer + strWriteOffset
            ,strBufferLen
            ,"%s"
            ,options->help);

//    putc('\n', stdout);
    strWriteOffset += snprintf(p_strBuffer + strWriteOffset
          ,strBufferLen
          ,"\n");
//---

      continue;
    }











// need extra count
    pos = snprintf(p_strBuffer + strWriteOffset
            ,strBufferLen
            ,"    ");

    if (options->short_name) {

      pos += snprintf(p_strBuffer + strWriteOffset + pos
            ,strBufferLen
            ,"-%c"
            ,options->short_name);
    }

    if (options->long_name && options->short_name) {

      pos += snprintf(p_strBuffer + strWriteOffset + pos
            ,strBufferLen
            ,", ");
    }

    if (options->long_name) {

      pos += snprintf(p_strBuffer + strWriteOffset + pos
            ,strBufferLen
            ,"%s"
            , options->long_name);
    }

    if (options->type == ARGPARSE_OPT_INTEGER) {

      pos += snprintf(p_strBuffer + strWriteOffset + pos
            ,strBufferLen
            ,"=<int>");

    } else if (options->type == ARGPARSE_OPT_FLOAT) {

      pos += snprintf(p_strBuffer + strWriteOffset + pos
            ,strBufferLen
            ,"=<flt>");

    } else if (options->type == ARGPARSE_OPT_STRING) {

      pos += snprintf(p_strBuffer + strWriteOffset + pos
            ,strBufferLen
            ,"=<str>");
    }

    // pos extra count is finnished here
    strWriteOffset += pos;

    if (pos <= usage_opts_width) {

      pad = usage_opts_width - pos;

    } else {

//    fputc('\n', stdout);
    strWriteOffset += snprintf(p_strBuffer + strWriteOffset
          ,strBufferLen
          ,"\n");
//---

      pad = usage_opts_width;
    }



    strWriteOffset += snprintf(p_strBuffer + strWriteOffset
          ,strBufferLen
          ,"%*s%s\n"
          ,pad + 2
          ,""
          ,options->help);
  }

    // print the assigned epilog, if any
    if (self->epilog) {

      strWriteOffset += snprintf(p_strBuffer + strWriteOffset
            ,strBufferLen
            ,"%s\n", self->epilog);
    }

    // retMsg: print is completed here

    // store reqired buffer length for alloc in 2nd cycle
    strBufferLen = strWriteOffset;

  } while (!retMsg);

  // dealloc lead in error-msg,if any
  if (self->retMsg) {

    if (self->retMsg->string.p_char) free(self->retMsg->string.p_char);
    free (self->retMsg);
  }

  // finally store the prepared 'usage' return message
  self->retMsg = retMsg;     




// temporary solution
 retMsg->string.len = strBufferLen;
}



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: ESP32_SwITCH_Get
 *  Desc: Processes the device-specific command line arguments from the get command
 *  Info: Invoked by cmd-line 'Get ESP32Control_Definition.common.Name getArgs'
 *  Para: ESP32Control_Definition_t *ESP32Control_Definition -> WebIF Definition that should get a set cmd
 *        uint8_t *getArgsText -> the getArgsText
 *        size_t getArgsTextLen -> length of the getArgsText
 *  Rets: strTextMultiple_t* -> response text in allocated memory, NULL=no text
 * -------------------------------------------------------------------------------------------------
 */
int
argparse_help_cb(struct argparse *self
                ,const struct argparse_option *option
                ,int flags
                ,void *stage1Value)
{
  (void)option;

  ArgParse_Usage(self);

printf("HELP-callback\n");

 return -3; // because ArgParse_Usage prepares an retMsg
//    exit(0);
}



