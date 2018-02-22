
ccc
format short

frame = 0

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



deviceID = 'B0F103E134230335'


dirstr = '/home/local/ANT/aaagrawa/DataDroppedDevices/Device0835_Calib24Feb2014/'




str = sprintf('%s/StereoParameters_%s_2_3.xml',dirstr,deviceID);
[K2,Dist2_1,K3,Dist3_1,R23,t23,~,~] = ReadCalibrationParams(str);
str = sprintf('%s/StereoParameters_%s_3_1.xml',dirstr,deviceID);
[K3_1,Dist3,K1,Dist1_1,R31,t31,~,~] = ReadCalibrationParams(str);
str = sprintf('%s/StereoParameters_%s_0_1.xml',dirstr,deviceID);
[K0,Dist0_1,K1_1,Dist1,R01,t01,~,~] = ReadCalibrationParams(str);
str = sprintf('%s/StereoParameters_%s_0_2.xml',dirstr,deviceID);
[K0_1,Dist0,K2_1,Dist2,R02,t02,~,~] = ReadCalibrationParams(str);



for ii = 0:3
    str = sprintf('Cam%d_Frame%02d.txt',ii,frame)
    x = load(str)';
    Allxy{ii+1,1} = x;
end


% for ii = 0:3
%     str = sprintf('Cam%d_Frame%02d.txt',ii,0)
%     x1 = load(str)';
%
%     str = sprintf('Cam%d_Frame%02d.txt',ii,1)
%     x2 = load(str)';
%
%     x = [x1 x2];
%     Allxy{ii+1,1} = x;
% end


N = size(x,2)

AllK{1,1} = K0;
AllK{2,1} = K1;
AllK{3,1} = K2;
AllK{4,1} = K3;
AllDist = [Dist0 Dist1 Dist2 Dist3];

fc = [1;1];
cc = [0;0];
alpha_c = 0;
kc = zeros(5,1);

[xx,yy] = meshgrid(0:8,0:4);
xx = xx(:)*49.026;
yy = yy(:)*49.026;
planePoints = [xx' ; yy' ; zeros(1,N)];


clear xx yy

for ii = 1:4
    xy = Allxy{ii,1};
    
    xyChecker = xy;
    
    im = Allim{ii,1};
    figure;imshow(im);hold on;
    plot(xy(1,:),xy(2,:),'r*');
    
    
    K = AllK{ii,1};
    Dist = AllDist(:,ii);
    
    xy = ProjectionFunction(inv(K)*[xy;ones(1,N)]);
    xy = UndistortOpenCV(xy,Dist,0);
    
    xyUndistorted{ii,1} = xy;
    
    
    [omckk,tc,Rc,H,x,ex2,JJ] = compute_extrinsic(xy,planePoints,fc,cc,kc,alpha_c);
    Rcall(:,:,ii) = Rc;
    tcall(:,ii) = tc;
    
    
    
    
    
    XYZ = Rc*planePoints + tc*ones(1,N);
    
    xy = ProjectionFunction(XYZ);
    
    % apply distortions
    [xy(1,:),xy(2,:)] = ApplyOpenCVDistortion(xy(1,:),xy(2,:),Dist,0);
    
    xy = ProjectionFunction(K*[xy;ones(1,N)]);
    
    e = abs(xy - xyChecker);
    e
    
    
    plot(xy(1,:),xy(2,:),'gs');
    
    
end

R0 = Rcall(:,:,1);
R1 = Rcall(:,:,2);
R2 = Rcall(:,:,3);
R3 = Rcall(:,:,4);



t0 = tcall(:,1);
t1 = tcall(:,2);
t2 = tcall(:,3);
t3 = tcall(:,4);


R01 = R1*R0';
t01 = t1 - R01*t0;

R10 = R01';
t10 = -R01'*t01;


R31 = R1*R3';
t31 = t1 - R31*t3;


R23 = R3*R2';
t23 = t3 - R23*t2;

R02 = R2*R0';
t02 = t2 - R02*t0;

R20 = R02';
t20 = -R02'*t02;

R03 = R3*R0';
t03 = t3 - R03*t0;

R30 = R03';
t30 = -R03'*t03;



rot2rpy(R23)'*180/pi
rot2rpy(R31)'*180/pi
rot2rpy(R01)'*180/pi
rot2rpy(R02)'*180/pi


estR(:,:,1) = R23;
estR(:,:,2) = R31;
estR(:,:,3) = R01;
estR(:,:,4) = R02;



estT = [t23 t31 t01 t02]


%
%
%
%
% rodrigues(R23)'*180/pi
% rodrigues(R31)'*180/pi
% rodrigues(R01)'*180/pi
% rodrigues(R02)'*180/pi
%


















