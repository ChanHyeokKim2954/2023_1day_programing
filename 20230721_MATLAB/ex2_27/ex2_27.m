clear all;
close all;
clc;

theta=[0:0.001:2*pi];
R=3;

x0=3;
y0=4;

x=x0+R*cos(theta);
y=y0+R*sin(theta);

plot(x,y);
axis([0 10 0 10]);
axis equal;
grid on;