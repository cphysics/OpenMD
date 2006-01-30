/* DynamicPropsCmd.h */

/* File autogenerated by gengetopt version 2.11  */

#ifndef DYNAMICPROPSCMD_H
#define DYNAMICPROPSCMD_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
#define CMDLINE_PARSER_PACKAGE "DynamicProps"
#endif

#ifndef CMDLINE_PARSER_VERSION
#define CMDLINE_PARSER_VERSION "1.00"
#endif

struct gengetopt_args_info
{
  char * input_arg;	/* input dump file.  */
  char * output_arg;	/* output file name.  */
  char * sele1_arg;	/* select first stuntdouble set.  */
  char * sele2_arg;	/* select second stuntdouble set (if sele2 is not set, use script from sele1).  */
  int order_arg;	/* Lengendre Polynomial Order.  */

  int help_given ;	/* Whether help was given.  */
  int version_given ;	/* Whether version was given.  */
  int input_given ;	/* Whether input was given.  */
  int output_given ;	/* Whether output was given.  */
  int sele1_given ;	/* Whether sele1 was given.  */
  int sele2_given ;	/* Whether sele2 was given.  */
  int order_given ;	/* Whether order was given.  */
  int rcorr_given ;	/* Whether rcorr was given.  */
  int vcorr_given ;	/* Whether vcorr was given.  */
  int dcorr_given ;	/* Whether dcorr was given.  */
  int lcorr_given ;	/* Whether lcorr was given.  */

} ;

int cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info);

void cmdline_parser_print_help(void);
void cmdline_parser_print_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* DYNAMICPROPSCMD_H */
