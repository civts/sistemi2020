/*	File for report business logic that is too verbose and would only
 * clutter the main report.c file
*/
#ifndef REPORT_UTILS_H
#define REPORT_UTILS_H
#include "../utils.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Flag for telling the report to show help dialog
const char helpFlag[] = "--help";

// Flag for telling the report to show grouped stats (a-z instead of a,b,c...)
// implies verbose
const char groupFlag[] = "-g";

// Flag for telling the report to show verbose stats
const char verboseFlag[] = "-v";

// Flag for telling the report to show percentage of occourrences instead of
// count
const char percentageFlag[] = "-p";

// Flag for telling the report to force another analysis
const char forceReAnalysisFlag[] = "-r";

// Flag for telling the report to give you only the stats for a given file
const char onlyFlag[] = "--only";

// Options that are semantically valid for the report program
const char *validOptions[] = {
    helpFlag, groupFlag, verboseFlag, percentageFlag, forceReAnalysisFlag,
};
const int validOptions_length = 5;

// Returns true if args are valid; And terminates w/ code 1 if they are not
// (provides the message to the user)
bool argsAreValid(int argc, const char *argv[]) {
  bool shouldTerminate = false;
  // no arguments is ok (in that case argc is 1)
  int i;
  if (contains(argc, argv, onlyFlag)) {
    bool invalid = false;
    if (streq(argv[1], groupFlag) && streq(argv[2], onlyFlag)) {
      // If the program was called with the -g --only flag combo I check that
      // there are no other falgs after them
      for (i = 3; i < argc; i++) {
        if (contains(validOptions_length, validOptions, argv[i])) {
          invalid = true;
          break;
        }
      }
    } else if (streq(argv[1], onlyFlag)) {
      // If the program was called with just the --only flag I check that there
      // are no other falgs after it
      for (i = 2; i < argc; i++) {
        if (contains(validOptions_length, validOptions, argv[i])) {
          invalid = true;
          break;
        }
      }
    }
    if (invalid) {
      perror("Invalid use of the --only option. Please use --help to see "
             "expected syntax.\n");
      shouldTerminate = true;
    }
  } else {
    // Cheking that each option is a valid one
    for (i = 1; i < argc; i++) {
      const char *opt = argv[i];
      bool a = contains(validOptions_length, validOptions, opt);
      if (!a) {
        char msgStart[] = "Option \"";
        char msgEnd[] = "\" is not valid, use --help to see available ones\n";
        int messageLength = strlen(msgStart) + strlen(opt) + strlen(msgEnd) + 1;
        char msg[messageLength];
        msg[0] = '\0';
        strcat(msg, msgStart);
        strcat(msg, opt);
        strcat(msg, msgEnd);
        perror(msg);
        shouldTerminate = true;
      }
    }
  }
  if (shouldTerminate)
    exit(1);
  return true;
}

// Unit test main
// void main() {
//   char uno[] = "main";
//   char due[] = "-r";
//   char tre[] = "--help";
//   char quattro[] = "-v";
//   char *a[4];
//   a[0] = uno;
//   a[1] = due;
//   a[2] = tre;
//   a[3] = quattro;
//   int b = argsAreValid(4, a);
//   printf("%d\n", b);
// }
#endif
