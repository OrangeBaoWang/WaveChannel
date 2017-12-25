#include "wt_proto_physical_layer.h"
#include "kiss_fft/kiss_fft.h"
#define PI                      3.1415926535897932384626433832795028841971 
#define MAX_FREQ_MISTAKE                       ((int)(1000.0/FREQ_ANALYZE_SAMPLE_TIME_MS))


static int format_freq_list_[FREQ_LIST_LEN] = FREQ_LIST;
static double theta = 0;

static int GetPcmMaxAmplitudeFreq(const RecvAudioType *pcm_buf, int len, int threshold)
{
  kiss_fft_cpx *in_data = NULL;
  kiss_fft_cpx *out_data = NULL;
  kiss_fft_cfg fft_cfg = NULL;
  in_data = (kiss_fft_cpx *)malloc(sizeof(kiss_fft_cpx)*len);
  if (in_data == NULL) {
    goto error_exit;
  }
  out_data = (kiss_fft_cpx *)malloc(sizeof(kiss_fft_cpx)*len);
  if (out_data == NULL) {
    goto error_exit;
  }
  int i;
  for (i = 0; i < len; i++) {
    in_data[i].r = (float)pcm_buf[i];
    in_data[i].i = 0;
  }
  //printf("################################################\n");
  fft_cfg = kiss_fft_alloc(len, 0, NULL, NULL);
  if (fft_cfg == NULL) {
    goto error_exit;
  }
  kiss_fft(fft_cfg, in_data, out_data);

  int size = len / 2;
  double max_item = 0;
  int max_item_mark = 0;
  for (i = 0; i < size; i++) {
    double out_data_item = sqrt(pow(out_data[i].r, 2) + pow(out_data[i].i, 2));
    if (out_data_item > max_item) {
      max_item = out_data_item;
      max_item_mark = i;
    }
  }
  max_item = max_item / (len / 2);
  if (max_item < threshold) {
    goto error_exit;
  }
  free(in_data);
  free(out_data);
  KISS_FFT_FREE(fft_cfg);
  kiss_fft_cleanup();
  return (max_item_mark*RECV_SAMPLE_RATE) / len;

error_exit:
  if (in_data != NULL) {
    free(in_data);
  }
  if (out_data != NULL) {
    free(out_data);
  }
  if (fft_cfg != NULL) {
    KISS_FFT_FREE(fft_cfg);
  }
  kiss_fft_cleanup();
  return -1;
}

static int FreqToFreqMark(int fft_freq, WTPhyFreqMarkType *mark)
{
  int max_freq_mistake = MAX_FREQ_MISTAKE;
  if (fft_freq<format_freq_list_[0] - max_freq_mistake || fft_freq > format_freq_list_[FREQ_LIST_LEN - 1] + max_freq_mistake) {
    return -1;
  }
  if (fft_freq < format_freq_list_[0]) {
    *mark = 0;
    return 0;
  }
  if (fft_freq > format_freq_list_[FREQ_LIST_LEN - 1]) {
    *mark = FREQ_LIST_LEN - 1;
    return 0;
  }
  int left = 0;
  int right = FREQ_LIST_LEN - 1;
  while (right - left > 1) {
    int mid = (left + right) / 2;
    if (fft_freq < format_freq_list_[mid]) {
      right = mid;
    }
    else {
      left = mid;
    }
  }
  int left_inval = fft_freq - format_freq_list_[left];
  int right_inval = format_freq_list_[right] - fft_freq;
  if (left_inval > max_freq_mistake&&right_inval > max_freq_mistake) {
    return -1;
  }
  if (left_inval < right_inval) {
    *mark = (WTPhyFreqMarkType)left;
  }
  else {
    *mark = (WTPhyFreqMarkType)right;
  }
  return 0;
}

static int FreqMarkToFreq(WTPhyFreqMarkType freq_mark, int *freq)
{
  if (freq_mark >= 0 && freq_mark < FREQ_LIST_LEN) {
    *freq = format_freq_list_[freq_mark];
    return 0;
  }
  return -1;
}

static int EncodeSound(int freq, void *buffer, int buffer_length,int sample_bit,int sample_rate)
{


  double amplitude;
  double theta_increment = 2.0 * PI * (freq) / (sample_rate);
  int frame;
  switch (sample_bit) {
    case 8:amplitude = (double)((127 * AUDIO_AMPLITUDE_SCALE) / 100); break;
    case 16:amplitude = (double)((32767 * AUDIO_AMPLITUDE_SCALE) / 100); break;
    default:return -1;
  }

  for (frame = 0; frame < (buffer_length/(sample_bit/8)); frame++) {

    switch (sample_bit) {
      case 8:
        ((signed char *)buffer)[frame] = (signed char)(sin(theta) * amplitude); break;
      case 16:
        ((signed short *)buffer)[frame] = (short)(sin(theta) * amplitude); break;
      default:
        return -1;
    }
    theta += theta_increment;

    if (theta > 2.0 * PI) {
      theta -= 2.0 * PI;
    }
  }

  return 0;
}

int WTPhysicalPcmToFreqMark(const RecvAudioType * pcm_buf, int pcm_len, WTPhyFreqMarkType * freq_mark)
{
  int threshold = 50;
  int freq;
  freq = GetPcmMaxAmplitudeFreq(pcm_buf, pcm_len, threshold);
  if (freq == -1) {
    return -1;
  }
  if (FreqToFreqMark(freq, freq_mark) != 0) {
    return -1;
  }
  return 0;
}

int WTPhysicalFreqMarkToPcm(WTPhyFreqMarkType freq_mark, void  *pcm_buf, int pcm_len, int sample_bit,int sample_rate)
{
  int freq;
  if (FreqMarkToFreq(freq_mark, &freq) != 0) {
    return -1;
  }
  if (EncodeSound(freq, pcm_buf, pcm_len,sample_bit, sample_rate) != 0) {
    return -1;
  }
  return 0;
}

int WTPhyAnalysisNumToRealNum(int ana_num)
{
  int remainder = ana_num % (ONE_FREQ_TIME_MS / FREQ_ANALYZE_SAMPLE_TIME_MS);
  int divsor = ana_num / (ONE_FREQ_TIME_MS / FREQ_ANALYZE_SAMPLE_TIME_MS);

  if (remainder >= ((ONE_FREQ_TIME_MS / FREQ_ANALYZE_SAMPLE_TIME_MS) - 1)) {
    remainder = 1;
  }
  else {
    remainder = 0;
  }
  return remainder + divsor;
}
