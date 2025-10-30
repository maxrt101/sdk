/** ========================================================================= *
 *
 * @file shell_parser.c
 * @date 09-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Implementation of shell input functions (get_line & parse)
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "error/assertion.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG shell

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
/**
 * Parses a line into tokens
 *
 * Handles double quotes (""), anything between them will be treated as 1 token
 *
 * Basically finds token borders and writes NULL between them, then stores
 * pointers to the start of each token into an array
 *
 * @param sh Shell context
 */
void shell_parse(shell_t * sh) {
  sh->args.size = 0;

  // Start of current token
  char * start = sh->line.buf;

  // Flag to know if quoted token is being processed
  bool is_quoted = false;

  uint16_t i;

  for (i = 0; i < sh->line.size; ++i) {
    char c = sh->line.buf[i];

    if (sh->args.size >= SHELL_MAX_TOKENS) {
      break;
    }

    // If quotes are being processed, check only for ending quotes, if no
    // check for space, NULL character (end of string) and starting quotes
    if (is_quoted ? (c == '"') : (c == ' ' || c == '\0' || c == '"')) {
      // If previous token is empty, overwrite it
      // This is necessary, as multiple spaces are not skipped, and each one,
      // except for the last will create an empty token
      if (sh->args.size && sh->args.buf[sh->args.size-1][0] == '\0') {
        sh->args.size -= 1;
      }

      // Update quoted flag
      // If flag is set, quoted token was processed, so if execution has
      // reached here, it means this is the end of quoted token, so reset the
      // flag
      // If flag is not set, check if current char is quotes, if it is, this
      // marks the start of quoted token
      is_quoted = is_quoted ? false : c == '"';

      // Save previous token to the array
      sh->args.buf[sh->args.size++] = start;

      // Terminate previous token
      sh->line.buf[i] = '\0';

      // Set next token start
      start = &sh->line.buf[i+1];
    }
  }

  // Terminate last token, if it's present
  if (sh->line.buf[i-1]) {
    sh->args.buf[sh->args.size++] = start;
    sh->line.buf[i] = '\0';
  }

  // If last token is empty, overwrite it
  // See explanation above
  if (sh->args.size && sh->args.buf[sh->args.size-1][0] == '\0') {
    sh->args.size -= 1;
  }

#if SHELL_DEBUG_PRINT_TOKENS
  for (uint8_t i = 0; i < ctx->ptr.size; ++i) {
    log_printf("'%s' ", ctx->ptr.buf[i]);
  }
  log_printf("\r\n");
#endif

#if SHELL_DEBUG_DEBUG_TOKENS_HEX
  for (uint8_t i = 0; i < ctx->ptr.size; ++i) {
    log_printf("'");
    for (uint8_t j = 0; ctx->ptr.buf[i][j]; ++j) {
      log_printf("%02x", ctx->ptr.buf[i][j]);
      if (ctx->ptr.buf[i][j+1]) {
        log_printf(" ");
      }
    }
    log_printf("' ");
  }
  log_printf("\r\n");
#endif
}
