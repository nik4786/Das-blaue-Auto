function configureSimulinkModel(nAnzahlInputs, nAnzahlOutputs)
    %open_system(new_system('test'));
    system_name = 'DasBlaueAuto_Controller_Model';

    open_system(system_name);
    
    sim_data = zeros(1,nAnzahlInputs);
    
    for t = 1:nAnzahlInputs
        add_block('simulink/Sources/From Workspace', [system_name, '/From Workspace', num2str(t)]);
        set_param([system_name, '/From Workspace', num2str(t)], 'VariableName', num2str(sim_data(:,t)));
        %set_param([system_name, '/From Workspace', num2str(t)],'position',[140,80,180,120]);
        %set_param([system_name, '/From Workspace', num2str(t)], 'VariableName', ['simin', num2str(t)]);
        add_line(system_name,['From Workspace', num2str(t), '/1'],['PID/',num2str(t)]);
    end
    
    for n = 1:nAnzahlOutputs
        add_block('simulink/Sinks/To File', [system_name, '/To File', num2str(n)]);
        %set_param('test/To Workspace1','position',[540,80,580,120]);
        set_param([system_name, '/To File', num2str(n)], 'FileName', ['simout', num2str(n)]);
        add_line(system_name,['PID/',num2str(n)],['To File', num2str(n), '/1']);
    end
    
    Simulink.BlockDiagram.arrangeSystem(system_name);
    save_system(system_name);