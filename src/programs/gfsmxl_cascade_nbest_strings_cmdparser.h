/* -*- Mode: C -*-
 *
 * File: gfsmxl_cascade_nbest_strings_cmdparser.h
 * Description: Headers for command-line parser struct gengetopt_args_info.
 *
 * File autogenerated by optgen.perl version 0.07.
 *
 */

#ifndef gfsmxl_cascade_nbest_strings_cmdparser_h
#define gfsmxl_cascade_nbest_strings_cmdparser_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * moocow: Never set PACKAGE and VERSION here.
 */

struct gengetopt_args_info {
  char * cascade_arg;	 /* Specify cascade for lookup (default=cascade.gfsc). */
  char * labels_arg;	 /* Specify alphabet for string->label lookup (default=cascade.lab). */
  int max_paths_arg;	 /* Specify maximum number of paths to extract (default=1). */
  int max_ops_arg;	 /* Specify maximum number of configuration-pop operations (default=-1). */
  float max_weight_arg;	 /* Specify maximum weight of extractable paths (default=1e+38). */
  int att_mode_flag;	 /* Respect AT&T regex-style escapes in input strings (default=0). */
  int debug_flag;	 /* Use old lookup & paths routine (for debugging) (default=0). */
  char * output_arg;	 /* Specify output file (default=stdout). (default=-). */

  int help_given;	 /* Whether help was given */
  int version_given;	 /* Whether version was given */
  int cascade_given;	 /* Whether cascade was given */
  int labels_given;	 /* Whether labels was given */
  int max_paths_given;	 /* Whether max-paths was given */
  int max_ops_given;	 /* Whether max-ops was given */
  int max_weight_given;	 /* Whether max-weight was given */
  int att_mode_given;	 /* Whether att-mode was given */
  int debug_given;	 /* Whether debug was given */
  int output_given;	 /* Whether output was given */
  
  char **inputs;         /* unnamed arguments */
  unsigned inputs_num;   /* number of unnamed arguments */
};

/* read rc files (if any) and parse all command-line options in one swell foop */
int  cmdline_parser (int argc, char *const *argv, struct gengetopt_args_info *args_info);

/* instantiate defaults from environment variables: you must call this yourself! */
void cmdline_parser_envdefaults (struct gengetopt_args_info *args_info);

/* read a single rc-file */
void cmdline_parser_read_rcfile (const char *filename,
				    struct gengetopt_args_info *args_info,
				    int user_specified);

/* read a single rc-file (stream) */
void cmdline_parser_read_rc_stream (FILE *rcfile,
				       const char *filename,
				       struct gengetopt_args_info *args_info);

/* parse a single option */
int cmdline_parser_parse_option (char oshort, const char *olong, const char *val,
				    struct gengetopt_args_info *args_info);

/* print help message */
void cmdline_parser_print_help(void);

/* print version */
void cmdline_parser_print_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* gfsmxl_cascade_nbest_strings_cmdparser_h */