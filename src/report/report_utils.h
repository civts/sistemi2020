/*	File for report business logic that is too verbose and would only
 * clutter the main report.c file
*/
#include "../utils.c"

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

// Options that are semantically valid for the report program
const char *validOptions[] = {
    helpFlag, groupFlag, verboseFlag, percentageFlag, forceReAnalysisFlag,
};

// Returns true if args are valid; And terminates w/ code 1 if they are not
// (provides the message to the user)
bool argsAreValid(int argc, char *argv[]) {
  // no arguments is ok (in that case argc is 1)
  int i;
  for (i = 1; i < argc; i++) {
    char *opt = argv[i];
    int a = contains(5, validOptions, opt);
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
      exit(1);
    }
  }
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