#ifndef __WT_PROTO_PHYSICAL_LAYER_H__
#define __WT_PROTO_PHYSICAL_LAYER_H__
#include "wt_proto_common.h"



typedef unsigned char WTPhyFreqMarkType;


int WTPhysicalPcmToFreqMark(const RecvAudioType *pcm_buf, int pcm_len, WTPhyFreqMarkType *freq_mark);

int WTPhysicalFreqMarkToPcm(WTPhyFreqMarkType freq_mark, void  *pcm_buf, int pcm_len,int sample_bit,int sample_rate);

int WTPhyAnalysisNumToRealNum(int ana_num);








#endif
