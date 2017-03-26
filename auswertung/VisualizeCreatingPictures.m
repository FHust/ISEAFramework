%This function creates picture files (.fig, .eps and .png) from the 
%Patch_*.csv files. The input parameter arrayWithTimes contains all points
%of time, at which the pictures are to be made.
function[] = VisualizeCreatingPictures(arrayWithTimes)

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
i = 1;
for dTime = arrayWithTimes
    while tempVec(i, 1) < dTime;
        i = i + 1;
    end
    
    
    temperatures = transpose(tempVec(i, 2:end));
    
    pause(0.01);
    hold on;
    title(num2str(tempVec(i, 1)));
    set(gca,'DataAspectRatio',[1 1 1]);
    patch('Vertices',vertices,'Faces',areas,'FaceColor','flat','FaceVertexCData',temperatures,'Marker','.','MarkerFaceColor','k');
    
    nameString = strcat('Visualisierung_', num2str(dTime));
    print(gcf, '-dpng', strcat(nameString,'.png'));
    print(gcf, '-depsc', strcat(nameString,'.eps'));
    hgsave(gcf, nameString);
end

end
