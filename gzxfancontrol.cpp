#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>

const float C_TEMP_MIN = 45.0f;
const float C_TEMP_MAX = 85.0f;
const float C_TEMP_RANGE = C_TEMP_MAX - C_TEMP_MIN;

const int C_SPEED_MIN = 0x00;
const int C_SPEED_MAX = 0x64;
const int C_SPEED_RANGE = C_SPEED_MAX - C_SPEED_MIN;

const float C_SPEED_PER_DEGREE = (float)C_SPEED_RANGE / C_TEMP_RANGE;
const float C_DEGREE_PER_SPEED = C_TEMP_RANGE / (float)C_SPEED_RANGE;

float GetTemp()
{
	static const size_t max_zones = 2;
	static const std::string filenames[max_zones]{
		"/sys/class/thermal/thermal_zone0/temp",
		"/sys/class/thermal/thermal_zone1/temp"
	};

	static std::fstream temperature_file[max_zones]{
		std::fstream(filenames[0], std::ios_base::in),
		std::fstream(filenames[1], std::ios_base::in)
	};

	static float temperature[max_zones]{ C_TEMP_MAX, C_TEMP_MAX };

	float max_temp = C_TEMP_MIN;

	for (size_t i = 0; i < max_zones; ++i)
	{
		if (!temperature_file[i])
		{
			temperature_file[i].close();
			temperature_file[i].open(filenames[i], std::ios_base::in);
			return C_TEMP_MAX;
		}

		temperature_file[i].seekg(0, temperature_file[i].beg);
		if (!(temperature_file[i] >> temperature[i]))
		{
			temperature_file[i].close();
			temperature_file[i].open(filenames[i], std::ios_base::in);
			return C_TEMP_MAX;
		}

		temperature[i] /= 1000.0f;
		if (max_temp < temperature[i])
		{
			max_temp = temperature[i];
		}
	}

	return std::min(std::max(max_temp, C_TEMP_MIN), C_TEMP_MAX);
}

void exec(const char* cmd)
{
	FILE* process = popen(cmd, "r");
	if (process)
	{
		pclose(process);
	}
}

void SetFanSpeed(int speed)
{
	speed = std::min(std::max(speed, C_SPEED_MIN), C_SPEED_MAX);

	std::stringstream stream;

	stream << "ipmitool raw 0x30 0x70 0x66 0x01 0x00 0x";

	if (speed <= 0x0F)
	{
		stream << "0";
	}

	stream << std::hex << speed;

	exec(stream.str().c_str());
}

int CalculateSpeed(float temp)
{
	float temp_offset = temp - C_TEMP_MIN;
	return (int)(temp_offset * C_SPEED_PER_DEGREE);
}

int main()
{
	while (true)
	{
		auto sleep_until = std::chrono::steady_clock::now() + std::chrono::milliseconds(250);

		float temp = GetTemp();
		int speed = CalculateSpeed(temp);
		SetFanSpeed(speed);

#ifndef NDEBUG
		std::cout << GetTemp() << " / " << speed << std::endl;
#endif

		std::this_thread::sleep_until(sleep_until);
	}

	return 0;
}
