
ccc


frame = 4


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





for ii = 0:3
    
    myimshow(Allim{ii+1,1})
    
    str = sprintf('Cam%d_Frame%02d.txt',ii,frame)
    p = load(str)';
    
    N = size(p,2);
    
    for jj = 1:N
        plot(p(1,jj),p(2,jj),'r*');
        text(p(1,jj),p(2,jj),num2str(jj));
        pause(0.05);
    end
    
    
end




