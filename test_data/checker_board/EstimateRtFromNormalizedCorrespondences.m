

%xy1 is a 2 by N matrix of normalized points in first camera
%xy2 is in second camera
%R,t is the relative rotation and translation. Xr = R*Xl + t

function [Rout,tout,cc,XYZ] = EstimateRtFromNormalizedCorrespondences(xy1,xy2,SCALING)

th = 1;


N = size(xy1,2);

if(size(xy1,1) == 2)
    xy1 = [xy1; ones(1,N)];
end

if(size(xy2,1) == 2)
    xy2 = [xy2; ones(1,N)];
end


if(SCALING)
    
    m1 = mean(xy1(1:2,:),2);
    
    s = 0;
    for ii = 1:N
        tt =  xy1(1:2,ii) - m1;
        s = s + norm(tt);
    end
    s = s/N;
    ff = sqrt(2)/s;
    T1 = [ff 0 -ff*m1(1) ; 0 ff -ff*m1(2) ; 0 0 1];
    clear ff m1 s
    
    m2 = mean(xy2(1:2,:),2);
    s = 0;
    for ii = 1:N
        tt =  xy2(1:2,ii) - m2;
        s = s + norm(tt);
    end
    s = s/N;
    ff = sqrt(2)/s;
    T2 = [ff 0 -ff*m2(1) ; 0 ff -ff*m2(2) ; 0 0 1];
    
    
    %     %scaling etc
    %     K1 = zeros(3,3);
    %     K2 = zeros(3,3);
    %     for ii = 1:N
    %         K1 = K1 + xy1(:,ii)*xy1(:,ii)';
    %         K2 = K2 + xy2(:,ii)*xy2(:,ii)';
    %     end
    %     T1 = inv(chol(K1)');
    %     T2 = inv(chol(K2)');
    %
    
    xy1t = T1*xy1;
    xy2t = T2*xy2;
    
    
    %     Kt1 = zeros(3,3);
    %     Kt2 = zeros(3,3);
    %     for ii = 1:N
    %         Kt1 = Kt1 + xy1t(:,ii)*xy1t(:,ii)';
    %         Kt2 = Kt2 + xy2t(:,ii)*xy2t(:,ii)';
    %     end
    
    
    
else
    xy1t = xy1;
    xy2t = xy2;
end






Rout = eye(3);
tout = zeros(3,1);




% % make A matrix
% A = zeros(N,9);
% for ii = 1:N
%     A(ii,:) = kron(xy2(:,ii)',xy1(:,ii)');
% end
% [~,~,vv] = svd(A,0);
% vv = vv(:,end);
% %get essential matrix
% Etg = reshape(vv,3,3);



% make A matrix
A = zeros(N,9);
for ii = 1:N
    A(ii,:) = kron(xy2t(:,ii)',xy1t(:,ii)');
end
[~,D,vv] = svd(A,0);

% fprintf('condition number = %f\n',cond(A));
% diag(D)'

vv = vv(:,end);
%get essential matrix
Et = reshape(vv,3,3);




%
% clear ee
%
% for run = 1:100
%
%     % take 8 points
%     idx = [];
%     while(size(idx,1) < 8)
%         idx = 1 + (N-1)*rand(8,1);
%         idx = round(idx);
%         idx = unique(idx);
%     end
%     Asmall = A(idx,:);
%
%     % do svd
%     [~,~,vv] = svd(Asmall);
%     vv = vv(:,end);
%
%     %get essential matrix
%     Et = reshape(vv,3,3);
%
%     tt = abs(A*Et(:));
%     tt = tt * 270/50;
%     idx = find(tt < th);
%     numInliner(run,1) = size(idx,1);
%
%     ee(run,1) = median(abs(A*Et(:)));
%
%     %     % make singular values to be 1,1,0
%     %     [U,~,V] = svd(Et);
%     %     Et = U*diag([1;1;0])*V';
%
%
%     EEsave(:,:,run) = Et;
% end
%
% [~,idx] = min(ee);
%
% %[~,idx] = max(numInliner);
%
%
% idx = idx(1);
%
% Et = EEsave(:,:,idx);
%
%




if(SCALING)
    Et = T1'*Et*T2;
end


% make singular values to be 1,1,0
[U,~,V] = svd(Et);
Et = U*diag([1;1;0])*V';
clear U V




% find rotation from E
[U,~,V] = svd(Et);

%use Hartley matrices:
Wmat = [0 -1 0; 1 0 0; 0 0 1];
%Zmat = [0 1 0; -1 0 0; 0 0 0];
% get two rotations

Re1 = U*Wmat*V';
Re2 = U*Wmat'*V';

%check if not a reflection matrix
if(det(Re1) < 0)
    Re1 = -Re1;
end

if(det(Re2) < 0)
    Re2 = -Re2;
end

%find normalized translation
te1 = U(:,3);

dd = abs(te1'*[1;0;0]);
dd = min(1,dd);
dd = max(0,dd);
dd = acosd(dd);

% if(dd > 45)
%     keyboard
% end



% we have 4 solutions due to twisted pair ambiguity, (R1,t) (R1,-t) (R2,t) (R2,-t)
Re(:,:,1) = Re1;
Re(:,:,2) = Re1;
Re(:,:,3) = Re2;
Re(:,:,4) = Re2;

te(:,1) = te1;
te(:,2) = -te1;
te(:,3) = te1;
te(:,4) = -te1;

cc = 0;


for jj = 1:4
    
    %for each solution, get inverse rotation and translation for 3D points
    pR = Re(:,:,jj)';
    pt = -Re(:,:,jj)'*te(:,jj);
    
    for ii = 1:N
        M1(:,ii) = reconstructPoint(xy1(:,ii),xy2(:,ii),eye(3),eye(3),pR,pt);
    end
    
    %point in other camera
    M2 = pR*M1 + pt*ones(1,N);
    
    %find whether Z is in front or back for both cameras
    idx1 = find(M1(3,:) < 0);
    idx2 = find(M2(3,:) < 0);
    
    
    if(isempty(idx1) && isempty(idx2))
        cc = cc + 1;
        % points are in front for both cameras
        Rout(:,:,cc) = Re(:,:,jj)';
        tout(:,cc) = -Re(:,:,jj)'*te(:,jj);
    end
    
    %
    %     figure;
    %     plot(M1(3,:),'r-*');hold on
    %     plot(M2(3,:),'g-*');
    %     legend('first camera','second camera');
    %     title('z of points')
    %
    
    
end


% do reconstruction again
clear M* XYZ*
XYZ = zeros(3,N);


if(cc == 1)
    for ii = 1:N
        XYZ(:,ii) = reconstructPoint(xy1(:,ii),xy2(:,ii),eye(3),eye(3),Rout,tout);
    end
end
















