/* -*- Mode: C -*-
 *
 * File: gfsmxl_cascade_nbest_strings_cmdparser.c
 * Description: Code for command-line parser struct gengetopt_args_info.
 *
 * File autogenerated by optgen.perl version 0.07
 * generated with the following command:
 * /usr/local/bin/optgen.perl -u -l --no-handle-rcfile --nopod -F gfsmxl_cascade_nbest_strings_cmdparser gfsmxl_cascade_nbest_strings.gog
 *
 * The developers of optgen.perl consider the fixed text that goes in all
 * optgen.perl output files to be in the public domain:
 * we make no copyright claims on it.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

/* If we use autoconf/autoheader.  */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_PWD_H
# include <pwd.h>
#endif

/* Allow user-overrides for PACKAGE and VERSION */
#ifndef PACKAGE
#  define PACKAGE "PACKAGE"
#endif

#ifndef VERSION
#  define VERSION "VERSION"
#endif


#ifndef PROGRAM
# define PROGRAM "gfsmxl_cascade_nbest_strings"
#endif

/* #define cmdline_parser_DEBUG */

/* Check for "configure's" getopt check result.  */
#ifndef HAVE_GETOPT_LONG
# include "getopt.h"
#else
# include <getopt.h>
#endif

#include "gfsmxl_cascade_nbest_strings_cmdparser.h"


/* user code section */

/* end user  code section */


void
cmdline_parser_print_version (void)
{
  printf("gfsmxl_cascade_nbest_strings (%s %s) by Bryan Jurish <moocow.bovine@gmail.com>\n", PACKAGE, VERSION);
}

void
cmdline_parser_print_help (void)
{
  cmdline_parser_print_version ();
  printf("\n");
  printf("Purpose:\n");
  printf("  Lookup N-best string path in a gfsmxl cascade for line-buffered input\n");
  printf("\n");
  
  printf("Usage: %s [OPTIONS]... TEXT_FILE\n", "gfsmxl_cascade_nbest_strings");
  
  printf("\n");
  printf(" Arguments:\n");
  printf("   TEXT_FILE  Input text file\n");
  
  printf("\n");
  printf(" Options:\n");
  printf("   -h         --help               Print help and exit.\n");
  printf("   -V         --version            Print version and exit.\n");
  printf("   -CCASCADE  --cascade=CASCADE    Specify cascade for lookup\n");
  printf("   -lLABFILE  --labels=LABFILE     Specify alphabet for string->label lookup\n");
  printf("   -u         --utf8               Assume UTF-8 encoded alphabet and input\n");
  printf("   -pN_PATHS  --max-paths=N_PATHS  Specify maximum number of paths to extract\n");
  printf("   -oN_OPS    --max-ops=N_OPS      Specify maximum number of configuration-pop operations\n");
  printf("   -wFLOAT    --max-weight=FLOAT   Specify maximum weight of extractable paths\n");
  printf("   -a         --att-mode           Respect AT&T regex-style escapes in input strings\n");
  printf("   -d         --debug              Use old lookup & paths routine (for debugging)\n");
  printf("   -FFILE     --output=FILE        Specify output file (default=stdout).\n");
}

#if defined(HAVE_STRDUP) || defined(strdup)
# define gog_strdup strdup
#else
/* gog_strdup(): automatically generated from strdup.c. */
/* strdup.c replacement of strdup, which is not standard */
static char *
gog_strdup (const char *s)
{
  char *result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
}
#endif /* HAVE_STRDUP */

/* clear_args(args_info): clears all args & resets to defaults */
static void
clear_args(struct gengetopt_args_info *args_info)
{
  args_info->cascade_arg = gog_strdup("cascade.gfsc"); 
  args_info->labels_arg = gog_strdup("cascade.lab"); 
  args_info->utf8_flag = 0; 
  args_info->max_paths_arg = 1; 
  args_info->max_ops_arg = -1; 
  args_info->max_weight_arg = 1e+38; 
  args_info->att_mode_flag = 0; 
  args_info->debug_flag = 0; 
  args_info->output_arg = gog_strdup("-"); 
}


int
cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info)
{
  int c;	/* Character of the parsed option.  */
  int missing_required_options = 0;	

  args_info->help_given = 0;
  args_info->version_given = 0;
  args_info->cascade_given = 0;
  args_info->labels_given = 0;
  args_info->utf8_given = 0;
  args_info->max_paths_given = 0;
  args_info->max_ops_given = 0;
  args_info->max_weight_given = 0;
  args_info->att_mode_given = 0;
  args_info->debug_given = 0;
  args_info->output_given = 0;

  clear_args(args_info);

  /* rcfile handling */
  
  /* end rcfile handling */

  optarg = 0;
  optind = 1;
  opterr = 1;
  optopt = '?';

  while (1)
    {
      int option_index = 0;
      static struct option long_options[] = {
	{ "help", 0, NULL, 'h' },
	{ "version", 0, NULL, 'V' },
	{ "cascade", 1, NULL, 'C' },
	{ "labels", 1, NULL, 'l' },
	{ "utf8", 0, NULL, 'u' },
	{ "max-paths", 1, NULL, 'p' },
	{ "max-ops", 1, NULL, 'o' },
	{ "max-weight", 1, NULL, 'w' },
	{ "att-mode", 0, NULL, 'a' },
	{ "debug", 0, NULL, 'd' },
	{ "output", 1, NULL, 'F' },
        { NULL,	0, NULL, 0 }
      };
      static char short_options[] = {
	'h',
	'V',
	'C', ':',
	'l', ':',
	'u',
	'p', ':',
	'o', ':',
	'w', ':',
	'a',
	'd',
	'F', ':',
	'\0'
      };

      c = getopt_long (argc, argv, short_options, long_options, &option_index);

      if (c == -1) break;	/* Exit from 'while (1)' loop.  */

      if (cmdline_parser_parse_option(c, long_options[option_index].name, optarg, args_info) != 0) {
	exit (EXIT_FAILURE);
      }
    } /* while */

  

  if ( missing_required_options )
    exit (EXIT_FAILURE);

  
  if (optind < argc) {
      int i = 0 ;
      args_info->inputs_num = argc - optind ;
      args_info->inputs = (char **)(malloc ((args_info->inputs_num)*sizeof(char *))) ;
      while (optind < argc)
        args_info->inputs[ i++ ] = gog_strdup (argv[optind++]) ; 
  }

  return 0;
}


/* Parse a single option */
int
cmdline_parser_parse_option(char oshort, const char *olong, const char *val,
			       struct gengetopt_args_info *args_info)
{
  if (!oshort && !(olong && *olong)) return 1;  /* ignore null options */

#ifdef cmdline_parser_DEBUG
  fprintf(stderr, "parse_option(): oshort='%c', olong='%s', val='%s'\n", oshort, olong, val);*/
#endif

  switch (oshort)
    {
      case 'h':	 /* Print help and exit. */
          if (args_info->help_given) {
            fprintf(stderr, "%s: `--help' (`-h') option given more than once\n", PROGRAM);
          }
          clear_args(args_info);
          cmdline_parser_print_help();
          exit(EXIT_SUCCESS);
        
          break;
        
        case 'V':	 /* Print version and exit. */
          if (args_info->version_given) {
            fprintf(stderr, "%s: `--version' (`-V') option given more than once\n", PROGRAM);
          }
          clear_args(args_info);
          cmdline_parser_print_version();
          exit(EXIT_SUCCESS);
        
          break;
        
        case 'C':	 /* Specify cascade for lookup */
          if (args_info->cascade_given) {
            fprintf(stderr, "%s: `--cascade' (`-C') option given more than once\n", PROGRAM);
          }
          args_info->cascade_given++;
          if (args_info->cascade_arg) free(args_info->cascade_arg);
          args_info->cascade_arg = gog_strdup(val);
          break;
        
        case 'l':	 /* Specify alphabet for string->label lookup */
          if (args_info->labels_given) {
            fprintf(stderr, "%s: `--labels' (`-l') option given more than once\n", PROGRAM);
          }
          args_info->labels_given++;
          if (args_info->labels_arg) free(args_info->labels_arg);
          args_info->labels_arg = gog_strdup(val);
          break;
        
        case 'u':	 /* Assume UTF-8 encoded alphabet and input */
          if (args_info->utf8_given) {
            fprintf(stderr, "%s: `--utf8' (`-u') option given more than once\n", PROGRAM);
          }
          args_info->utf8_given++;
         if (args_info->utf8_given <= 1)
           args_info->utf8_flag = !(args_info->utf8_flag);
          break;
        
        case 'p':	 /* Specify maximum number of paths to extract */
          if (args_info->max_paths_given) {
            fprintf(stderr, "%s: `--max-paths' (`-p') option given more than once\n", PROGRAM);
          }
          args_info->max_paths_given++;
          args_info->max_paths_arg = (int)atoi(val);
          break;
        
        case 'o':	 /* Specify maximum number of configuration-pop operations */
          if (args_info->max_ops_given) {
            fprintf(stderr, "%s: `--max-ops' (`-o') option given more than once\n", PROGRAM);
          }
          args_info->max_ops_given++;
          args_info->max_ops_arg = (int)atoi(val);
          break;
        
        case 'w':	 /* Specify maximum weight of extractable paths */
          if (args_info->max_weight_given) {
            fprintf(stderr, "%s: `--max-weight' (`-w') option given more than once\n", PROGRAM);
          }
          args_info->max_weight_given++;
          args_info->max_weight_arg = (float)strtod(val, NULL);
          break;
        
        case 'a':	 /* Respect AT&T regex-style escapes in input strings */
          if (args_info->att_mode_given) {
            fprintf(stderr, "%s: `--att-mode' (`-a') option given more than once\n", PROGRAM);
          }
          args_info->att_mode_given++;
         if (args_info->att_mode_given <= 1)
           args_info->att_mode_flag = !(args_info->att_mode_flag);
          break;
        
        case 'd':	 /* Use old lookup & paths routine (for debugging) */
          if (args_info->debug_given) {
            fprintf(stderr, "%s: `--debug' (`-d') option given more than once\n", PROGRAM);
          }
          args_info->debug_given++;
         if (args_info->debug_given <= 1)
           args_info->debug_flag = !(args_info->debug_flag);
          break;
        
        case 'F':	 /* Specify output file (default=stdout). */
          if (args_info->output_given) {
            fprintf(stderr, "%s: `--output' (`-F') option given more than once\n", PROGRAM);
          }
          args_info->output_given++;
          if (args_info->output_arg) free(args_info->output_arg);
          args_info->output_arg = gog_strdup(val);
          break;
        
        case 0:	 /* Long option(s) with no short form */
        /* Print help and exit. */
          if (strcmp(olong, "help") == 0) {
            if (args_info->help_given) {
              fprintf(stderr, "%s: `--help' (`-h') option given more than once\n", PROGRAM);
            }
            clear_args(args_info);
            cmdline_parser_print_help();
            exit(EXIT_SUCCESS);
          
          }
          
          /* Print version and exit. */
          else if (strcmp(olong, "version") == 0) {
            if (args_info->version_given) {
              fprintf(stderr, "%s: `--version' (`-V') option given more than once\n", PROGRAM);
            }
            clear_args(args_info);
            cmdline_parser_print_version();
            exit(EXIT_SUCCESS);
          
          }
          
          /* Specify cascade for lookup */
          else if (strcmp(olong, "cascade") == 0) {
            if (args_info->cascade_given) {
              fprintf(stderr, "%s: `--cascade' (`-C') option given more than once\n", PROGRAM);
            }
            args_info->cascade_given++;
            if (args_info->cascade_arg) free(args_info->cascade_arg);
            args_info->cascade_arg = gog_strdup(val);
          }
          
          /* Specify alphabet for string->label lookup */
          else if (strcmp(olong, "labels") == 0) {
            if (args_info->labels_given) {
              fprintf(stderr, "%s: `--labels' (`-l') option given more than once\n", PROGRAM);
            }
            args_info->labels_given++;
            if (args_info->labels_arg) free(args_info->labels_arg);
            args_info->labels_arg = gog_strdup(val);
          }
          
          /* Assume UTF-8 encoded alphabet and input */
          else if (strcmp(olong, "utf8") == 0) {
            if (args_info->utf8_given) {
              fprintf(stderr, "%s: `--utf8' (`-u') option given more than once\n", PROGRAM);
            }
            args_info->utf8_given++;
           if (args_info->utf8_given <= 1)
             args_info->utf8_flag = !(args_info->utf8_flag);
          }
          
          /* Specify maximum number of paths to extract */
          else if (strcmp(olong, "max-paths") == 0) {
            if (args_info->max_paths_given) {
              fprintf(stderr, "%s: `--max-paths' (`-p') option given more than once\n", PROGRAM);
            }
            args_info->max_paths_given++;
            args_info->max_paths_arg = (int)atoi(val);
          }
          
          /* Specify maximum number of configuration-pop operations */
          else if (strcmp(olong, "max-ops") == 0) {
            if (args_info->max_ops_given) {
              fprintf(stderr, "%s: `--max-ops' (`-o') option given more than once\n", PROGRAM);
            }
            args_info->max_ops_given++;
            args_info->max_ops_arg = (int)atoi(val);
          }
          
          /* Specify maximum weight of extractable paths */
          else if (strcmp(olong, "max-weight") == 0) {
            if (args_info->max_weight_given) {
              fprintf(stderr, "%s: `--max-weight' (`-w') option given more than once\n", PROGRAM);
            }
            args_info->max_weight_given++;
            args_info->max_weight_arg = (float)strtod(val, NULL);
          }
          
          /* Respect AT&T regex-style escapes in input strings */
          else if (strcmp(olong, "att-mode") == 0) {
            if (args_info->att_mode_given) {
              fprintf(stderr, "%s: `--att-mode' (`-a') option given more than once\n", PROGRAM);
            }
            args_info->att_mode_given++;
           if (args_info->att_mode_given <= 1)
             args_info->att_mode_flag = !(args_info->att_mode_flag);
          }
          
          /* Use old lookup & paths routine (for debugging) */
          else if (strcmp(olong, "debug") == 0) {
            if (args_info->debug_given) {
              fprintf(stderr, "%s: `--debug' (`-d') option given more than once\n", PROGRAM);
            }
            args_info->debug_given++;
           if (args_info->debug_given <= 1)
             args_info->debug_flag = !(args_info->debug_flag);
          }
          
          /* Specify output file (default=stdout). */
          else if (strcmp(olong, "output") == 0) {
            if (args_info->output_given) {
              fprintf(stderr, "%s: `--output' (`-F') option given more than once\n", PROGRAM);
            }
            args_info->output_given++;
            if (args_info->output_arg) free(args_info->output_arg);
            args_info->output_arg = gog_strdup(val);
          }
          
          else {
            fprintf(stderr, "%s: unknown long option '%s'.\n", PROGRAM, olong);
            return (EXIT_FAILURE);
          }
          break;

        case '?':	 /* Invalid Option */
          fprintf(stderr, "%s: unknown option '%s'.\n", PROGRAM, olong);
          return (EXIT_FAILURE);


        default:	/* bug: options not considered.  */
          fprintf (stderr, "%s: option unknown: %c\n", PROGRAM, oshort);
          abort ();
        } /* switch */
  return 0;
}


/* Initialize options not yet given from environmental defaults */
void
cmdline_parser_envdefaults(struct gengetopt_args_info *args_info)
{
  

  return;
}


/* Load option values from an .rc file */
void
cmdline_parser_read_rcfile(const char *filename,
			      struct gengetopt_args_info *args_info,
			      int user_specified)
{
  char *fullname;
  FILE *rcfile;

  if (!filename) return; /* ignore NULL filenames */

#if defined(HAVE_GETUID) && defined(HAVE_GETPWUID)
  if (*filename == '~') {
    /* tilde-expansion hack */
    struct passwd *pwent = getpwuid(getuid());
    if (!pwent) {
      fprintf(stderr, "%s: user-id %d not found!\n", PROGRAM, getuid());
      return;
    }
    if (!pwent->pw_dir) {
      fprintf(stderr, "%s: home directory for user-id %d not found!\n", PROGRAM, getuid());
      return;
    }
    fullname = (char *)malloc(strlen(pwent->pw_dir)+strlen(filename));
    strcpy(fullname, pwent->pw_dir);
    strcat(fullname, filename+1);
  } else {
    fullname = gog_strdup(filename);
  }
#else /* !(defined(HAVE_GETUID) && defined(HAVE_GETPWUID)) */
  fullname = gog_strdup(filename);
#endif /* defined(HAVE_GETUID) && defined(HAVE_GETPWUID) */

  /* try to open */
  rcfile = fopen(fullname,"r");
  if (!rcfile) {
    if (user_specified) {
      fprintf(stderr, "%s: warning: open failed for rc-file '%s': %s\n",
	      PROGRAM, fullname, strerror(errno));
    }
  }
  else {
   cmdline_parser_read_rc_stream(rcfile, fullname, args_info);
  }

  /* cleanup */
  if (fullname != filename) free(fullname);
  if (rcfile) fclose(rcfile);

  return;
}


/* Parse option values from an .rc file : guts */
#define OPTPARSE_GET 32
void
cmdline_parser_read_rc_stream(FILE *rcfile,
				 const char *filename,
				 struct gengetopt_args_info *args_info)
{
  char *optname  = (char *)malloc(OPTPARSE_GET);
  char *optval   = (char *)malloc(OPTPARSE_GET);
  size_t onsize  = OPTPARSE_GET;
  size_t ovsize  = OPTPARSE_GET;
  size_t onlen   = 0;
  size_t ovlen   = 0;
  int    lineno  = 0;
  char c;

#ifdef cmdline_parser_DEBUG
  fprintf(stderr, "cmdline_parser_read_rc_stream('%s'):\n", filename);
#endif

  while ((c = fgetc(rcfile)) != EOF) {
    onlen = 0;
    ovlen = 0;
    lineno++;

    /* -- get next option-name */
    /* skip leading space and comments */
    if (isspace(c)) continue;
    if (c == '#') {
      while ((c = fgetc(rcfile)) != EOF) {
	if (c == '\n') break;
      }
      continue;
    }

    /* parse option-name */
    while (c != EOF && c != '=' && !isspace(c)) {
      /* re-allocate if necessary */
      if (onlen >= onsize-1) {
	char *tmp = (char *)malloc(onsize+OPTPARSE_GET);
	strcpy(tmp,optname);
	free(optname);

	onsize += OPTPARSE_GET;
	optname = tmp;
      }
      optname[onlen++] = c;
      c = fgetc(rcfile);
    }
    optname[onlen++] = '\0';

#ifdef cmdline_parser_DEBUG
    fprintf(stderr, "cmdline_parser_read_rc_stream('%s'): line %d: optname='%s'\n",
	    filename, lineno, optname);
#endif

    /* -- get next option-value */
    /* skip leading space */
    while ((c = fgetc(rcfile)) != EOF && isspace(c)) {
      ;
    }

    /* parse option-value */
    while (c != EOF && c != '\n') {
      /* re-allocate if necessary */
      if (ovlen >= ovsize-1) {
	char *tmp = (char *)malloc(ovsize+OPTPARSE_GET);
	strcpy(tmp,optval);
	free(optval);
	ovsize += OPTPARSE_GET;
	optval = tmp;
      }
      optval[ovlen++] = c;
      c = fgetc(rcfile);
    }
    optval[ovlen++] = '\0';

    /* now do the action for the option */
    if (cmdline_parser_parse_option('\0',optname,optval,args_info) != 0) {
      fprintf(stderr, "%s: error in file '%s' at line %d.\n", PROGRAM, filename, lineno);
      
    }
  }

  /* cleanup */
  free(optname);
  free(optval);

  return;
}
