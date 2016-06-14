#include "mbed.h"
#define NB_BIT 8 // bit used

//              Byte{LSB         ...        MSB}
PinName PIN[NB_BIT]={D3,D4,D5,D6,D7,D11,D12,D13}; //Connection to DAC0800
//      Port        {B ,B ,B ,B ,A ,A  ,A  ,A  }      
gpio_t GPIO[NB_BIT]; //GPIO used

// functions prototype
// test function
void test1(); // test the GPIO with the conversion decimal to binary
void test3(); // test the ramp generator
void test4(); // test the ramp generator between 0V & 1V 
void test5(); // test the algorithms with the function : Calcul_quantum,CalculPente & ValeurCNA_Tension
void test6(); // test the ramp generator between 0V & 1V on 200us duration.
void init_gpioDAC(); // GPIO & DAC0800 initialisation
float CalculPente(float Ampl_max, float Ampl_min, float duree); //mathematic function but not exploitable (no linearity between the theory & the result) 
void ramp(float pente,float pas, bool sens); //general function to choose positiv ramp or negative ramp
void RampNeg(float pente,float pas); // negative ramp generator
void RampPos(float pente,float pas); // positive ramp generator
void EnvoiBit(unsigned char valeur); // decoder value to GPIO
void ramp_01V(float pente,float pas, bool sens); // ramp generator between 0V and 1V
float Calcul_Quantum(float pleine_echelle, int n); // quantum calculator 
unsigned char ValeurCNA_Tension(float Vmin, float vchoisi); // extract the value matched (on 1 byte) to the voltage that you want


int main() {
    
    test6();
    
}

void EnvoiBit(unsigned char valeur){
    for (int i=0;i< NB_BIT ;i++){ // 1 action for every bit
        if(i<=3){ // Pin of portB: D3,D4,D5,D6
            switch((valeur >> i) & 0x01){ //0x01: mask to select the LSB
                case 1: GPIOB->BSRR = GPIO[i].mask; //set at 1
                        break;
                case 0: GPIOB->BSRR = (uint32_t) GPIO[i].mask << 16; // set at 0 
                        break;
            }
        } else{ //Pin of portA: D7,D11,D12,D13
            switch((valeur >> i) & 0x01){
                case 1: GPIOA->BSRR = GPIO[i].mask; //set at 1
                        break;
                case 0: GPIOA->BSRR = (uint32_t) GPIO[i].mask << 16; // set at 0 
                        break;
            }
        }     
    }
    wait_us(1);// synchronisation
}
void init_gpioDAC(){ // GPIO initialisation
    for(int i=0;i<NB_BIT;i++)
        gpio_init_out(&GPIO[i], PIN[i]); 
}

void ramp(float pente,float pas, bool sens){   
    if(sens == false) // select the ramp direction
        RampNeg(pente,pas);
    else RampPos(pente,pas);
}

void RampPos(float pente,float pas){
    float t=0;
    unsigned char valeur= 0;
    unsigned char old_valeur=255;
    while(valeur<=1){// avoid you have a flat at 0V at the ramp beginning
        valeur = (unsigned char) (pente * t);
        t+=pas;
    } 
    t-=pas;
    while(valeur < 255){ 
        valeur= (unsigned char) (pente * t); // next value
        t+=pas; // next state
        if(valeur != old_valeur) EnvoiBit(valeur); // up to date GPIO
        old_valeur=valeur;
    }
    EnvoiBit(0);//reinitialisation
    
}
void RampNeg(float pente,float pas){ // a completer
    float t=0;
    unsigned char valeur= 0;
    unsigned char old_valeur=255;
    while(valeur<=1){// avoid you have a flat at 0V at the ramp beginning
        valeur = (unsigned char) (pente * t);
        t+=pas;
    } 
    t-=pas;
    while(valeur < 255){
        valeur= (unsigned char) pente * t; // next value
        t+=pas; // next state
        if(valeur != old_valeur) EnvoiBit(255-valeur); // up to date GPIO
        old_valeur=valeur;
    }
    EnvoiBit(0);//reinitialisation

}

void ramp_01V(float pente,float pas, bool sens){

    float t=0;
    unsigned char valeur= 0; 
    unsigned char old_valeur=255; 
    while(valeur<=1){// avoid you have a flat at 0V at the ramp beginning
        valeur = (unsigned char) (pente * t);
        t+=pas;
    } 
    t-=pas;
    while(valeur < 32){ // 32 -> value to have an amplitude of 1V 
        valeur = (unsigned char) (pente * t);
        t+=pas;
        if(valeur != old_valeur)EnvoiBit(valeur); // up to date GPIO
        old_valeur=valeur;
    }
    EnvoiBit(0);//reinitialisation
}

float CalculPente(float Ampl_max, float Ampl_min, float duree){
    return ((Ampl_max - Ampl_min)/duree);   // mathematic calcul
}
float Calcul_Quantum(float pleine_echelle, int n){
    int denom=2;
    for (int i=1; i<n;i++) denom*=2; // 2^n
    denom--; //2^n - 1
    return  pleine_echelle / denom ; //q= pleine_echelle / (2^n - 1)
}
unsigned char ValeurCNA_Tension(float Vmin, float vchoisi){
    float q= Calcul_Quantum(8.2,8);// example in this case
    float tempo=0;
    unsigned char result=0;
    while ( ! ((tempo >= vchoisi - q) && (tempo <= vchoisi + q) ) && result<255)  { // Calculate the decimal which correspond to the voltage compare to the quantum
        result ++;
        tempo+=q;
    }
    if ( (vchoisi - tempo) > (tempo + q - vchoisi) ){//precision on the decimal which approch to the voltage choosing
        result ++;
    }
    return result;
}

void test1(){
    unsigned char valeur=0; // type sur 1 octet -> 0 à 255
    init_gpioDAC();
    while (1){
        valeur=~valeur;
        EnvoiBit(valeur);      
        wait_ms(10);
    }    
}
void test3(){
    init_gpioDAC();
    float pente = 10, pas= 0.1;
    bool sens=1; //0: ramp decreasing ; 1 ramp increasing
    while(1) {
        ramp(pente,pas,sens);
        wait_ms(400);
    }    
}
void test4(){ 
    init_gpioDAC();
    float pente = 15, pas= 0.1; // select the duration of the ramp with the variable "pente"
    bool sens=1; //0: ramp decreasing ; 1 ramp increasing
    while(1) {
        ramp_01V(pente,pas,sens);
        wait_ms(400);
    }
}
void test5(){
    printf("****************************************************** \n");
    printf("calcul pente: %f \n", CalculPente(1,0,0.02)); 
    printf("calcul quantum: %f \n", Calcul_Quantum(8.2,8));
    printf("valeur choisie: %d \n",ValeurCNA_Tension(0, 6.5));
    printf("****************************************************** \n");
    wait(1);
}
void test6(){
    printf("****************************************************** \n");
    printf("calcul pente pour 200us: %f \n", CalculPente(1,0,0.0002));
    printf("****************************************************** \n");
    wait(1);
    init_gpioDAC();
    float pente = 0.495f, pas= 0.1f; // Value validate with the measure in the scope (no linearity)
    bool sens=1; //0: decreasing ; 1 : increasing
    while(1) {
        ramp_01V(pente,pas,sens); 
        wait_ms(400);
    }
}

