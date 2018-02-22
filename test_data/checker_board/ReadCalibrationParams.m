

function [K1,Dist1,K2,Dist2,R,T,Rect1,Rect2] = ReadCalibrationParams(str)

fp = fopen(str,'r');

a1 = [];

while(~feof(fp))
    
    tt = fgets(fp);
    
    if(strfind(tt,'<data>'))
        
        %start reading from next line
        
        while(1)
            tt2 = fgets(fp);
            c = strfind(tt2,'</data>');
            
            if(isempty(c))
                val = sscanf(tt2,'%f');
                a1 = [a1; val];
                
            else
                val = sscanf(tt2(1:c-1),'%f');
                a1 = [a1; val];
                break;
            end
        end
    end
    
    
    %     if(size(a1,1) == 46)
    %         break
    %     end
    
    
end




K1 = reshape(a1(1:9),3,3)';
Dist1 = a1(10:17);


K2 = reshape(a1(18:26),3,3)';
Dist2 = a1(27:34);

R = reshape(a1(35:43),3,3)';
T = a1(44:46) * 1000;

Rect1 = reshape(a1(47:55),3,3)';
Rect2 = reshape(a1(56:64),3,3)';

P1 = reshape(a1(65:76),4,3)';
P2 = reshape(a1(77:88),4,3)';







fclose(fp);

