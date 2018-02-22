
% estT =
%    -0.9992    0.0017   -0.9999   -0.0067
%     0.0129   -1.0000   -0.0009    0.9996
%     0.0366   -0.0093   -0.0164   -0.0278
% ans =
%     1.1008    0.4159   -0.8648
% ans =
%     1.1815   -1.2033    0.5751
% ans =
%    -0.6851   -0.6707   -0.0289
% ans =
%    -2.8464    0.1427    0.2239



ccc
format short


dirstr = ''

str = sprintf('%s/StereoParameters_2_3.xml',dirstr);
[K2,Dist2_1,K3,Dist3_1,R23,t23,~,~] = ReadCalibrationParams(str);
str = sprintf('%s/StereoParameters_3_1.xml',dirstr);
[K3_1,Dist3,K1,Dist1_1,R31,t31,~,~] = ReadCalibrationParams(str);
str = sprintf('%s/StereoParameters_0_1.xml',dirstr);
[K0,Dist0_1,K1_1,Dist1,R01,t01,~,~] = ReadCalibrationParams(str);
str = sprintf('%s/StereoParameters_0_2.xml',dirstr);
[K0_1,Dist0,K2_1,Dist2,R02,t02,~,~] = ReadCalibrationParams(str);





for ii = 0:3
    str = sprintf('Cam%d_Frame%02d.txt',ii,0)
    x1 = load(str)';
    
    str = sprintf('Cam%d_Frame%02d.txt',ii,1)
    x2 = load(str)';
    
    str = sprintf('Cam%d_Frame%02d.txt',ii,2)
    x3 = load(str)';
    
    str = sprintf('Cam%d_Frame%02d.txt',ii,3)
    x4 = load(str)';
    
    str = sprintf('Cam%d_Frame%02d.txt',ii,4)
    x5 = load(str)';
    
    str = sprintf('Cam%d_Frame%02d.txt',ii,5)
    x6 = load(str)';
    
    x = [x1 x2 x3 x4 x5 x6];
    Allxy{ii+1,1} = x;
end



AllK{1,1} = K0;
AllK{2,1} = K1;
AllK{3,1} = K2;
AllK{4,1} = K3;
AllDist = [Dist0 Dist1 Dist2 Dist3];

clear xx yy

for ii = 1:4
    xy = Allxy{ii,1};
    
    N = size(xy,2)
    K = AllK{ii,1};
    Dist = AllDist(:,ii);
    
    xy = ProjectionFunction(inv(K)*[xy;ones(1,N)]);
    xy = UndistortOpenCV(xy,Dist,1);
    
    xyUndistorted{ii,1} = xy;
    
    
end

[R23,t23,cc,XYZ] = EstimateRtFromNormalizedCorrespondences(xyUndistorted{3,1},xyUndistorted{4,1},1);
[R31,t31,cc,XYZ] = EstimateRtFromNormalizedCorrespondences(xyUndistorted{4,1},xyUndistorted{2,1},1);
[R01,t01,cc,XYZ] = EstimateRtFromNormalizedCorrespondences(xyUndistorted{1,1},xyUndistorted{2,1},1);
[R02,t02,cc,XYZ] = EstimateRtFromNormalizedCorrespondences(xyUndistorted{1,1},xyUndistorted{3,1},1);


% compute epipolar error
x = [xyUndistorted{3,1} ; ones(1,N)];
y = [xyUndistorted{4,1} ; ones(1,N)];
ff = (K3(1,1) + K3(2,2) + K2(1,1) + K2(2,2))/4
ep23 = abs(ff*diag(y'*CrossMatrix(t23)*R23*x));
myplot(ep23)

% compute epipolar error
x = [xyUndistorted{4,1} ; ones(1,N)];
y = [xyUndistorted{2,1} ; ones(1,N)];
ff = (K3(1,1) + K3(2,2) + K1(1,1) + K1(2,2))/4
ep31 = abs(ff*diag(y'*CrossMatrix(t31)*R31*x));
myplot(ep31)

x = [xyUndistorted{1,1} ; ones(1,N)];
y = [xyUndistorted{2,1} ; ones(1,N)];
ff = (K0(1,1) + K0(2,2) + K1(1,1) + K1(2,2))/4
ep01 = abs(ff*diag(y'*CrossMatrix(t01)*R01*x));
myplot(ep01)

x = [xyUndistorted{1,1} ; ones(1,N)];
y = [xyUndistorted{3,1} ; ones(1,N)];
ff = (K0(1,1) + K0(2,2) + K2(1,1) + K2(2,2))/4
ep02 = abs(ff*diag(y'*CrossMatrix(t02)*R02*x));
myplot(ep02)



estR(:,:,1) = R23;
estR(:,:,2) = R31;
estR(:,:,3) = R01;
estR(:,:,4) = R02;



estT = [t23 t31 t01 t02]

estEuler = [rot2rpy(R23)*180/pi rot2rpy(R31)*180/pi rot2rpy(R01)*180/pi rot2rpy(R02)*180/pi]

strSave = sprintf('Device230A_Calibration.mat')
save(strSave)




%
%
%
%
% rodrigues(R23)'*180/pi
% rodrigues(R31)'*180/pi
% rodrigues(R01)'*180/pi
% rodrigues(R02)'*180/pi
%






% estT =
%    -0.9995   -0.0021   -1.0000    0.0065
%     0.0156   -0.9999    0.0013    0.9986
%     0.0273   -0.0155    0.0072   -0.0529
% estEuler =
%     2.0942    1.6106    0.5046   -2.9720
%     0.4781    0.0177    0.4171   -0.1347
%    -0.2979    0.6070   -0.3192   -0.6018

%
%
% estT =
%    -0.9995   -0.0020   -1.0000    0.0066
%     0.0158   -0.9999    0.0011    0.9985
%     0.0258   -0.0162    0.0059   -0.0539
% estEuler =
%     2.1344    1.6479    0.4682   -3.1009
%     0.2854    0.0859    0.1988   -0.2298
%    -0.2997    0.5858   -0.3142   -0.6149
% strSave =

