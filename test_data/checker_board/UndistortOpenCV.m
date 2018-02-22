


% undistorts normalized coordinates given by xd (2 by N matrix)
% kc is 8 by 1 array.
% if flag is 1, only radial undistortion is done


function [uv] = UndistortOpenCV(xy,kc,flag)

p1 = kc(3);
p2 = kc(4);

n1 = kc(1);
n2 = kc(2);
n3 = kc(5);
d1 = kc(6);
d2 = kc(7);
d3 = kc(8);

x = xy(1,:);
y = xy(2,:);

u = x;
v = y;


for ii = 1:100
    
    
    r2 = u.^2 + v.^2;
    
    k_radial = (1 + n1*r2 + n2*r2.^2 + n3*r2.^3) ./ (1 + d1*r2 + d2*r2.^2 + d3*r2.^3);
    
    if(flag)
        u = x./k_radial;
        v = y./k_radial;
        
    else
        delta_x = 2*p1*u.*v + p2*(r2 + 2*u.^2);
        
        delta_y = p1*(r2 + 2*v.^2) + 2*p2*u.*v;
        
        u = (x - delta_x)./k_radial;
        v = (y - delta_y)./k_radial;
    end
    
    
end


% compute final error

r2 = u.^2 + v.^2;

k_radial = (1 + n1*r2 + n2*r2.^2 + n3*r2.^3) ./ (1 + d1*r2 + d2*r2.^2 + d3*r2.^3);


if(flag)
    x1 = u.*k_radial;
    y1 = v.*k_radial;
    
else
    delta_x = 2*p1*u.*v + p2*(r2 + 2*u.^2);
    
    delta_y = p1*(r2 + 2*v.^2) + 2*p2*u.*v;
    
    x1 = u.*k_radial + delta_x;
    y1 = v.*k_radial + delta_y;
end

ex = abs(x - x1);
ey = abs(y - y1);


% myplot(ex(:));hold on
% plot(ey(:),'g-*');hold on
% title('redistorting undistorted points and computing error');
% legend('x','y');

uv(1,:) = u;
uv(2,:) = v;
















