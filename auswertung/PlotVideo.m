%PlotVideo

%This m-script evaluates the Patch_*.csv files and displays them as a
%video.

%%Read in
if(exist('Patch_Areas.csv','file') && exist('Patch_Vertices.csv','file') && exist('Patch_Temperatures.csv','file'))    
    disp('new Simulation results were found')
    movefile('Patch_*','SimulationsErgebnisse\.')
    disp('Reading in csv-files ...');
    areas = csvread('SimulationsErgebnisse\Patch_Areas.csv');
    vertices = csvread('SimulationsErgebnisse\Patch_Vertices.csv');
    tempVec = csvread('SimulationsErgebnisse\Patch_Temperatures.csv');
elseif(exist('SimulationsErgebnisse\Patch_Areas.csv','file') && exist('SimulationsErgebnisse\Patch_Vertices.csv','file') && exist('SimulationsErgebnisse\Patch_Temperatures.csv','file'))    
    disp('no new simulation results were found, using existing ones');
    disp('Reading in csv-files ...');
    areas = csvread('SimulationsErgebnisse\Patch_Areas.csv');
    vertices = csvread('SimulationsErgebnisse\Patch_Vertices.csv');
    tempVec = csvread('SimulationsErgebnisse\Patch_Temperatures.csv');
else
    error('no simulation results were found')
end




areas(areas==0) = NaN;

timeVector = tempVec(:,1);
simDuration = timeVector(end) - timeVector(1);
speedfactor = input(['Total Simulation time was ',num2str(simDuration),'s.\n Choose a speedfactor > 1 to speed up graphics (simulation_time/speedfactor) (default 10)?: ']);

if(isempty(speedfactor))
    speedfactor = 10;
end

dtVector = diff(timeVector)/speedfactor;

%%Setting figure configuration
figure('units','normalized','outerposition',[0 0 1 1]);
set(gcf,'Renderer','zbuffer');
view([27 55]);
xlabel('x [m]','fontsize',14);
ylabel('y [m]','fontsize',14);
zlabel('z [m]','fontsize',14);
cb = colorbar('vert');
ylabel(cb,'Temperatur [\circ C]','fontsize',14);
caxis([min(min(tempVec(:, 2:end)))-2,max(max(tempVec(:, 2:end)))+2]);

    time = num2str(timeVector(1));
    th = title(time);
    set(gca,'DataAspectRatio',[1 1 1]);
    temperatures = transpose(tempVec(1, 2:end));
    h = patch('Vertices',vertices,'Faces',areas,'FaceColor','flat','FaceVertexCData',temperatures,'Marker','.','MarkerFaceColor','k');
    
for i = 2:1:length(timeVector)
   pause(dtVector(i-1)-0.05);
   %tic
   temperatures = transpose(tempVec(i, 2:end)); 
   set(h,'FaceVertexCData',temperatures);
   title(num2str(timeVector(i)));
   %toc
end
