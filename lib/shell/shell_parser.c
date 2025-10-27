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
 * @param ctx Shell context
 */
void shell_parse(shell_t * ctx) {
  ctx->ptr.size = 0;

  // Start of current token
  char * start = ctx->line.buf;

  // Flag to know if quoted token is being processed
  bool is_quoted = false;

  uint16_t i;

  for (i = 0; i < ctx->line.size; ++i) {
    char c = ctx->line.buf[i];

    if (ctx->ptr.size >= SHELL_MAX_TOKENS) {
      break;
    }

    // If quotes are being processed, check only for ending quotes, if no
    // check for space, NULL character (end of string) and starting quotes
    if (is_quoted ? (c == '"') : (c == ' ' || c == '\0' || c == '"')) {
      // If previous token is empty, overwrite it
      // This is necessary, as multiple spaces are not skipped, and each one,
      // except for the last will create an empty token
      if (ctx->ptr.size && ctx->ptr.buf[ctx->ptr.size-1][0] == '\0') {
        ctx->ptr.size -= 1;
      }

      // Update quoted flag
      // If flag is set, quoted token was processed, so if execution has
      // reached here, it means this is the end of quoted token, so reset the
      // flag
      // If flag is not set, check if current char is quotes, if it is, this
      // marks the start of quoted token
      is_quoted = is_quoted ? false : c == '"';

      // Save previous token to the array
      ctx->ptr.buf[ctx->ptr.size++] = start;

      // Terminate previous token
      ctx->line.buf[i] = '\0';

      // Set next token start
      start = &ctx->line.buf[i+1];
    }
  }

  // Terminate last token, if it's present
  if (ctx->line.buf[i-1]) {
    ctx->ptr.buf[ctx->ptr.size++] = start;
    ctx->line.buf[i] = '\0';
  }

  // If last token is empty, overwrite it
  // See explanation above
  if (ctx->ptr.size && ctx->ptr.buf[ctx->ptr.size-1][0] == '\0') {
    ctx->ptr.size -= 1;
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
