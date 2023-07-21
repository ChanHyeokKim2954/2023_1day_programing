close all;
clear all;
clc;

N=100;
x0=10*rand(N,1);
y0=10*rand(N,1);
R=10*rand(N,1);

fig=figure;
axis([-20 20 -20 20]);
axis equal;
grid on;
hold on;

for n=1:N
    draw_circle(x0(n),y0(n),R(n),fig);
    pause(0.1);
end
