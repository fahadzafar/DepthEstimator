

function [KL,DistL,RotL,tL] = ReadCalibrationFile(str)

CL = load(str);

KL = reshape(CL(1:9),3,3)';
DistL = CL(10:17);
RotL = rodrigues(CL(18:20)*pi/180);
tL = CL(21:23);

