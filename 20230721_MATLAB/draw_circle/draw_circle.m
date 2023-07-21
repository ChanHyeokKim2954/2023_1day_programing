function draw_circle(x0,y0,R,fig)
theta=[0:0.001:2*pi];

x=x0+R*cos(theta);
y=y0+R*sin(theta);

figure(fig);
plot(x,y);
return

