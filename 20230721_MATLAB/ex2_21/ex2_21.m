clear all;
close all;
clc;

Tmax=50;
dT=0.01;
t=0;

index = 1;

while(t<=Tmax)
    t_array(index) = t;
    y(index) = sin(t/pi);

    index = index + 1;
    t = t + dT;
end

plot(t_array,y);