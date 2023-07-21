clear all;
close all;
clc;

t=[1:0.01:2*pi];

figure;
for f=5:0.1:10
    y=sin(f*t);

    plot(t,y);
    grid on;
    xlabel('t');
    ylable(['sin(' num2str(f,'%3.1f') 't)']);
   %pause(0.1);
end