%This m-script evaluates the Patch_*.csv files, creates a video and stores
%it into a mpeg- and an avi-file.
function[] = VisualizeMakingMovie()

%%Read in
disp('Reading in csv-files ...');
areas = csvread('Patch_Areas.csv');
vertices = csvread('Patch_Vertices.csv');
f = fopen('Patch_Temperatures.csv');
x = fread(f,1);
if x == '#'
    tempVec = csvread('Patch_Temperatures.csv',2,0 );
else
    tempVec = csvread('Patch_Temperatures.csv' );
end
fclose(f);
tempVecSize = size(tempVec);
clear frames;


%%Asking configurations from user
lengthInSec = input('How long do you want the video to play in sec?: ');
framesPerSec = input('How many frames per second?: ');
frameCount = lengthInSec * framesPerSec;
timeFactor = tempVec(end, 1) / frameCount;


%%Setting figure configuaration
handleToFigure = figure('units','normalized','outerposition',[0 0 1 1]);
set(gcf,'Renderer','zbuffer');
view([27 55]);
xlabel('x [m]','fontsize',14);
ylabel('y [m]','fontsize',14);
zlabel('z [m]','fontsize',14);
cb = colorbar('vert');
ylabel(cb,'Temperatur [\circ C]','fontsize',14);
caxis([min(min(tempVec(:, 2:end)))-2,max(max(tempVec(:, 2:end)))+2]);


%%Opening files
disp('Building movie file ...');
writerM4 = VideoWriter('Thermalsimulation', 'MPEG-4');
writerM4.FrameRate = framesPerSec;
%writerM4.CompressionRatio = 100;
writerM4.open;
writerAvi = VideoWriter('Thermalsimulation', 'Motion JPEG AVI');
writerAvi.FrameRate = framesPerSec;
writerAvi.open;


%%Loop to load frames of patch into movie
disp('Creating frames ...');
j = 0;
set(handleToFigure,'Renderer','zBuffer');
for i = 0:frameCount
    if(j < tempVecSize(1))
        if(tempVec(j + 1, 1) <= i * timeFactor)
            while(tempVec(j + 1, 1) <= i * timeFactor && j < tempVecSize(1) - 1)
                j = j + 1;
            end
            
            temperatures = transpose(tempVec(j, 2:end));
            hold on;
            title(num2str(tempVec(j, 1)));
            set(gca,'DataAspectRatio',[1 1 1]);
            patch('Vertices',vertices,'Faces',areas,'FaceColor','flat','FaceVertexCData',temperatures,'Marker','.','MarkerFaceColor','k');
        end
    end
    
    print(handleToFigure, '-r90', '-djpeg', 'frame.jpg');
    [X, map] = imread('frame.jpg');
    frame = im2frame(X, map);
    writerM4.writeVideo(frame);
    writerAvi.writeVideo(frame);
end

end
