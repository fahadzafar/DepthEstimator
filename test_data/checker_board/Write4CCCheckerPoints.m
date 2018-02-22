

clear all;close all;clc;

frame = 5


str = sprintf('png/frame%02d_2_1,0_12_001.png',frame)
im10 = imread(str);
im1 = im10(:,1:400);
im0 = im10(:,401:800);

str = sprintf('png/frame%02d_2_3,2_34_001.png',frame)
im32 = imread(str);
im3 = im32(:,1:400);
im2 = im32(:,401:800);

Allim{1,1} = im0;
Allim{2,1} = im1;
Allim{3,1} = im2;
Allim{4,1} = im3;

for ii = 1:4
    im = Allim{ii,1};
    figure;imshow(im);hold on
    corners = findCorners(im,0.01,1);
    chessboards = chessboardsFromCorners(corners);
    for i=1:length(chessboards)
        % extract chessboard
        cb = chessboards{i};
        p = corners.p(cb(:),:)+1;
        p = p';
        nn = size(p,2);
        
        for jj = 1:nn
            plot(p(1,jj),p(2,jj),'r*');
            text(p(1,jj),p(2,jj),num2str(jj));
            pause(0.05);
        end
        Allp{ii,1} = p;
    end
    
    
end

keyboard

p1 = Allp{1};
p2 = Allp{2};
p3 = Allp{3};
p4 = Allp{4};


if(frame == 1)
    q1 = p1;
    q3 = p3;
    q4 = p4;
    
    
    tt = flipud(reshape(p2(1,:),5,9))';
    tt = tt(:);
    q2(1,:) = tt';
    
    tt = flipud(reshape(p2(2,:),5,9))';
    tt = tt(:);
    q2(2,:) = tt';
    
elseif(frame == 2)
    q1 = p1;
    
    tt = flipud(reshape(p2(1,:),5,9))';
    tt = tt(:);
    q2(1,:) = tt';
    
    tt = flipud(reshape(p2(2,:),5,9))';
    tt = tt(:);
    q2(2,:) = tt';
    
    
    tt = flipud(reshape(p3(1,:),5,9))';
    tt = tt(:);
    q3(1,:) = tt';
    
    tt = flipud(reshape(p3(2,:),5,9))';
    tt = tt(:);
    q3(2,:) = tt';
    
    
    tt = flipud(reshape(p4(1,:),5,9))';
    tt = tt(:);
    q4(1,:) = tt';
    
    tt = flipud(reshape(p4(2,:),5,9))';
    tt = tt(:);
    q4(2,:) = tt';
    
    
elseif(frame == 4)
    
    idx = reshape([1:45],9,5);
    idx = idx(1:8,:);
    idx = idx(:);
    q1 = p1(:,idx);
    q2 = p2(:,idx);
    q3 = p3;
    q4 = p4(:,idx);
    
    
else
    q1 = p1;
    q2 = p2;
    q3 = p3;
    q4 = p4;
end




N = size(q1,2);

Allq{1,1} = q1;
Allq{2,1} = q2;
Allq{3,1} = q3;
Allq{4,1} = q4;

close all

for ii = 1:4
    
    im = Allim{ii,1};
    figure;imshow(im);hold on;
    
    p = Allq{ii,1};
    
    for jj = 1:N
        plot(p(1,jj),p(2,jj),'r*');
        text(p(1,jj),p(2,jj),num2str(jj));
        pause(0.05);
    end
    
end

%write data into files
for ii = 0:3
    
    p = Allq{ii+1};
    
    %p = round(p);
    
    str = sprintf('Cam%d_Frame%02d.txt',ii,frame)
    
    fp = fopen(str,'w');
    
    %fprintf(fp,'%d\n',N);
    
    for jj = 1:N
        fprintf(fp,'%f\t%f\n',p(1,jj),p(2,jj));
    end
    
    fclose(fp);
end




im = [im3 im2];
figure;imshow(im);hold on;
p = Allq{4};
for jj = 1:N
    plot(p(1,jj),p(2,jj),'r*');
    text(p(1,jj),p(2,jj),num2str(jj));
end
p = Allq{3};
for jj = 1:N
    plot(p(1,jj)+400,p(2,jj),'r*');
    text(p(1,jj)+400,p(2,jj),num2str(jj));
end


im = [im1 im0];
figure;imshow(im);hold on;
p = Allq{2};
for jj = 1:N
    plot(p(1,jj),p(2,jj),'r*');
    text(p(1,jj),p(2,jj),num2str(jj));
end
p = Allq{1};
for jj = 1:N
    plot(p(1,jj)+400,p(2,jj),'r*');
    text(p(1,jj)+400,p(2,jj),num2str(jj));
end














