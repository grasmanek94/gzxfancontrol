#include <cstdio>
#include <cstdlib>
#include <unistd.h>

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

int GetFanSpeed(float min_temp, float max_temp, float current_temp_a, float current_temp_b)
{
	float current_temp = current_temp_a < current_temp_b ? current_temp_b : current_temp_a;
	float diff_temp = max_temp - min_temp;
	float current_progress = current_temp < min_temp ? 0.0f : (current_temp - min_temp);
	float output_speed = lerp(0.0f, 255.0f, current_progress / diff_temp);
	return (int)output_speed;
}

int main()
{
	float MIN_TEMP = 42000.0f;
	float MAX_TEMP = 82000.0f;

	char buffer[128];

	FILE *temp_a;
	FILE *temp_b;

	temp_a = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	temp_b = fopen("/sys/class/thermal/thermal_zone1/temp", "r");

	float r_temp_a = 0.00f;
	float r_temp_b = 0.00f;

	int fan_speed = 0;

	while (1)
	{
		fseek(temp_a, 0, SEEK_SET);
		fseek(temp_b, 0, SEEK_SET);

		fread(buffer, 128, 1, temp_a);
		r_temp_a = (float)atoi(buffer);

		fread(buffer, 128, 1, temp_b);
		r_temp_b = (float)atoi(buffer);

		fan_speed = GetFanSpeed(MIN_TEMP, MAX_TEMP, r_temp_a, r_temp_b);
		if(fan_speed > 255)
		{
			fan_speed = 255;
		}
		sprintf(buffer, "ipmitool raw 0x30 0x70 0x66 0x01 0x00 0x%02x", fan_speed);

		system(buffer);

		printf("%f %f\n", r_temp_a, r_temp_b);

		usleep(1);
	}

	fclose(temp_a);
	fclose(temp_b);

	return 0;
}
