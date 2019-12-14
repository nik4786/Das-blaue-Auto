%--------------------------------------------------------------------------
%       Tool for Controller Configuration regarding to Ziegler-Nichols 
%                           (Step Response Method)
%--------------------------------------------------------------------------

function [ControllerParameter] = AdvancedZieglerNichols(n)
%% import csv-file

% uigetfile opens file dialog, in which user has to choose the CSV file
% path- and filename are saved as string
[file,path] = uigetfile('*.csv'); 

% both strings are combined to one single string with fullfile-command
% --> CSV can be imported, even if filing path is not corresponding with
%     the current folder in MATLAB
csvfile = importdata(fullfile(path,file));


%% User Input to specify Controller Type (P, PI, PID) and Signalname

% input opens UI in Comman Window
% User has to choose Controller Type and name of signal of CSV File
% Also, User has the opportunity to plot the result by typing 'Y' or 'N'
ControllerType = input('Please choose Controller Type (P, PI, PID)!:\n', 's');
SignalName = input('Please enter the name of the signal in the csv-file!:\n', 's');
UserReq = input('Do you want to plot the results? Y/N [Y]:','s');
%% Data-Preprocessing

% get time and save it in an array
timestamp = csvfile.data(:,1);
% get column regarding to choosen signal name
headeridx = find(strcmp(csvfile.colheaders, SignalName));
% get right data and save it in an array
data = -csvfile.data(:,headeridx);

% shift timestamp, so that cut data starts with 0 again
musTime = timestamp(:)-timestamp(1);
% convert microseconds into seconds
sTime = musTime*1e-06;

%% Polynomial curve fitting with polyfit and polyval to get mathmatical expression of step response

% nth-polynomial function to approximate step-response behavior
fit = polyfit(sTime,data,n);
% array with fitted values
polyfunction = polyval(fit,sTime);

%% cut data at maximum point

% get index of maximum value of whole input data
[~,maxidx] = max(polyfunction);
% cut time and data array with extracted index as last element
polyfunction = polyfunction(1:maxidx);
sTime = sTime(1:maxidx);

% set all negative data values to zero and increase number and save index
% of negative values in array 'zeroidx' (probably these values are at the
% beginning of data. It is not possible, that zero values are at the end of
% the data, because it is cut at its maximum point
zeroidx = 0;
for i = 1:length(polyfunction)
    if polyfunction(i) < 0
        polyfunction(i) = 0;
        zeroidx=i;
    end
end
% Data and time is cut again and is now starting at the last point, which
% was set to zero in the last step --> eliminate an unnecessary turning
% point at the beginning of the data
if zeroidx > 0
    polyfunction = polyfunction(zeroidx:end);
    sTime = sTime(zeroidx:end);
    sTime = sTime(:) - sTime(1);  
end

%% Differentiation to get turning point of step response

% get first differntiation of data with respect to the time by 
% using the gradient-function
y_p = gradient(polyfunction, sTime);

% because of the fact, that the step response method only works, when the
% behavior of the measured data is representing a PT2-behavior 
% (concave), the inflection point can be determined by 
% calculating the point of time, at which the first differentiation is at 
% its maximum. So it is provided, that that only concave inflection points
% are recognized by the code.
% To be more precise, the necessary point of time and the data are
% interpolated with interp1-function
tInflPoi = interp1(y_p,sTime,max(y_p));
yInflPoi = interp1(sTime, polyfunction, tInflPoi);

%% determine function for inflection tangent tngnt = m*sTime + intrcpt

% some mathematical operations to get the values for defining the function
% of the inflection tangent
m  = interp1(sTime, y_p, tInflPoi);
intrcpt = yInflPoi - m*tInflPoi;
tngnt = m*sTime + intrcpt; 

%% Getting values for Ks, Ta, Tu

% maximum value of step response correspond with Ks
Ks = max(polyfunction);

% get Index of nearest value of tngnt to Ks and interpolating
% afterwards to get Ta
TaPre = interp1(tngnt,sTime,Ks);
Tu = interp1(tngnt,sTime,0);
Ta = TaPre - Tu;

%% Applying Ks, Ta and Tu to Ziegler-Nichols tuning rules

% dpeending of the userinput at the beginnging of the code, the right
% paramters are calculated in this section by applying the calculated
% values of the last section with the tuning rules by Ziegler-Nichols. If
% there is error by typing something else than 'P', 'PI' or 'PID', the user
% will be indicated explicitly about this mistake.
switch ControllerType
    case 'P'
       ControllerParameter = [(Ta/(Ks*Tu))];
    case 'PI'
       ControllerParameter = [(0.9*Ta/(Ks*Tu)), 3.33*Tu];
    case 'PID'
       ControllerParameter = [(0.9*Ta/(Ks*Tu)), (2*Tu), (0.5*Tu)];
    otherwise
       display('Please chose P, PI or PID correctly!')
end

%% Plot script to visualize the result of the step response method 

% If the user required to visualize the result, following plot script will be
% exeuted
if UserReq == 'Y'
    % create figure
    Fig = figure;hold on;box on
    set(Fig,'Color',[0.95 0.95 0.95]) 
    % create axis
    Axis_stepresponse = subplot(1,1,1);
    %plot all necessary lines
    Line_stepresponse = plot(sTime,polyfunction);
    Line_tangent = plot(sTime,tngnt);
    Mark_InflectionPoint = plot(tInflPoi,yInflPoi);
    % plot marks to point out both intersects
    Mark_Ta = plot(TaPre,Ks);
    Mark_Tu = plot(Tu,0);
    % some figure settings
    set(Axis_stepresponse, 'Title', text('String', 'Step Response Method by Ziegler-Nichols', 'Color', 'k', 'FontWeight', 'bold'))
    set(Axis_stepresponse, 'xlim', [0,TaPre+0.1])
    set(Axis_stepresponse, 'ylim', [0,Ks+0.75])
    set(get(Axis_stepresponse, 'XLabel'), 'String', 'time [s]')
    set(get(Axis_stepresponse, 'YLabel'), 'String', 'velocity [m/s]')
    set(Axis_stepresponse, 'XGrid', 'on')
    set(Axis_stepresponse, 'XTick', 0:0.2:sTime(end)) 
    set(Axis_stepresponse, 'YGrid', 'on')
    set(Line_stepresponse, 'LineWidth', 1', 'LineStyle', '-', 'Color', 'b')
    set(Line_tangent, 'LineWidth', 0.5', 'LineStyle', '-', 'Color', 'r')
    set(Mark_InflectionPoint, 'LineWidth', 1','Marker', 'o', 'Color', 'r')
    set(Mark_Ta, 'LineWidth', 1', 'Marker', 'o', 'Color', 'r')
    set(Mark_Tu, 'LineWidth', 1', 'Marker', 'o', 'Color', 'r')
    yline(Ks,'--', 'LineWidth', 1')
    xline(Tu, '--', 'LineWidth', 1')
    xline(TaPre, '--', 'LineWidth', 1')
    text(0.5*TaPre, Ks+0.1, 'v = Ks')
    legend(Axis_stepresponse, [Line_stepresponse Line_tangent], 'Step-Response', 'Inflection tangent', 'Location', 'NorthEast') 
end
end

