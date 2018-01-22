#include "wave_trans_recv.h"
#include "transceiver/recv/wt_recv_link_layer.h"
#include "transceiver/recv/wt_recv_physical_layer.h"

#ifdef __cplusplus
extern "C" {
#endif

int WaveTransRecvInit(void)
{
#ifdef FREQ_MODE_MUX
  if (WTRecvLinkLayerInitForMix() != 0) {
    return -1;
  }
  if (WTRecvPhyLayerInitForMixing() != 0) {
    WTRecvLinkLayerExitForMix();
    return -1;
  }
  return 0;
#else
#ifdef FREQ_MODE_COMPARE
  if (WTRecvLinkLayerInitForCompare() != 0) {
    return -1;
  }
  if (WTRecvPhyLayerInitForCompare() != 0) {
    WTRecvLinkLayerExitForCompare();
    return -1;
  }
  return 0;
#else
  if (WTRecvLinkLayerInit() != 0) {
    return -1;
  }
  if (WTRecvPhyLayerInit() != 0) {
    WTRecvLinkLayerExit();
    return -1;
  }
  return 0;
#endif
#endif
}

void WaveTransRecvExit(void)
{
#ifdef FREQ_MODE_MUX
  WTRecvPhyLayerExitForMixing();
  WTRecvLinkLayerExitForMix();
#else
#ifdef FREQ_MODE_COMPARE
  WTRecvPhyLayerExitForCompare();
  WTRecvLinkLayerExitForCompare();
#else
  WTRecvPhyLayerExit();
  WTRecvLinkLayerExit();
#endif
#endif
}

void WaveTransRecvSetPcm(const RecvAudioType * pcm, int pcm_len)
{
#ifdef FREQ_MODE_MUX
  WTRecvPhyLayerSendPcmForMixing(pcm, pcm_len);
#else
#ifdef FREQ_MODE_COMPARE
  WTRecvPhyLayerSendPcmForCompare(pcm,pcm_len);
#else
  WTRecvPhyLayerSendPcm(pcm, pcm_len);
#endif
#endif
}

int WaveTransRecvGetContext(void * context, int context_len)
{
#ifdef FREQ_MODE_MUX
  return WTRecvLinkLayerGetDataForMix(context, context_len);
#else
#ifdef FREQ_MODE_COMPARE
  return WTRecvLinkLayerGetDataForCompare(context, context_len);
#else
  return WTRecvLinkLayerGetData(context, context_len);
#endif
#endif
}


#ifdef __cplusplus
}
#endif