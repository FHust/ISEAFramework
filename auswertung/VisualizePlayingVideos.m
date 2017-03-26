%This m-script evaluates the Patch_*.csv files and displays them as a
%video.
function[] = VisualizePlayingVideos()

%%Read in
disp('Reading in csv-files ...');
areas = csvread('Patch_Areas.csv');
vertices = csvread('Patch_Vertices.csv');
tempVec = csvread('Patch_Temperatures.csv');


%%Fill 0 components of area with NaNs, as needed by patch command
[n m] = size(areas);
for i = 1:n
    for j=1:m
        if(areas(i, j) == 0)
            areas(i, j) = NaN;
        end
    end
end


%%Setting time configuration
tempVecSize = size(tempVec);
if tempVecSize(1) > 1
    deltaTime = input('How long do you want the video to play in sec?: ');
end
clkVal = clock;
startTime = clkVal(6) + 60 * clkVal(5) + 3600 * clkVal(4);
timeFactor = deltaTime / tempVec(end, 1); 


%%Setting figure configuaration
figure('units','normalized','outerposition',[0 0 1 1]);
set(gcf,'Renderer','zbuffer');
view([27 55]);
xlabel('x [m]','fontsize',14);
ylabel('y [m]','fontsize',14);
zlabel('z [m]','fontsize',14);
cb = colorbar('vert');
ylabel(cb,'Temperatur [\circ C]','fontsize',14);
caxis([min(min(tempVec(:, 2:end)))-2,max(max(tempVec(:, 2:end)))+2]);



%%Loop to display data by patch
for i = 1:tempVecSize(1)
    clkVal = clock;
    
    if(i < tempVecSize(1))
        if(clkVal(6) + 60 * clkVal(5) + 3600 * clkVal(4) > tempVec(i + 1, 1) * timeFactor + startTime)
            continue;
        end
    end
    
    while(clkVal(6) + 60 * clkVal(5) + 3600 * clkVal(4) < tempVec(i, 1) * timeFactor + startTime)
        clkVal = clock;
    end
    
    
    
    temperatures = transpose(tempVec(i, 2:end));
    
    pause(0.01);
    hold on;
    title(num2str(tempVec(i, 1)));
    set(gca,'DataAspectRatio',[1 1 1]);
    patch('Vertices',vertices,'Faces',areas,'FaceColor','flat','FaceVertexCData',temperatures,'Marker','.','MarkerFaceColor','k');
end

end
