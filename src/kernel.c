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

#include <stdbool.h>
#include "kernel.h"


double
SensirionSHT3xARP_calculateOutput(uint8_t outputSelect, double *  inputVariables, double *  outputVariables)
{
	double  Rh;
	double  Tcelcius;
	double  Tfahrenheit;
	double  Vsupply;
	double  Vt;
	double  Vrh;
	double  calibratedValue = 0.0;

	bool calculateAllOutputs = (outputSelect == kSensirionSHT3xARPOutputVariableIndexMax);

	Vsupply = inputVariables[kSensirionSHT3xARPInputVariableIndexVsupply];
	Vt      = inputVariables[kSensirionSHT3xARPInputVariableIndexVt];
	Vrh     = inputVariables[kSensirionSHT3xARPInputVariableIndexVrh];

	if (calculateAllOutputs ||
	    (outputSelect == kSensirionSHT3xARPOutputVariableIndexCalibratedRelativeHumidity))
	{
		Rh              = kSensirionSHT3xARPSensorCalibrationConstant1 + kSensirionSHT3xARPSensorCalibrationConstant2 * (Vrh / Vsupply);
		calibratedValue = outputVariables[kSensirionSHT3xARPOutputVariableIndexCalibratedRelativeHumidity] = Rh;
	}

	if (calculateAllOutputs ||
	    (outputSelect == kSensirionSHT3xARPOutputVariableIndexCalibratedTemperatureCelcius))
	{
		Tcelcius        = kSensirionSHT3xARPSensorCalibrationConstant3 + kSensirionSHT3xARPSensorCalibrationConstant4 * (Vt / Vsupply);
		calibratedValue = outputVariables[kSensirionSHT3xARPOutputVariableIndexCalibratedTemperatureCelcius] = Tcelcius;
	}

	if (calculateAllOutputs ||
	    (outputSelect == kSensirionSHT3xARPOutputVariableIndexCalibratedTemperatureFahrenheit))
	{
		Tfahrenheit     = kSensirionSHT3xARPSensorCalibrationConstant5 + kSensirionSHT3xARPSensorCalibrationConstant6 * (Vt / Vsupply);
		calibratedValue = outputVariables[kSensirionSHT3xARPOutputVariableIndexCalibratedTemperatureFahrenheit] = Tfahrenheit;
	}

	return calibratedValue;
}
