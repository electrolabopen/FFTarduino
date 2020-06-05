/*

  Example of use of the FFT libray
        Copyright (C) 2014 Enrique Condes

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*En este ejemplo, el arduino simula el muestreo de una senoidal de 1000Hz 
 * con amplitud de 100, muestreada a 5000Hz. Las muestras son guardadas en el 
 * arreglo VReal. Las muestras son mostradas segun la funcion Hamming. La FFT 
 * es calculada usando las muestras mostradas. Cuando las magnitudes de cada una
 * de las frecuencia que componen la señal, son calculadas. Finalmente, la frecuencia
 * con el pico más alto es obtenida, siendo esa la frecuencia principal presente 
 * en la señal.
 
  In this example, the Arduino simulates the sampling of a sinusoidal 1000 Hz
  signal with an amplitude of 100, sampled at 5000 Hz. Samples are stored
  inside the vReal array. The samples are windowed according to Hamming
  function. The FFT is computed using the windowed samples. Then the magnitudes
  of each of the frequencies that compose the signal are calculated. Finally,
  the frequency with the highest peak is obtained, being that the main frequency
  present in the signal.
*/

#include "arduinoFFT.h"

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
These values can be changed in order to evaluate the functions
*/
const uint16_t muestras = 64; //This value MUST ALWAYS be a power of 2
const double frecuenciasenal;
const double frecuenciamuestreo = 5000;
const uint8_t amplitud = 5;

/*
 * 
These are the input and output vectors
Input vectors receive computed results from FFT
*/
double vReal[muestras];
double vImag[muestras];
unsigned long duracion1; 
unsigned long duracion2; 
int Frequency;
#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void setup()
{
  Serial.begin(38400);//115200
  Serial.println("FFT  ONDA SENOIDAL");
}

void loop()
{
  duracion1 = pulseIn (2, HIGH,100000000); //mide el flanco de subida
duracion2 = pulseIn (2, LOW,100000000); //mide el flanco de bajada
Frequency =(1000000/(duracion1+duracion2)); // cacula el valor de frecuencia
  const double frecuenciasenal =  Frequency;
  /* Build raw data */
  
  double ciclos = (((muestras-1) * frecuenciasenal) / frecuenciamuestreo); //Number of signal cycles that the sampling will read
   
  for (uint16_t i = 0; i < muestras; i++)
  {
    vReal[i] = int8_t((amplitud * (sin((i * (twoPi * ciclos)) / muestras))) / 2.0);/* Build data with positive and negative values*/
    //vReal[i] = uint8_t((amplitude * (sin((i * (twoPi * cycles)) / samples) + 1.0)) / 2.0);/* Build data displaced on the Y axis to include only positive values*/
    vImag[i] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
  }
  /* Print the results of the simulated sampling according to time */
  Serial.println("Data:");
  //PrintVector(vReal, muestras, SCL_TIME);
  FFT.Windowing(vReal, muestras, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  //Serial.println("Weighed data:");
  //PrintVector(vReal, muestras, SCL_TIME);
  FFT.Compute(vReal, vImag, muestras, FFT_FORWARD); /* Compute FFT */
  //Serial.println("Computed Real values:");
  //PrintVector(vReal, muestras, SCL_INDEX);
  //Serial.println("Computed Imaginary values:");
  //PrintVector(vImag, muestras, SCL_INDEX);
  FFT.ComplexToMagnitude(vReal, vImag, muestras); /* Compute magnitudes */
  Serial.println("Computed magnitudes:");
  PrintVector(vReal, (muestras >> 1), SCL_FREQUENCY);
  double x = FFT.MajorPeak(vReal, muestras, frecuenciamuestreo);
  Serial.println("PICO MÁS ALTO ES PARA:");
  Serial.print(x, 3);
  Serial.print(F("Hz"));
  while(1); /* Run Once */
  // delay(2000); /* Repeat after delay */
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscisa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscisa = (i * 1.0);
  break;
      case SCL_TIME:
        abscisa = ((i * 1.0) / frecuenciamuestreo);
  break;
      case SCL_FREQUENCY:
        abscisa = ((i * 1.0 * frecuenciamuestreo) / muestras);
  break;
    }
    Serial.print("DATA,TIME");
    Serial.print(F(", "));
     Serial.print(abscisa, 3);
    if(scaleType==SCL_FREQUENCY)
     
      Serial.print(F("Hz"));
      Serial.print(F(", "));
      Serial.println(vData[i], 3);
        
    
  }
  
  //Serial.println();
}
