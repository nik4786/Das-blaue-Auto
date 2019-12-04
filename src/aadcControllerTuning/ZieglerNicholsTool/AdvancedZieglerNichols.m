%--------------------------------------------------------------------------
%       Tool for Controller Configuration regarding to Ziegler-Nichols 
%                           (Step Response Method)
%--------------------------------------------------------------------------

function [ControllerParameter] = AdvancedZieglerNichols(n)

%clear all; close all; clc;

%% import csv-file

[file,~] = uigetfile('*.csv'); 
csvfile = importdata(file);

%% User Input to specify Controller Type (P, PI, PID) and Signalname

ControllerType = input('Please choose Controller Type (P, PI, PID)!:\n', 's');
SignalName = input('Please enter the name of the signal in the csv-file!:\n', 's');

%% Data-Preprocessing

% get time and corresponding value and save it in 'time'- and 'data'-vector
timestamp = csvfile.data(:,1);
headeridx = find(strcmp(csvfile.colheaders, SignalName));
data = -csvfile.data(:,headeridx);

% Move timestamp (--> timestamp starts with zero)
musTime = timestamp(:)-timestamp(1);
sTime = musTime/1e06;

%% Polynomial curve fitting with polyfit and polyval to get mathmatical expression of step response

% 9th-polynomial function to approximate step-response behavior
fit = polyfit(sTime,data,n);
% array with fitted values
polyfunction = polyval(fit,sTime);

%% cut data at maximum point

[~,maxidx] = max(polyfunction);
polyfunction = polyfunction(1:maxidx);
sTime = sTime(1:maxidx);

zeroidx = 0;
for i = 1:length(polyfunction)
    if polyfunction(i) < 0
        polyfunction(i) = 0;
        zeroidx=i;
    end
end

if zeroidx > 0
    polyfunction = polyfunction(zeroidx:end);
    sTime = sTime(zeroidx:end);
    sTime = sTime(:) - sTime(1);  
end

%% Differentiation to get turning point of step response

y_p = gradient(polyfunction, sTime);
y_pp = gradient(y_p, sTime);

tInflPoi = interp1(y_p,sTime,max(y_p));
yInflPoi = interp1(sTime, polyfunction, tInflPoi);

%% determine function for inflection tangent tngnt = m*sTime + intrcpt
%m = interp1(y_p, sTime, tInflPoi);
m  = interp1(sTime, y_p, tInflPoi);
intrcpt = yInflPoi - m*tInflPoi;
tngnt = m*sTime + intrcpt; 

%% Getting values for Ks, Ta, Tu

% maximum value of step response correspond with Ks
Ks = max(polyfunction);

% getting Index of nearest value of tngnt to Ks and interpolating
% afterwards to get Ta

TaPre = interp1(tngnt,sTime,Ks);
Tu = interp1(tngnt,sTime,0);
Ta = TaPre - Tu;

%% Applying Ks, Ta and Tu to Ziegler-Nichols tuning rules

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

UserReq = input('Do you want to plot the results? Y/N [Y]:','s');
if UserReq == 'Y'
    % creates figure
    Fig = figure;hold on;box on
    set(Fig,'Color',[0.95 0.95 0.95]) 
    
    Axis_stepresponse = subplot(1,1,1);
    Line_stepresponse = plot(sTime,polyfunction);
    Line_tangent = plot(sTime,tngnt);
    Mark_InflectionPoint = plot(tInflPoi,yInflPoi);
    Mark_Ta = plot(TaPre,Ks);
    Mark_Tu = plot(Tu,0);
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
