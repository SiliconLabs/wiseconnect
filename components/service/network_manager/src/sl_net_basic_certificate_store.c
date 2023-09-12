#include "sl_net.h"

sl_status_t sl_net_set_certificate(sl_net_certificate_id_t id, const void *data, uint32_t data_length)
{
  UNUSED_PARAMETER(id);
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(data_length);
  return SL_STATUS_NOT_SUPPORTED;
}
sl_status_t sl_net_get_certificate(sl_net_certificate_id_t id, void *data, uint32_t data_length)
{
  UNUSED_PARAMETER(id);
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(data_length);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_net_verify_certificate()
{
  return SL_STATUS_NOT_SUPPORTED;
}
