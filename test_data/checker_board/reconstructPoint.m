
function [M] = reconstructPoint(m1, m2, K1, K2, R, t)

%   RECONSTRUCTPOINT Reconstruct a 3D point
%   [M] = RECONSTRUCTPOINT(X1,X2,K1,K2,R,T) takes as input two points,
%   xy-coordinates that correspond to the same 3D point in two images, as
%   well as the two instrinsic camera matrices K1 and K2, and the motion
%   parameters between the two views (the rotation R and translation T),
%   and returns to the user the reconstructed point in three dimensions.
%   The reconstruction uses the linear least-squares method.

% File:    reconstructPoint.m
% Author:  Ambrish Tyagi
% Date:    September 16, 2005



if(size(m1,1) == 3)
    m1 = m1/m1(3);
end

if(size(m2,1) == 3)
    m2 = m2/m2(3);
end


B = K2 * R;

Z = [K1(1,:) - m1(1)*K1(3,:);...
    K1(2,:) - m1(2)*K1(3,:);...
    B(1,:) - m2(1)*B(3,:);...
    B(2,:) - m2(2)*B(3,:)];

z = [0; 0; (m2(1)*K2(3,:) - K2(1,:))*t; (m2(2)*K2(3,:) - K2(2,:))*t];

% pZ = pinv(Z);
% M = pZ*z;

M = Z\z;

