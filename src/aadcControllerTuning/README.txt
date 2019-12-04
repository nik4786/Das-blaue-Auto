---------------------------------------------------
Controller-Tuning-Tool MATLAB: Step Response Method
---------------------------------------------------

1 FOREWORD

With this tool, it should be possible to tune a arbitrary controller (P, PI, PID) in a relatively fast and easy fashion in MATLAB. The used method of controller-tuning is the Step Response Method by Ziegler-Nichols.


2 PREREQUISITES

- MATLAB 2019a (not tested with previous versions)
- Step response depending on controller as csv-file (Convertion of adtfdat-file in csv-file with adtf-dattool)


3 PREPARATION

At first, it is necessary to record a step response in adtf. Probably, a adtfdat-file is generated. To use MATLAB, a conversion of the adftdat-file into a csv-file is needed. The easiest way for doing this is to use the adtf-dattool with the corresponding csv-processor to convert into csv.
After converting, the step response have to be cut to a range, in which only the step response is mapped. 
Finally, all files (m-function, csv-files) have to be put into the same folder.


4 APPLICATION

By using 'AdvancedZieglerNichols(u1)' in the MATLAB-Command-Window the process is started:

	- 	'u1' constitutes the polynomial degree of the function, which is used to fit the noisy step 				respone. The values, used for this, can be very various. If the used value is too low, it is 
		possible, that the tuning won't work, because of not presenting a inevitable inflection point.
		If the value is too high, there's a chance of having a very falsified depiction of the step
		step response, which leads to wrong results. 

		Tipp: 	Use the plot-function (description to follow) to get a feeling for the behavior of the step 			response

	-	At first a user-interface will pop up, in which the cut csv-file has to be choosen.
		Afterwards, the Controller-Type (P,PI,PID) has to be specified in the CW.
		Then, the signal-name of the relevant value in the csv-file has to be typed into the CW.
		At least, it is possible to activate the opportunity to plot the result with 'Y' od 'N'.

	-	Finally, the result will be plotted, if it was choosen, and a array with the Parameter-Results
		will be output
		