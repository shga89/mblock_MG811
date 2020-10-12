/**
    Powered by Smart Technology Benin
    
    @autor: AMOUSSOU Z. Kenneth
    @date: 14-09-2018
    @version: 1.0
*/
#include "MG811.h"

/**
    Constructor
*/
MG811::MG811(uint8_t input = A0){
    _input = input;
}

/**
    function: begin
    - summary: 보정과정에서 측정한 값을 설정
    - parameter:
        v400: CO2가 400ppm일 때의 측정 값 (환기가 잘 되는 개방된 공간에서 측정)
        v40000: CO2가 40000ppm일 때의 측정 값 (날숨을 밀폐된 봉지에 모아서 그 안에 센서를 넣고 측정)
    - return: 없음
*/
void MG811::begin(float v400, float v40000){
    _V400 = v400;
    _V40000 = v40000;
}

/**
    function: raw
    @summary: measure multiple raw data from the sensor and compute the mean
    @parameter: none
    @return:
        float: return the voltage measured from the sensor
*/
float MG811::raw(){
    uint8_t i = 0;
    float buffer = 0;
    for(i = 0; i < 20; i++){
        buffer += analogRead(_input);
        delay(50); // 20ms  
    }
    buffer /= i; // 10개를 모아서 평균
    //return map(buffer, 0, 1023, 0, 5);  //0~1023을 0~5 범위로 변환
	return buffer; //값이 너무 작아서 이걸로 바꿈. 석현
}

/**
    function: read
    @summary: measure voltage from the sensor and compute the CO2 ppm value
    @parameter: none
    @return:
        float: return the CO2 concentration in 'ppm'
        
        Formulas:
            deltaV = (V400 - V40000)
            A = deltaV/(log10(400) - log10(40000))
            B = log10(400)
            C = (<measurement from sensor in volt> - V400)/A + B
            <CO2 ppm> = pow(10, C)
*/
float MG811::read(){
    float buffer = 0;
    buffer = (_V400 - _V40000)/(log10(400) - log10(40000)); // Delta V
    buffer = (raw() - _V400)/buffer;
    buffer += log10(400);
    return pow(10, buffer);
}

/**
    function: calibrate
    @summary: calibrate the sensor to get reference value for measurement
              
              Power on the sensor
              
              [0] Put the sensor outdoor or indoor with good ventilation
                  Wait at least two (02) hours - for warming up
                  Read it's measurement - You get 400ppm reference voltage
              
              [1] Put the sensor in a bag filled with exhaled air
                  Wait a couple of minutes
                  Read it's measurement - You get 40000ppm reference voltage
              
              The reference value measured by this function should be used with 
              the `begin` method in order to use tthe sensor
              
    @see: this function needs Serial communication to be enabled to print out   
          information
          
          Serial.begin(9600)
    @parameter: none
    @return: none
*/
void MG811::calibrate(){
    uint8_t i = 0;
    Serial.println("Time (mn) \t\t Measurement (volt)");
    for(i = 0; i < 120; i++){
        delay(60000); // wait 1 minute
        Serial.print(i);
        Serial.print("\t\t ");
        Serial.println(raw(), 3);
    }
}


