#include <stdio.h>
#define No_Calibration_Point 5
#define VALUE -2

struct 
{
	double X[No_Calibration_Point];
	double Y[No_Calibration_Point];
}cal_data;

int i;
double y;

double linear_mapping(double x)
{
	double x1 = cal_data.X[i];
	double x2 = cal_data.X[i + 1];
	double y1 = cal_data.Y[i];
	double y2 = cal_data.Y[i + 1];

	for (int j = 0; j < No_Calibration_Point-1; j++) {
		if (x < cal_data.X[0])		i = 0;
		else if (x >= cal_data.X[j] && x < cal_data.X[j + 1])	i = j;
		else i = No_Calibration_Point-2;
	}
	return y = ((y2 - y1) / (x2 - x1)) * (x - x1) + y1;
}

int main(void)
{
	cal_data = {
		{ 0.0 ,1.0 ,2.0 ,3.0 ,4.0},
		{-30.0, -15.0, 0.0, 15.0, 30.0}
	};

	y = linear_mapping(VALUE);

	printf("%lf \n", y);
}