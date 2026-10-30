#include <platform.h>
#include <string.h>
#include <fcntl.h>

static error_t cio_open(device_t * d, va_list ap) {
  ASSERT_RETURN(d->ctx, E_NULL);
  dev_cio_ctx_t * ctx = d->ctx;

  ctx->out             = va_arg(ap, void*);
  ctx->in              = va_arg(ap, void*);
  ctx->is_non_blocking = false;
  tcgetattr(fileno(ctx->in), &ctx->orig_tty);

  return E_OK;
}

static error_t cio_close(device_t * d) {
  ASSERT_RETURN(d->ctx, E_NULL);
  dev_cio_ctx_t * ctx = d->ctx;

  if (ctx->is_non_blocking) {
    int fd = fileno(ctx->in);
    tcsetattr(fd, TCSANOW, &ctx->orig_tty);
    ctx->is_non_blocking = false;
  }

  return E_OK;
}

static error_t cio_read(device_t * d, uint8_t * buf, size_t len) {
  ASSERT_RETURN(d->ctx && buf, E_NULL);
  ASSERT_RETURN(len, E_EMPTY);

  dev_cio_ctx_t * ctx = d->ctx;

  ASSERT_RETURN(ctx->in, E_NULL);

  size_t res = fread(buf, 1, len, ctx->in);

  if (res == 0 && ctx->is_non_blocking) {
    clearerr(ctx->in);
    return E_AGAIN;
  }

  return res == len ? E_OK : E_IO;
}

static error_t cio_write(device_t * d, const uint8_t * buf, size_t len) {
  ASSERT_RETURN(d->ctx && buf, E_NULL);
  ASSERT_RETURN(len, E_EMPTY);

  dev_cio_ctx_t * ctx = d->ctx;

  ASSERT_RETURN(ctx->out, E_NULL);

  size_t res = fwrite(buf, 1, len, ctx->out);

  return res == len ? E_OK : E_IO;
}

static error_t cio_ioctl(device_t * d, int cmd, va_list ap) {
  ASSERT_RETURN(d->ctx, E_NULL);
  dev_cio_ctx_t * ctx = d->ctx;

  switch (cmd) {
    case DEV_IOCTL_IS_CMD_SUPPORTED: {
      int c = va_arg(ap, int);
      return
        c == DEV_IOCTL_IS_CMD_SUPPORTED ||
        c == DEV_IOCTL_SET_BLOCKING
          ? E_EXIST
          : E_IO;
    }

    case DEV_IOCTL_SET_BLOCKING: {
      int blocking = va_arg(ap, int);

      if (!blocking == ctx->is_non_blocking) {
        return E_ALREADY;
      }

      int fd = fileno(ctx->in);

      if (blocking) {
        tcsetattr(fd, TCSANOW, &ctx->orig_tty);
      } else {
        struct termios tty;
        memcpy(&tty, &ctx->orig_tty, sizeof(struct termios));

        tty.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(fd, TCSANOW, &tty);

        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
      }

      ctx->is_non_blocking = !blocking;

      return E_OK;
    }

    default:
      break;
  }

  return E_NOTIMPL;
}

DEVICE_API_DECLARE(cio, {
  .open  = cio_open,
  .close = cio_close,
  .read  = cio_read,
  .write = cio_write,
  .ioctl = cio_ioctl,
});
