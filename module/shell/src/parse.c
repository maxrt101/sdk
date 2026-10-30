#include <shell.h>

/**
 * @brief Parses a line into tokens
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
  char * start = sh->buf;

  // Strip leading whitespace
  while (*start == ' ') {
    start++;
  }

  bool is_quoted = false;
  uint16_t i;

  // Iterate until the null-terminator
  for (i = (start - sh->buf); sh->buf[i] != '\0'; ++i) {
    if (sh->args.size >= SHELL_MAX_TOKENS) {
      break;
    }

    char c = sh->buf[i];

    // If quoted, look only for quotes. Otherwise, look for spaces or quotes.
    if (is_quoted ? (c == '"') : (c == ' ' || c == '"')) {

      // Null-terminate the current character to cap off the token
      sh->buf[i] = '\0';

      // Only save the token if it's not empty
      // This ignores consecutive spaces
      if (*start != '\0') {
        sh->args.buf[sh->args.size++] = start;
      }

      // Toggle quote flag if the delimiter was a quote
      if (c == '"') {
        is_quoted = !is_quoted;
      }

      // Move start to the next character
      start = &sh->buf[i+1];
    }
  }

  // Add the final token if the string didn't end on a space delimiter
  if (*start != '\0' && sh->args.size < SHELL_MAX_TOKENS) {
    sh->args.buf[sh->args.size++] = start;
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
