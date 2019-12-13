function cpp2mat(sourcepath, sourcefile, headerpath, headerfile)
%% Setting Legacy Code Tool

% initialize data structure to register characteristics of C++-Code and
% properties of S-function 
def = legacy_code('initialize');

% defining path, in which the header and source file each is located
def.IncPaths = {headerpath};
def.SrcPaths = {sourcepath};

% specify C++-Source and -Header filenames 
def.SourceFiles = {sourcefile};
def.HeaderFiles = {headerfile};

% specify name and output function information of S-function
def.SFunctionName = 'PIDcpp';
def.OutputFcnSpec = 'double y1 = PIDController(double u1, double u2)';

% specify the language of source file
def.Options.language = 'C++';

% generate S-function source file 
legacy_code('sfcn_cmex_generate', def);

% compile and link S-function source into dynamically loadable executable
legacy_code('compile', def);

%% Assign Simulink-Model to 'sys'
sys = 'DasBlaueAuto_Controller_Model';
open_system(sys);

%% add S-Function-Block to sys-Model automatically
% description: add_block('Extracting S-Function in Lib', 'Modelname/Blockname', 'Parameter'**, 'Parametervalue') 
% **List of Parameternames: https://de.mathworks.com/help/simulink/slref/block-specific-parameters.html#mw_62a140a2-e0e3-4989-90f9-4c431c38fd1c
try
    add_block('simulink/User-Defined Functions/S-Function','DasBlaueAuto_Controller_Model/PID', 'FunctionName', 'PIDcpp');
catch error
    warning('Error! Check if required Simulink-Block is already existing!')
end

