#ifndef __WT_SEND_PHYSICAL_LAYER_H__
#define __WT_SEND_PHYSICAL_LAYER_H__
#include "proto_utils/wt_proto_physical_layer.h"
#include "proto_utils/wt_proto_link_layer.h"
#include "transceiver/send/wt_send_link_layer.h"

typedef struct {
  void      *data_;
}WTSendPhyHander;

typedef struct {
  void          *buff_;
  int           buff_len_;
  int           sample_rate_;
  int           sample_bit_;
}WTSendPcmBuffType;

typedef struct {
  int           sample_rate_;
  int           sample_bit_;
}WTSendPhyHanderAttr;

WTSendPhyHander *WTSendPhyLayerCreatHander(WTSendPhyHanderAttr *attr);

void WTSendPhyLayerDestroyHander(WTSendPhyHander * hander);

WTSendPcmBuffType * WTSendPhyLayerGetPcm(WTSendPhyHander *hander, WTSendLinkPackageS *packages);

void WTSendPhyLayerReleasePcm(WTSendPhyHander *handler);

#endif

