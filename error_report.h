#ifndef INFOTO_ERROR_REPORT_H
#define INFOTO_ERROR_REPORT_H

/**
 * structure to report back errors
 */
typedef struct {
  char *message;
} error_report;

/**
 * Convenience function to create error_report object.
 *
 * @param[in] msg The message to store in the structure
 * @return A new error report object with the given message
 */
static inline error_report error(char *msg) { return (error_report){msg}; }

#endif
