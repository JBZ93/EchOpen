#include "mbed_assert.h"
#include "myanalogin.h"

#if DEVICE_ANALOGIN

#include "wait_api.h"
#include "cmsis.h"
#include "pinmap.h"
#include "mbed_error.h"
#include "PeripheralPins.h"

ADC_HandleTypeDef myAdcHandle;

void myanalogin_init(analogin_t *obj, PinName pin)
{
#if defined(ADC1)
    static int adc1_inited = 0;
#endif
    // Get the peripheral name from the pin and assign it to the object
    obj->adc = (ADCName)pinmap_peripheral(pin, PinMap_ADC);
    MBED_ASSERT(obj->adc != (ADCName)NC);

    // Get the functions (adc channel) from the pin and assign it to the object
    uint32_t function = pinmap_function(pin, PinMap_ADC);
    MBED_ASSERT(function != (uint32_t)NC);
    obj->channel = STM_PIN_CHANNEL(function);

    // Configure GPIO
    pinmap_pinout(pin, PinMap_ADC);

    // Save pin number for the read function
    obj->pin = pin;

    // Check if ADC is already initialized
    // Enable ADC clock
#if defined(ADC1)
    if ((obj->adc == ADC_1) && adc1_inited) return;
    if (obj->adc == ADC_1) {
        __ADC1_CLK_ENABLE();
        adc1_inited = 1;
    }
#endif

    // Configure ADC
    myAdcHandle.Instance = (ADC_TypeDef *)(obj->adc);
    myAdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;
    myAdcHandle.Init.Resolution            = ADC_RESOLUTION_10B;//ADC_RESOLUTION12b;
    myAdcHandle.Init.ScanConvMode          = DISABLE;
    myAdcHandle.Init.ContinuousConvMode    = ENABLE; // pour continuous mode
    //myAdcHandle.Init.ContinuousConvMode    = DISABLE; // pour single mode
    myAdcHandle.Init.DiscontinuousConvMode = DISABLE;
    myAdcHandle.Init.NbrOfDiscConversion   = 0;
    myAdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    myAdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
    myAdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    myAdcHandle.Init.NbrOfConversion       = 1;
    myAdcHandle.Init.DMAContinuousRequests = DISABLE; // possible parametrage du dma...
    myAdcHandle.Init.EOCSelection          = ADC_EOC_SEQ_CONV ;//EOC_SINGLE_CONV; //enable pour savoir s'il a fini la conversion etant donnée qu'on est en continuous mode
    if (HAL_ADC_Init(&myAdcHandle) != HAL_OK) {
        error("Cannot initialize ADC\n");
    }
    
    ADC_ChannelConfTypeDef sConfig = {0};

    myAdcHandle.Instance = (ADC_TypeDef *)(obj->adc);

    // Configure ADC channel
    sConfig.Rank         = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES ; // valeur single mode  
    sConfig.Offset       = 0;

    switch (obj->channel) {
        case 0:
            sConfig.Channel = ADC_CHANNEL_0;
            break;
        case 1:
            sConfig.Channel = ADC_CHANNEL_1;
            break;
        case 2:
            sConfig.Channel = ADC_CHANNEL_2;
            break;
        case 3:
            sConfig.Channel = ADC_CHANNEL_3;
            break;
        case 4:
            sConfig.Channel = ADC_CHANNEL_4;
            break;
        case 5:
            sConfig.Channel = ADC_CHANNEL_5;
            break;
        case 6:
            sConfig.Channel = ADC_CHANNEL_6;
            break;
        case 7:
            sConfig.Channel = ADC_CHANNEL_7;
            break;
        case 8:
            sConfig.Channel = ADC_CHANNEL_8;
            break;
        case 9:
            sConfig.Channel = ADC_CHANNEL_9;
            break;
        case 10:
            sConfig.Channel = ADC_CHANNEL_10;
            break;
        case 11:
            sConfig.Channel = ADC_CHANNEL_11;
            break;
        case 12:
            sConfig.Channel = ADC_CHANNEL_12;
            break;
        case 13:
            sConfig.Channel = ADC_CHANNEL_13;
            break;
        case 14:
            sConfig.Channel = ADC_CHANNEL_14;
            break;
        case 15:
            sConfig.Channel = ADC_CHANNEL_15;
            break;
        default:
            return ;
    }

    HAL_ADC_ConfigChannel(&myAdcHandle, &sConfig);
    
    printf("End of init... \n");
}

static inline uint16_t myadc_read(analogin_t *obj)
{
    
    HAL_ADC_Start(&myAdcHandle); // Start conversion
    // Wait end of conversion and get value
    if (HAL_ADC_PollForConversion(&myAdcHandle, 10) == HAL_OK) {
        //printf("OK\n");
        return (HAL_ADC_GetValue(&myAdcHandle));
    } else {
        return 0;
    }
}

uint16_t myanalogin_read_u16(analogin_t *obj)
{
    uint16_t value = myadc_read(obj);
    // 12-bit to 16-bit conversion
    value = ((value << 4) & (uint16_t)0xFFF0) | ((value >> 8) & (uint16_t)0x000F);
    return value;
}

float myanalogin_read(analogin_t *obj)
{
    uint16_t value = myadc_read(obj);
    return (float)value * (1.0f / (float)0xFFF); // 12 bits range
}

#endif
