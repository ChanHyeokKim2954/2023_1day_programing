clear all;
close all;
clc;

t0=0;
tf=20;
x0=[0 0.25];

[t,x]=ode23(@VanDerPol,[t0 tf],x0);

plot(t,x);