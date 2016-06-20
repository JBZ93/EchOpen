#include "mbed.h"
#include "myanalogin.h"
AnalogIn ana_in(A0);
analogin_t adc;
extern ADC_HandleTypeDef myAdcHandle;
Ticker timer;
Timer temp;
void test1(){ // mesure le temps effectuer 480 echantillons
    float tempo[480]; // 480 nb echantillons pendant 200us en theorie
    temp.start();
    for (int i = 0; i < 480; i++)
    tempo[i]=ana_in.read();
    temp.stop();
    printf("duree: %d us \n", temp.read_us()); // 1,776 ms

}
void test2(){
    //premier temps: Single mode //977us
    myanalogin_init(&adc,A1);
    wait_ms(10);
    HAL_ADC_Start(&myAdcHandle); // pour le continuous mode uniquement
    uint32_t tempo2[480]; // pour le continuous mode uniquement
    float tempo[480]; // 480 nb echantillons pendant 200us en theorie
    temp.start();
    for (int i = 0; i < 480; i++){
    //tempo[i]=myanalogin_read(&adc); // single mode
    //tempo2[i]=HAL_ADC_GetValue(&myAdcHandle); // continuous mode // 104us -> la conversion ne se fait probablement pas entierement -> changer le sampletime
        //HAL_ADC_Start(&myAdcHandle);
        tempo2[i]=HAL_ADC_GetValue(&myAdcHandle); // single mode V2 // 456us-> dememe on ne garanti que la conversion est faire entierement
        //while(!(__HAL_ADC_GET_FLAG(&myAdcHandle, ADC_FLAG_EOC)));
        HAL_ADC_PollForConversion(&myAdcHandle, 5);
        //wait_us(1);
    }
    temp.stop();
    printf("*********************");
    for (int i = 0; i < 480; i++){
        printf("%f \n",((float)tempo2[i] * (1.0f / (float)0xFFF))*3300);    
    }
    
    printf("duree: %d us \n", temp.read_us()); 

}
int main() {

    test2();
}
