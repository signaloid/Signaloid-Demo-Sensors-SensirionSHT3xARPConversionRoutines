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

#include <stdio.h>
#include <stdlib.h>
#include <uxhw.h>
#include "utilities.h"

void
printUsage(void)
{
	fprintf(stderr, "Example: SHT3xARP sensor conversion routines - Signaloid version\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: Valid command-line arguments are:\n");
	fprintf(
		stderr,
		"\t[-o, --output <Path to output CSV file : str>] (Specify the output file.)\n"
		"\t[-S, --select-output <output : int>] (Compute 0-indexed output. Calculate all possible outputs if equal to %d. Default value: %d.)\n"
		"\t[-M, --multiple-executions <Number of executions : int (Default: 1)>] (Repeated execute kernel for benchmarking.)\n"
		"\t[-T, --time] (Timing mode: Times and prints the timing of the kernel execution.)\n"
		"\t[-b, --benchmarking] (Benchmarking mode: Generate outputs in format for benchmarking.)\n"
		"\t[-j, --json] (Print output in JSON format.)\n"
		"\t[-h, --help] (Display this help message.)\n",
		kOutputDistributionIndexMax,
		kOutputDistributionIndexMax);
	fprintf(stderr, "\n");

	return;
}

static void
setDefaultCommandLineArguments(CommandLineArguments *  arguments)
{
	/*
	 *	Older GCC versions have a bug which gives a spurious warning for
	 *	the C universal zero initializer `{0}`. Any workaround makes the
	 *	code less portable or prevents the common code from adding new
	 *	fields to the `CommonCommandLineArguments` struct. Therefore, we
	 *	surpress this warning.
	 *
	 *	See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119.
	 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

	*arguments = (CommandLineArguments)
	{
		.common			= (CommonCommandLineArguments) {0}
	};
#pragma GCC diagnostic pop

	return;
}

CommonConstantReturnType
getCommandLineArguments(
	int			argc,
	char *			argv[],
	CommandLineArguments *	arguments)
{
	DemoOption		demoSpecificOptions = {0};

	if (arguments == NULL)
	{
		fprintf(stderr, "Arguments pointer is NULL.\n");

		return kCommonConstantReturnTypeError;
	}

	setDefaultCommandLineArguments(arguments);

	/*
	 *	This application example has no application specific arguments.
	 */
	if (parseArgs(argc, argv, &arguments->common, &demoSpecificOptions) != 0)
	{
		fprintf(stderr, "Parsing command line arguments failed\n");
		printUsage();
		
		return kCommonConstantReturnTypeError;
	}

	/*
	 *	Process command-line arguments
	 */
	if (arguments->common.isHelpEnabled)
	{
		printUsage();

		exit(EXIT_SUCCESS);
	}

	if (arguments->common.isInputFromFileEnabled)
	{
		fprintf(stderr, "Reading inputs from CSV file is not currently supported\n");

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	Write to output file is not supported in MonteCarlo Mode.
	 */
	if (arguments->common.isWriteToFileEnabled && arguments->common.isMonteCarloMode)
	{
		fprintf(stderr, "Writing to output file is not supported in MonteCarlo Mode.\n");

		return kCommonConstantReturnTypeError;
	}

	if (arguments->common.isVerbose)
	{
		fprintf(stderr, "Warning: Verbose mode not supported. Continuing in non-verbose mode.\n");
	}

	/*
	 *	If no output selected from CLA, set the print all value as default.
	 */
	if (!arguments->common.isOutputSelected)
	{
		arguments->common.outputSelect = kOutputDistributionIndexMax;
	}

	/*
	 *	If a single output is selected, we must be in benchmarking mode or Monte Carlo mode.
	 */
	if (arguments->common.outputSelect > kOutputDistributionIndexMax)
	{
		fprintf(
			stderr,
			"Output select value (-S option) is greater than the possible number of outputs: Provided %zd. Max: %d\n",
			arguments->common.outputSelect,
			kOutputDistributionIndexMax);
	}
	/*
	 *	When all outputs are selected, we cannot be in benchmarking mode or Monte Carlo mode.
	 */
	else if (arguments->common.outputSelect == kOutputDistributionIndexMax)
	{
		if ((arguments->common.isBenchmarkingMode) || (arguments->common.isMonteCarloMode))
		{
			fprintf(stderr, "Error: Please select a single output when in benchmarking mode or Monte Carlo mode.\n");

			return kCommonConstantReturnTypeError;
		}
	}

	return kCommonConstantReturnTypeSuccess;
}

void
printCalibratedValueAndProbabilities(double calibratedSensorOutput, const char *  variableDescription, const char *  unitsOfMeasurement)
{
	/*
	 *	Note: the calculations of the quantities involving UxHwDoubleProbabilityGT()
	 *	are purposefully written so as to be self-explanatory and easily checkable,
	 *	not for efficiency or "cleverness". Also, beware the "percent greater than"
	 *	and "percent less than" are tricky for larger versus smaller so don't jump
	 *	to conclusions when you read the code.
	 */
	printf("%s: %.2lf %s.\n", variableDescription, calibratedSensorOutput, unitsOfMeasurement);
	printf("\n");
	printf(
		"\tProbability that calibrated sensor output is   5%% or more smaller than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		1 - UxHwDoubleProbabilityGT(calibratedSensorOutput, calibratedSensorOutput * (1 - 0.05)));
	printf(
		"\tProbability that calibrated sensor output is  50%% or more smaller than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		1 - UxHwDoubleProbabilityGT(calibratedSensorOutput, calibratedSensorOutput * (1 - 0.50)));
	printf(
		"\tProbability that calibrated sensor output is 100%% or more smaller than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		1 - UxHwDoubleProbabilityGT(calibratedSensorOutput, calibratedSensorOutput * (1 - 1.00)));
	printf(
		"\tProbability that calibrated sensor output is 200%% or more smaller than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		1 - UxHwDoubleProbabilityGT(calibratedSensorOutput, calibratedSensorOutput * (1 - 2.00)));
	printf("\n");
	printf(
		"\tProbability that calibrated sensor output is   5%% or more greater than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		UxHwDoubleProbabilityGT(calibratedSensorOutput, 1.05 * calibratedSensorOutput));
	printf(
		"\tProbability that calibrated sensor output is  50%% or more greater than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		UxHwDoubleProbabilityGT(calibratedSensorOutput, 1.50 * calibratedSensorOutput));
	printf(
		"\tProbability that calibrated sensor output is 100%% or more greater than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		UxHwDoubleProbabilityGT(calibratedSensorOutput, 2.00 * calibratedSensorOutput));
	printf(
		"\tProbability that calibrated sensor output is 200%% or more greater than %.2"SignaloidParticleModifier"lf, is %.6"SignaloidParticleModifier"lf\n",
		calibratedSensorOutput,
		UxHwDoubleProbabilityGT(calibratedSensorOutput, 3.00 * calibratedSensorOutput));
}

void
populateJSONVariableStruct(
	JSONVariable *		jsonVariable,
	double *		outputVariableValues,
	const char *		outputVariableDescription,
	OutputDistributionIndex	outputSelect,
	size_t			numberOfOutputVariableValues)
{
	snprintf(jsonVariable->variableSymbol, kCommonConstantMaxCharsPerJSONVariableSymbol, "outputDistributions[%u]", outputSelect);
	snprintf(jsonVariable->variableDescription, kCommonConstantMaxCharsPerJSONVariableDescription, "%s", outputVariableDescription);
	jsonVariable->values = (JSONVariablePointer){ .asDouble = outputVariableValues };
	jsonVariable->type = kJSONVariableTypeDouble;
	jsonVariable->size = numberOfOutputVariableValues;

	return;
}

void
printJSONFormattedOutput(
	CommandLineArguments *	arguments,
	double *		monteCarloOutputSamples,
	double *		outputDistributions,
	const char **		outputVariableDescriptions)
{
	JSONVariable			jsonVariables[kOutputDistributionIndexMax];
	OutputDistributionIndex		outputSelectLowerBound;
	OutputDistributionIndex		outputSelectUpperBound;

	if (arguments->common.outputSelect == kOutputDistributionIndexMax)
	{
		outputSelectLowerBound = (OutputDistributionIndex)0;
		outputSelectUpperBound = kOutputDistributionIndexMax;
	}
	else
	{
		outputSelectLowerBound = arguments->common.outputSelect;
		outputSelectUpperBound = outputSelectLowerBound + 1;
	}

	for (OutputDistributionIndex outputSelect = outputSelectLowerBound; outputSelect < outputSelectUpperBound; outputSelect++)
	{
		/*
		 *	If in Monte Carlo mode, `pointerToOutputVariable` points to the beginning of the `monteCarloOutputSamples` array.
		 *	In this case, `arguments.common.numberOfMonteCarloIterations` is the length of the `monteCarloOutputSamples` array.
		 *	Else, it points to the entry of the `outputVariables` to be used.
		 *	In this case, `arguments.common.numberOfMonteCarloIterations` equals 1.
		 */
		double *	pointerToOutputVariable = arguments->common.isMonteCarloMode ? monteCarloOutputSamples : &outputDistributions[outputSelect];

		populateJSONVariableStruct(
			&jsonVariables[outputSelect],
			pointerToOutputVariable,
			outputVariableDescriptions[outputSelect],
			outputSelect,
			arguments->common.numberOfMonteCarloIterations);
	}

	printJSONVariables(
		&jsonVariables[outputSelectLowerBound],
		outputSelectUpperBound - outputSelectLowerBound,
		"SHT3xARP Sensor Calibration Use Case");

	return;
}
