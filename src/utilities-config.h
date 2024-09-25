/*
 *	Copyright (c) 2024, Signaloid.
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

/*
 *	These constant values are taken from Figure 9 and Figure 10 in
 *	page 9 of SHT3x_ARP Datasheet, 2024-07-03
 */
#define kSensorCalibrationConstant1				(-12.5)
#define kSensorCalibrationConstant2				(125)
#define kSensorCalibrationConstant3				(-66.875)
#define kSensorCalibrationConstant4				(218.75)
#define kSensorCalibrationConstant5				(-88.375)
#define kSensorCalibrationConstant6				(393.75)

#define kDefaultInputDistributionVtUniformDistLow		(2.3)
#define kDefaultInputDistributionVtUniformDistHigh		(2.7)
#define kDefaultInputDistributionVrhUniformDistLow		(2.3)
#define kDefaultInputDistributionVrhUniformDistHigh		(2.7)
#define kDefaultInputDistributionVsupplyUniformDistLow		(4.8)
#define kDefaultInputDistributionVsupplyUniformDistHigh		(5.4)

/*
 *	Input Distributions:
 *		kInputDistributionIndexVrh	: Ratiometric Analog Voltage for humidity measurement (in Volt)
 *		kInputDistributionIndexVt	: Ratiometric Analog Voltage for temperature measurement (in Volt)
 *		kInputDistributionIndexVsupply	: Supply Voltage (in Volt)
 */
typedef enum
{
	kInputDistributionIndexVrh	= 0,
	kInputDistributionIndexVt	= 1,
	kInputDistributionIndexVsupply	= 2,
	kInputDistributionIndexMax,
} InputDistributionIndex;

/*
 *	Output Distributions:
 *		kOutputDistributionIndexCalibratedRelativeHumidity	: Calibrated Relative Humidity (percentage %)
 *		kOutputDistributionIndexCalibratedTemperatureCelcius	: Calibrated Temperature (in Celsius)
 *		kOutputDistributionIndexCalibratedTemperatureFahrenheit	: Calibrated Temperature (in Farenheit)
 */
typedef enum
{
	kOutputDistributionIndexCalibratedRelativeHumidity	= 0,
	kOutputDistributionIndexCalibratedTemperatureCelcius	= 1,
	kOutputDistributionIndexCalibratedTemperatureFahrenheit	= 2,
	kOutputDistributionIndexMax,
} OutputDistributionIndex;
