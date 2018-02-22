


% kc is 8 by 1 array.
% if flag is 1, only radial undistortion is done


function [u,v] = ApplyOpenCVDistortion(x,y,kc,flag)

p1 = kc(3);
p2 = kc(4);

n1 = kc(1);
n2 = kc(2);
n3 = kc(5);
d1 = kc(6);
d2 = kc(7);
d3 = kc(8);



r2 = x.^2 + y.^2;
k_radial = (1 + n1*r2 + n2*r2.^2 + n3*r2.^3) ./ (1 + d1*r2 + d2*r2.^2 + d3*r2.^3);


if(flag)
    u = x.*k_radial;
    v = y.*k_radial;
else
    delta_x = 2*p1*x.*y + p2*(r2 + 2*x.^2);
    delta_y = p1*(r2 + 2*y.^2) + 2*p2*x.*y;
    u = x.*k_radial + delta_x;
    v = y.*k_radial + delta_y;
end


















