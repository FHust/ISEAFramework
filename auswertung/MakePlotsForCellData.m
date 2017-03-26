%This function creates graphs for current, voltage, state of charge, heat 
%generation and temperature from Data_i_timeEl_cur_volt_soc_heat_temp.csv.
%The input parameter cellVector contains each cell to be in the graph as
%indices, the index for the first cell being 1.
function[] = MakePlotsForCellData(cellVector)

set(0, 'DefaultAxesFontSize', 20, 'DefaultAxesXGrid', 'on', 'DefaultAxesYGrid', 'on');

data = csvread('Data_i_timeEl_cur_volt_soc_heat_temp.csv');

numberOfCells = 1;
while data(numberOfCells, 1) < data(numberOfCells + 1, 1) && numberOfCells < size(data, 1)
    numberOfCells = numberOfCells + 1;
end

numberOfTimes = size(data, 1) / numberOfCells;

timeVector = zeros(numberOfTimes, 1);
for i = 1:numberOfTimes
    timeVector(i) = data(numberOfCells * i, 2);
end

numberOfRowsInData = size(data, 1);

sizeOfCellVector = size(cellVector, 1);
if sizeOfCellVector == 1
    sizeOfCellVector = size(cellVector, 2);
    cellVector = transpose(cellVector);
end





%%current
clf('reset');
handleToFigure = figure(1);
hold all;
TitleString = 'Current in Cells no. 1';
for i = 1:sizeOfCellVector
    plot(timeVector, data(cellVector(i):numberOfCells:numberOfRowsInData, 3));
    if (i ~= 1)
        TitleString = strcat(TitleString, ', ', num2str(cellVector(i)));
    end
end
legend(num2str(cellVector, 'Cell no. %d'));
title(TitleString);
xlabel('time[sec]');
ylabel('current[A]');
print(handleToFigure, '-dpng', 'cells_current.png');
print(handleToFigure, '-depsc', 'cells_current.eps');
hgsave(handleToFigure,'cells_current');
hold off;




%%voltage
clf('reset');
handleToFigure = figure(1);
hold all;
TitleString = 'Voltage in Cells no. 1';
for i = 1:sizeOfCellVector
    plot(timeVector, data(cellVector(i):numberOfCells:numberOfRowsInData, 4));
    if (i ~= 1)
        TitleString = strcat(TitleString, ', ', num2str(cellVector(i)));
    end
end
legend(num2str(cellVector, 'Cell no. %d'));
title(TitleString);
xlabel('time[sec]');
ylabel('voltage[V]');
print(handleToFigure, '-dpng', 'cells_voltage.png');
print(handleToFigure, '-depsc', 'cells_voltage.eps');
hgsave(handleToFigure,'cells_voltage');



%%soc
clf('reset');
handleToFigure = figure(1);
hold all;
TitleString = 'SoC in Cells no. 1';
for i = 1:sizeOfCellVector
    plot(timeVector, data(cellVector(i):numberOfCells:numberOfRowsInData, 5));
    if (i ~= 1)
        TitleString = strcat(TitleString, ', ', num2str(cellVector(i)));
    end
end
legend(num2str(cellVector, 'Cell no. %d'));
title(TitleString);
xlabel('time[sec]');
ylabel('soc[%]');
print(handleToFigure, '-dpng', 'cells_soc.png');
print(handleToFigure, '-depsc', 'cells_soc.eps');
hgsave(handleToFigure,'cells_soc');



%%heat
clf('reset');
handleToFigure = figure(1);
hold all;
TitleString = 'Heat generation in Cells no. 1';
for i = 1:sizeOfCellVector
    plot(timeVector, data(cellVector(i):numberOfCells:numberOfRowsInData, 6));
    if (i ~= 1)
        TitleString = strcat(TitleString, ', ', num2str(cellVector(i)));
    end
end
legend(num2str(cellVector, 'Cell no. %d'));
title(TitleString);
xlabel('time[sec]');
ylabel('heat generation[W]');
print(handleToFigure, '-dpng', 'cells_heat.png');
print(handleToFigure, '-depsc', 'cells_heat.eps');
hgsave(handleToFigure,'cells_heat');



%%temperature
clf('reset');
handleToFigure = figure(1);
hold all;
TitleString = 'Average temperature in Cells no. 1';
for i = 1:sizeOfCellVector
    plot(timeVector, data(cellVector(i):numberOfCells:numberOfRowsInData, 7));
    if (i ~= 1)
        TitleString = strcat(TitleString, ', ', num2str(cellVector(i)));
    end
end
legend(num2str(cellVector, 'Cell no. %d'));
title(TitleString);
xlabel('time[sec]');
ylabel('temperature[°C]');
print(handleToFigure, '-dpng', 'cells_temperature.png');
print(handleToFigure, '-depsc', 'cells_temperature.eps');
hgsave(handleToFigure,'cells_temperature');

end
