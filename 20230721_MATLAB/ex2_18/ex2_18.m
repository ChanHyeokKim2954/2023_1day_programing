clear all;
close all;
clc;

for n=2:9
    for m=1:9
        table(m,n)=n*m;
    end
end

table(:,2:end)

figure;
mesh(table);