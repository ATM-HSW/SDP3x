s=serial('COM6','Baudrate',115200)
fopen(s)
fwrite(s,1.0,'float32'); fread(s,1,'float32');
pause(0.5);
fwrite(s,1.0,'float32'); fread(s,1,'float32') 

pause(1);

fwrite(s,0.0,'float32'); fread(s,1,'float32');
pause(0.5);
fwrite(s,0.0,'float32'); fread(s,1,'float32') 

pause(5);

for kk=1:100; 
    if kk>20
        fwrite(s,1.0,'float32'); 
    else
        fwrite(s,0.0,'float32'); 
    end
    while(s.BytesAvailable<4) end, 
    rr(kk)=fread(s,1,'float32'); 
    pause(.1); 
end
 
fclose(s);

plot(rr);
clear all;