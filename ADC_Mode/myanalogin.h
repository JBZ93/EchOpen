
#include "device.h"

#if DEVICE_ANALOGIN

#ifdef __cplusplus
extern "C" {
#endif

typedef struct analogin_s analogin_t;

void     myanalogin_init    (analogin_t *obj, PinName pin);
static inline uint16_t myadc_read(analogin_t *obj);
float myanalogin_read(analogin_t *obj);
uint16_t myanalogin_read_u16(analogin_t *obj);

#ifdef __cplusplus
}
#endif

#endif

