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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <uxhw.h>
#include "utilities.h"

/**
 *	@brief  Sets the Input Distributions via call to UxHw Parametric function.
 *
 *	@param  inputDistributions	: An array of double values, where the function writes
 *					the distributional data.
 */
static void
setInputDistributionsViaUxHwCall(double *  inputDistributions)
{
	inputDistributions[kInputDistributionIndexVrh] = UxHwDoubleUniformDist(
								kDefaultInputDistributionVrhUniformDistLow,
								kDefaultInputDistributionVrhUniformDistHigh);

	inputDistributions[kInputDistributionIndexVt] = UxHwDoubleUniformDist(
								kDefaultInputDistributionVtUniformDistLow,
								kDefaultInputDistributionVtUniformDistHigh);

	inputDistributions[kInputDistributionIndexVsupply] = UxHwDoubleUniformDist(
								kDefaultInputDistributionVsupplyUniformDistLow,
								kDefaultInputDistributionVsupplyUniformDistHigh);

	return;
}

/**
 *	@brief  Sensor calibration routines taken from Figure 9 and Figure 10 in
 *		page 9 of Datasheet_SHT3x_ARP.pdf, 2024-07-03.
 *
 *	@param  arguments		: Pointer to command line arguments struct.
 *	@param  inputDistributions	: The array of input distributions used in the calculation.
 * 	@param  outputDistributions	: An array of of output distributions. Writes the result to `outputDistributions[outputSelectValue]`.
 *
 *	@return	double			: Returns the distributional value calculated.
 */
static double
calculateSensorOutput(CommandLineArguments *  arguments, double *  inputDistributions, double *  outputDistributions)
{
	double	Rh;
	double	Tcelcius;
	double	Tfahrenheit;
	double	Vsupply;
	double	Vt;
	double	Vrh;
	double	calibratedValue = 0.0;

	bool	calculateAllOutputs = (arguments->common.outputSelect == kOutputDistributionIndexMax);

	Vsupply = inputDistributions[kInputDistributionIndexVsupply];
	Vt = inputDistributions[kInputDistributionIndexVt];
	Vrh = inputDistributions[kInputDistributionIndexVrh];

	if (calculateAllOutputs ||
		(arguments->common.outputSelect == kOutputDistributionIndexCalibratedRelativeHumidity))
	{
		Rh = kSensorCalibrationConstant1 + kSensorCalibrationConstant2 * (Vrh / Vsupply);
		calibratedValue = outputDistributions[kOutputDistributionIndexCalibratedRelativeHumidity] = Rh;
	}

	if (calculateAllOutputs ||
		(arguments->common.outputSelect == kOutputDistributionIndexCalibratedTemperatureCelcius))
	{
		Tcelcius = kSensorCalibrationConstant3 + kSensorCalibrationConstant4 * (Vt / Vsupply);
		calibratedValue = outputDistributions[kOutputDistributionIndexCalibratedTemperatureCelcius] = Tcelcius;
	}

	if (calculateAllOutputs ||
		(arguments->common.outputSelect == kOutputDistributionIndexCalibratedTemperatureFahrenheit))
	{
		Tfahrenheit =  kSensorCalibrationConstant5 + kSensorCalibrationConstant6 * (Vt/Vsupply);
		calibratedValue = outputDistributions[kOutputDistributionIndexCalibratedTemperatureFahrenheit] = Tfahrenheit;
	}

	return	calibratedValue;
}

int
main(int argc, char *  argv[])
{
	CommandLineArguments	arguments = {0};

	double			calibratedSensorOutput;
	double *		monteCarloOutputSamples = NULL;
	clock_t			start;
	clock_t			end;
	double			cpuTimeUsedSeconds;
	double			inputDistributions[kInputDistributionIndexMax];
	double			outputDistributions[kOutputDistributionIndexMax];
	const char *		outputVariableNames[kOutputDistributionIndexMax] =
				{
					"Calibrated Relative Humidity",
					"Calibrated Temperature (in Celsius)",
					"Calibrated Temperature (in Farenheit)",
				};
	const char *		unitsOfMeasurement[] =
				{
					[kOutputDistributionIndexCalibratedRelativeHumidity]		= "%",
					[kOutputDistributionIndexCalibratedTemperatureCelcius]		= "Celcius",
					[kOutputDistributionIndexCalibratedTemperatureFahrenheit]	= "Fahrenheit",
				};
	MeanAndVariance		meanAndVariance;

	/*
	 *	Get command line arguments.
	 */
	if (getCommandLineArguments(argc, argv, &arguments))
	{
		return kCommonConstantReturnTypeError;
	}

	if (arguments.common.isMonteCarloMode)
	{
		monteCarloOutputSamples = (double *) checkedMalloc(
							arguments.common.numberOfMonteCarloIterations * sizeof(double),
							__FILE__,
							__LINE__);
	}

	/*
	 *	Start timing.
	 */
	if (arguments.common.isTimingEnabled || arguments.common.isBenchmarkingMode)
	{
		start = clock();
	}

	for (size_t i = 0; i < arguments.common.numberOfMonteCarloIterations; i++)
	{
		/*
		 *	Set input distribution values, inside the main computation
		 *	loop, so that it can also generate samples in the native
		 *	Monte Carlo Execution Mode.
		 */
		setInputDistributionsViaUxHwCall(inputDistributions);

		calibratedSensorOutput = calculateSensorOutput(&arguments, inputDistributions, outputDistributions);

		/*
		 *	For this application, calibratedSensorOutput is the item we track.
		 */
		if (arguments.common.isMonteCarloMode)
		{
			monteCarloOutputSamples[i] = calibratedSensorOutput;
		}
	}

	/*
	 *	If not doing Laplace version, then approximate the cost of the third phase of
	 *	Monte Carlo (post-processing), by calculating the mean and variance.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		meanAndVariance = calculateMeanAndVarianceOfDoubleSamples(monteCarloOutputSamples, arguments.common.numberOfMonteCarloIterations);
		calibratedSensorOutput = meanAndVariance.mean;
	}

	/*
	 *	Stop timing.
	 */
	if (arguments.common.isTimingEnabled || arguments.common.isBenchmarkingMode)
	{
		end = clock();
		cpuTimeUsedSeconds = ((double)(end - start)) / CLOCKS_PER_SEC;
	}

	if (arguments.common.isBenchmarkingMode)
	{
		/*
		 *	In benchmarking mode, we print:
		 *		(1) single result (for calculating Wasserstein distance to reference)
		 *		(2) time in microseconds (benchmarking setup expects cpu time in microseconds)
		 */
		printf("%lf %" PRIu64 "\n", calibratedSensorOutput, (uint64_t)(cpuTimeUsedSeconds*1000000));
	}
	else
	{
		/*
		 *	Print the results (either in JSON or standard output format).
		 */
		if (!arguments.common.isOutputJSONMode)
		{
			if (arguments.common.outputSelect == kOutputDistributionIndexMax)
			{
				for (size_t i = 0; i < kOutputDistributionIndexMax; i++)
				{
					printCalibratedValueAndProbabilities(
						outputDistributions[i],
						outputVariableNames[i],
						unitsOfMeasurement[i]);
				}
			}
			else
			{
				printCalibratedValueAndProbabilities(
					calibratedSensorOutput,
					outputVariableNames[arguments.common.outputSelect],
					unitsOfMeasurement[arguments.common.outputSelect]);
			}
		}
		else
		{
			printJSONFormattedOutput(
				&arguments,
				monteCarloOutputSamples,
				outputDistributions,
				outputVariableNames);
		}

		/*
		 *	Print timing result.
		 */
		if (arguments.common.isTimingEnabled)
		{
			printf("\nCPU time used: %lf seconds\n", cpuTimeUsedSeconds);
		}

		/*
		 *	Write output data.
		 */
		if (arguments.common.isWriteToFileEnabled)
		{
			if (writeOutputDoubleDistributionsToCSV(
				arguments.common.outputFilePath,
				outputDistributions,
				outputVariableNames,
				kOutputDistributionIndexMax))
			{
				return kCommonConstantReturnTypeError;
			}
		}
	}

	/*
	 *	Save Monte carlo outputs in an output file
	 *	Free dynamically-allocated memory.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		saveMonteCarloDoubleDataToDataDotOutFile(monteCarloOutputSamples, (uint64_t)(cpuTimeUsedSeconds*1000000), arguments.common.numberOfMonteCarloIterations);
		
		free(monteCarloOutputSamples);
	}

	return 0;
}
