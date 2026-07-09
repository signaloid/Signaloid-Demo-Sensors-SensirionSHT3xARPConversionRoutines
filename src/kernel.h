/*
 *	Copyright (c) 2026, Signaloid.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

#pragma once


#include <stdint.h>


/*
 *	These constant values are taken from Figure 9 and Figure 10 in
 *	page 9 of SHT3x_ARP Datasheet, 2024-07-03
 */
#define kSensirionSHT3xARPSensorCalibrationConstant1    (-12.5)
#define kSensirionSHT3xARPSensorCalibrationConstant2    (125)
#define kSensirionSHT3xARPSensorCalibrationConstant3    (-66.875)
#define kSensirionSHT3xARPSensorCalibrationConstant4    (218.75)
#define kSensirionSHT3xARPSensorCalibrationConstant5    (-88.375)
#define kSensirionSHT3xARPSensorCalibrationConstant6    (393.75)

#define kSensirionSHT3xARPDefaultInputDistributionVtUniformDistLow          (2.3)
#define kSensirionSHT3xARPDefaultInputDistributionVtUniformDistHigh         (2.7)
#define kSensirionSHT3xARPDefaultInputDistributionVrhUniformDistLow         (2.3)
#define kSensirionSHT3xARPDefaultInputDistributionVrhUniformDistHigh        (2.7)
#define kSensirionSHT3xARPDefaultInputDistributionVsupplyUniformDistLow     (4.8)
#define kSensirionSHT3xARPDefaultInputDistributionVsupplyUniformDistHigh    (5.4)

/*
 *	Input Variables:
 *		kSensirionSHT3xARPInputVariableIndexVrh		: Ratiometric Analog Voltage for humidity measurement (in Volt)
 *		kSensirionSHT3xARPInputVariableIndexVt		: Ratiometric Analog Voltage for temperature measurement (in Volt)
 *		kSensirionSHT3xARPInputVariableIndexVsupply	: Supply Voltage (in Volt)
 */
typedef enum
{
	kSensirionSHT3xARPInputVariableIndexVrh     = 0,
	kSensirionSHT3xARPInputVariableIndexVt      = 1,
	kSensirionSHT3xARPInputVariableIndexVsupply = 2,
	kSensirionSHT3xARPInputVariableIndexMax,
} SensirionSHT3xARPInputVariableIndex;

/*
 *	Output Variables:
 *		kSensirionSHT3xARPOutputVariableIndexCalibratedRelativeHumidity		: Calibrated Relative Humidity (percentage %)
 *		kSensirionSHT3xARPOutputVariableIndexCalibratedTemperatureCelcius	: Calibrated Temperature (in Celsius)
 *		kSensirionSHT3xARPOutputVariableIndexCalibratedTemperatureFahrenheit	: Calibrated Temperature (in Farenheit)
 */
typedef enum
{
	kSensirionSHT3xARPOutputVariableIndexCalibratedRelativeHumidity         = 0,
	kSensirionSHT3xARPOutputVariableIndexCalibratedTemperatureCelcius       = 1,
	kSensirionSHT3xARPOutputVariableIndexCalibratedTemperatureFahrenheit    = 2,
	kSensirionSHT3xARPOutputVariableIndexMax,
} SensirionSHT3xARPOutputVariableIndex;

/**
 *	@brief  Sensor calibration routines taken from Figure 9 and Figure 10 in
 *		page 9 of Datasheet_SHT3x_ARP.pdf, 2024-07-03.
 *
 *	@param  arguments		: Pointer to command line arguments struct.
 *	@param  inputVariables		: The array of input variables used in the calculation.
 *	@param  outputVariables		: An array of output variables. Writes the result to
 *					  `outputVariables[outputSelectValue]`.
 *	@return	double			: Returns the distributional value calculated.
 */
double
SensirionSHT3xARP_calculateOutput(
	uint8_t     outputSelect,
	double *    inputVariables,
	double *    outputVariables
);
