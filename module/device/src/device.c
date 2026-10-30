#include <device.h>
#include <util.h>

LDS_DEFINE(device_api);

error_t device_open(device_t * d, ...) {
  ASSERT_RETURN(d && d->api && d->api->open, E_NULL);

  va_list ap;
  va_start(ap, d);
  error_t res = d->api->open(d, ap);
  va_end(ap);

  return res;
}

error_t device_close(device_t * d) {
  ASSERT_RETURN(d && d->api && d->api->close, E_NULL);

  return d->api->close(d);
}

error_t device_read(device_t * d, uint8_t * buf, size_t len) {
  ASSERT_RETURN(d && d->api && d->api->read, E_NULL);

  return d->api->read(d, buf, len);
}

error_t device_write(device_t * d, const uint8_t * buf, size_t len) {
  ASSERT_RETURN(d && d->api && d->api->write, E_NULL);

  return d->api->write(d, buf, len);
}

error_t device_ioctl(device_t * d, int cmd, ...) {
  ASSERT_RETURN(d && d->api && d->api->ioctl, E_NULL);

  va_list ap;
  va_start(ap, cmd);
  error_t res = d->api->ioctl(d, cmd, ap);
  va_end(ap);

  return res;
}

bool device_supports_ioctl(device_t * d, dev_ioctl_t cmd) {
  return device_ioctl(d, DEV_IOCTL_IS_CMD_SUPPORTED, cmd) == E_EXIST;
}
