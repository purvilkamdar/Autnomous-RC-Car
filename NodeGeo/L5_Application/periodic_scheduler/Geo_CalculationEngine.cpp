/*
 * Geo_CalculationEngine.cpp
 *
 *  Created on: Dec 4, 2016
 *      Author: ythao
 */

#include "Geo_CalculationEngine.h"
#include "geo_data.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "io.hpp"

#define std_deviation_factor 2 //Increasing this number widens the GPS coordinate filter range

#define decimalDegrees 0.000001
#define meterPerDecimalDegree(latitude) ((0.00005*pow(latitude,3) - 0.01912*pow(latitude,2) + 0.02642*latitude + 111.32)*0.001)

Geo_CalculationEngine::Geo_CalculationEngine(){
	distance_Magnitude = 0;
	angleError_ToNorth = 0;
}

Geo_CalculationEngine::~Geo_CalculationEngine(){

}


void Geo_CalculationEngine::deviation_filter_controller(geo_data geo_data_, calculated_geo_data &calculated_geo_data_){

	if(geo_data_.GPS_message_status == valid)
	{
		if(calculated_geo_data_.counter >= arr_size)
		{
		    //printf("Counter is greater than arr_size of %i\n", arr_size);

			int i = 0;

			double latitude_mean = 0;
			for(i = 0; i < arr_size; i++)
				latitude_mean += calculated_geo_data_.geo_circularArr[lat_Arr][i];
			latitude_mean = latitude_mean/arr_size;
			calculated_geo_data_.c_lat_mean = latitude_mean;

			double latitude_std_deviation = 0;
			for(i = 0; i < arr_size; i++)
				latitude_std_deviation += pow(fabs(calculated_geo_data_.geo_circularArr[lat_Arr][i] - latitude_mean ),2);
			latitude_std_deviation = sqrt(latitude_std_deviation/(arr_size-1))*std_deviation_factor;

			double longitude_mean = 0;
			for(i = 0; i < arr_size; i++)
				longitude_mean += calculated_geo_data_.geo_circularArr[long_Arr][i];
			longitude_mean = longitude_mean/arr_size;
			calculated_geo_data_.c_long_mean = longitude_mean;

			double longitude_std_deviation = 0;
			for(i = 0; i < arr_size; i++)
				longitude_std_deviation += pow(fabs(calculated_geo_data_.geo_circularArr[lat_Arr][i] - longitude_mean ),2);
			longitude_std_deviation = sqrt(longitude_std_deviation/(arr_size-1))*std_deviation_factor;

			//Updating calculated_geo_data array for latitude & longitude
			if(calculated_geo_data_.arr_index > 0)
			{
				if(geo_data_.latitude > (calculated_geo_data_.geo_circularArr[lat_Arr][calculated_geo_data_.arr_index-1] - latitude_std_deviation) && geo_data_.latitude < (calculated_geo_data_.geo_circularArr[lat_Arr][calculated_geo_data_.arr_index-1] + latitude_std_deviation))
				{
					calculated_geo_data_.geo_circularArr[lat_Arr][calculated_geo_data_.arr_index] = geo_data_.latitude;
					LE.off(1);
				}
				else
					{
						calculated_geo_data_.geo_circularArr[lat_Arr][calculated_geo_data_.arr_index] = calculated_geo_data_.geo_circularArr[lat_Arr][calculated_geo_data_.arr_index - 1];
						LE.on(1);
					}

				if(geo_data_.longitude > (calculated_geo_data_.geo_circularArr[long_Arr][calculated_geo_data_.arr_index-1] - longitude_std_deviation) && geo_data_.longitude < (calculated_geo_data_.geo_circularArr[long_Arr][calculated_geo_data_.arr_index-1] + longitude_std_deviation))
				{
					calculated_geo_data_.geo_circularArr[long_Arr][calculated_geo_data_.arr_index] = geo_data_.longitude;
					LE.off(4);
				}
				else
					{
						calculated_geo_data_.geo_circularArr[long_Arr][calculated_geo_data_.arr_index] = calculated_geo_data_.geo_circularArr[long_Arr][calculated_geo_data_.arr_index - 1];
						LE.on(4);
					}
			}
			else if (calculated_geo_data_.arr_index == 0)
				{
					if(geo_data_.latitude > (calculated_geo_data_.geo_circularArr[lat_Arr][arr_size - 1] - latitude_std_deviation) && geo_data_.latitude < (calculated_geo_data_.geo_circularArr[lat_Arr][arr_size - 1] + latitude_std_deviation))
					{
						calculated_geo_data_.geo_circularArr[lat_Arr][calculated_geo_data_.arr_index] = geo_data_.latitude;
						LE.off(1);
					}
					else
						{
							calculated_geo_data_.geo_circularArr[lat_Arr][calculated_geo_data_.arr_index] =calculated_geo_data_.geo_circularArr[lat_Arr][arr_size - 1];
							LE.on(1);
						}

					if(geo_data_.longitude > (calculated_geo_data_.geo_circularArr[long_Arr][arr_size - 1] - longitude_std_deviation) && geo_data_.longitude < (calculated_geo_data_.geo_circularArr[long_Arr][arr_size - 1] + longitude_std_deviation))
					{
						calculated_geo_data_.geo_circularArr[long_Arr][calculated_geo_data_.arr_index] = geo_data_.longitude;
						LE.off(4);
					}
					else
						{
							calculated_geo_data_.geo_circularArr[long_Arr][calculated_geo_data_.arr_index] = calculated_geo_data_.geo_circularArr[long_Arr][arr_size - 1];
							LE.on(4);
						}
				}
		}
		else
			{
				calculated_geo_data_.geo_circularArr[lat_Arr][calculated_geo_data_.arr_index] = geo_data_.latitude;
				calculated_geo_data_.geo_circularArr[long_Arr][calculated_geo_data_.arr_index] = geo_data_.longitude;
			}

		calculated_geo_data_.geo_circularArr[time_Arr][calculated_geo_data_.arr_index] = calculated_geo_data_.counter;
		calculated_geo_data_.counter++;
		calculated_geo_data_.arr_index++;

		//Resets the array index to go to he beginning of circular array.
		if(calculated_geo_data_.arr_index > arr_size-1)
		{
			calculated_geo_data_.arr_index = 0;
			//printf("\nNew arr_index = %i\n\n",calculated_geo_data_.arr_index);
		}
	}
	else{
			printf("GPS Data not valid. Aborted deviation filter task.\n");
		}
}


void Geo_CalculationEngine::linear_regression_computer(calculated_geo_data &calculated_geo_data_){

if(calculated_geo_data_.counter >= arr_size)
	{
	int i = 0;

	double sum_lat = 0;
	for(i = 0; i< arr_size; i++)
		sum_lat += calculated_geo_data_.geo_circularArr[lat_Arr][i]; //Summation of latitude E(lat)

	double sum_long = 0;
	for(i = 0; i< arr_size; i++)
		sum_long += calculated_geo_data_.geo_circularArr[long_Arr][i]; //Summation of longitude E(long)

	double sum_time = 0;
	for(i = 0; i< arr_size; i++)
		sum_time += calculated_geo_data_.geo_circularArr[time_Arr][i]; //Summation of the time E(time)

	double sum_time_sqr = 0;
		sum_time_sqr = pow(sum_time,2);	//Summation of time Squared E(time)^2

	double sqr_sum_time = 0;
	for(i=0; i<arr_size; i++)
		sqr_sum_time += pow(fabs(calculated_geo_data_.geo_circularArr[time_Arr][i]),2); //Summation of each time squared E(time^2)

	double sum_time_x_lat = 0;
	for(i=0; i<arr_size; i++)
		sum_time_x_lat += calculated_geo_data_.geo_circularArr[time_Arr][i] * calculated_geo_data_.geo_circularArr[lat_Arr][i]; //Summation of time x lat E(time*lat)

	double sum_time_x_long = 0;
	for(i=0; i<arr_size; i++)
		sum_time_x_long += calculated_geo_data_.geo_circularArr[time_Arr][i] * calculated_geo_data_.geo_circularArr[long_Arr][i]; //Summation of time x long E(time*long)

	/*
	 * Solving linear equations for latitude y=ax+b
	 * a = (E(time)*E(lat) - n*E(time*lat)) / (E(time)^2 - n*E(time^2)) , n = arr_size
	 * b = (E(time)*E(time*lat) - E(lat)*E(time^2)) / E(time)^2 - n*E(time^2)) , n = arr_size
	 */
	double a_lat = 0;
	double b_lat = 0;

	a_lat = (sum_time*sum_lat - arr_size*sum_time_x_lat) / (sum_time_sqr - arr_size*sqr_sum_time);
	b_lat = (sum_time*sum_time_x_lat - sum_lat*sqr_sum_time) / (sum_time_sqr - arr_size*sqr_sum_time);

//	printf("a_lat = %f\n", a_lat);
//	printf("b_lat = %f\n", b_lat);

	/*
	 * Solving linear equations for longitude y=ax+b
	 * a = (E(time)*E(long) - n*E(time*long)) / (E(time)^2 - n*E(time^2)) , n = arr_size
	 * b = (E(time)*E(time*long) - E(long)*E(time^2)) / E(time)^2 - n*E(time^2)) , n = arr_size
	 */
	double a_long = 0;
	double b_long = 0;

	a_long = (sum_time*sum_long - arr_size*sum_time_x_long) / (sum_time_sqr - arr_size*sqr_sum_time);
	b_long = (sum_time*sum_time_x_long - sum_long*sqr_sum_time) / (sum_time_sqr - arr_size*sqr_sum_time);

//	printf("a_long = %f\n", a_long);
//	printf("b_long = %f\n", b_long);

	/*
	 * Calculates the estimate GPS coordinates for lattitude and longitude
	 */

	//Estimated latitude
	calculated_geo_data_.c_latitude = a_lat*(calculated_geo_data_.counter) + b_lat;

	//Estimated longitude
	calculated_geo_data_.c_longitude = a_long*(calculated_geo_data_.counter) + b_long;
	}
	else
	{
		printf("Not enough valid GPS data points to compute calculated coordinates.\n");
		calculated_geo_data_.c_latitude = 0.0;
		calculated_geo_data_.c_longitude = 0.0;
	}

}

double Geo_CalculationEngine::distanceToTargetLocation(geo_data geo_data_, master_data master_data_){

		struct Coordinates{
			double x;
			double y;
		} current_A, destination_B, vector_R;

		if (geo_data_.GPS_message_status == valid)
		{
		double distance = 0;
		double magnitude = 0;

		current_A.x = geo_data_.latitude;
		current_A.y = geo_data_.longitude;
		destination_B.x = master_data_.m_latitude;
		destination_B.y = master_data_.m_longitude;

		vector_R.x = current_A.x - destination_B.x;
		vector_R.y = current_A.y - destination_B.y;

		magnitude = sqrt(pow(vector_R.x,2)+pow(vector_R.y,2));
		//printf("Magnitude = %f\n", magnitude);

		distance = (magnitude/decimalDegrees)*meterPerDecimalDegree(geo_data_.latitude);

		/*---Updates 8-segment display to show current distance to checkpoint (in meters)---*/
		if((int)distance >= 99)
			LD.setNumber(99);
		else
			LD.setNumber((int)distance);

		return distance;

		}
		else
			{
			LD.setLeftDigit('F');
			LD.setRightDigit('F');
			return 5555.5555;
			}
}

bool Geo_CalculationEngine::distanceToTargetLocation_wCalculatedGeoData(calculated_geo_data &calculated_geo_data_, master_data master_data_){

	struct Coordinates{
		double x;
		double y;
	} current_A, destination_B, vector_R;

	if(calculated_geo_data_.counter >= arr_size)
	{
	double distance = 0;
	double magnitude = 0;

	current_A.x = calculated_geo_data_.c_latitude;
	current_A.y = calculated_geo_data_.c_longitude;
	destination_B.x = master_data_.m_latitude;
	destination_B.y = master_data_.m_longitude;

	vector_R.x = current_A.x - destination_B.x;
	vector_R.y = current_A.y - destination_B.y;

	magnitude = sqrt(pow(vector_R.x,2)+pow(vector_R.y,2));
	//printf("Magnitude = %f\n", magnitude);

	distance = (magnitude/decimalDegrees)*meterPerDecimalDegree(calculated_geo_data_.c_latitude);

	/*---Updates 8-segment display to show current distance to checkpoint (in meters)---*/
	if((int)distance >= 99)
		LD.setNumber(99);
	else
		LD.setNumber((int)distance);

	calculated_geo_data_.c_distanceToLocation = distance;

	return true;
	}
	else
		{
		LD.setLeftDigit('F');
		LD.setRightDigit('F');
		calculated_geo_data_.c_distanceToLocation = 55.55;

		return false;
		}

}

double Geo_CalculationEngine::angleOfError(geo_data geo_data_, master_data master_data_){

		struct Coordinates{
			double x;
			double y;
		} current_A, projection_B, destination_C, vector_P, vector_R;

		if(geo_data_.GPS_message_status == valid)
		{
		double angle = 0;
		double angle_abs_difference = 0;
		double angle_final = 0;
		double compass_new = 0;
		double productVectors = 0 ;
		double magnitude_R = 0;
		double magnitude_P = 0;

		/*---Reassigning input to better visual them as coordinates---*/
		current_A.x = geo_data_.longitude;
		current_A.y = geo_data_.latitude;
		destination_C.x = master_data_.m_longitude;
		destination_C.y = master_data_.m_latitude;
		projection_B.x = geo_data_.longitude;
		projection_B.y = geo_data_.latitude + 0.1;

		/*---Vector AB-> (vector of car position to its destination)  is equal to A-C ---*/
		vector_R.x = projection_B.x - current_A.x;
		vector_R.y = projection_B.y - current_A.y;

		/*---Vector AC-> (vector of car position to north) is equal to B-C ---*/
		vector_P.x = destination_C.x - current_A.x;
		vector_P.y = destination_C.y - current_A.y;

		/*---Product of both Vectors R & P (CA * CB)---*/
		productVectors = (vector_R.x * vector_P.x) + (vector_R.y * vector_P.y);
		//printf("Product of Vectors = %f\n",productVectors);

		/*---Magnitude of vector R (||CA||)---*/
		magnitude_R = sqrt(pow(vector_R.x,2) + pow(vector_R.y,2));
		//printf("Magnitude_R = %f\n", magnitude_R);

		/*---Magnitude of vector P (||CB||)---*/
		magnitude_P = sqrt(pow(vector_P.x,2) + pow(vector_P.y,2));
		this->distance_Magnitude = magnitude_P;
		//printf("Magnitude_p = %f\n\n", magnitude_P);

		/*---Computes Arc-Cosine and converts results to degrees. Result is angle between north vector and next destination vector---*/
		angle = acos(productVectors/(magnitude_R*magnitude_P)) * (180.0 / M_PI);

		/*---Next 20 lines of code determines if next destination is East or West then determines to turn left or right---*/
		/*---Checks if destination_longitude is less than current_longitude and change sign of angle to mark if destination is on East(+) or West(-) side.---*/
		if(destination_C.x < current_A.x)
			angle = angle*(-1);
		this->angleError_ToNorth = angle;
	//	printf("Angle with respect to North = %f\n", angle);
		/*---Perform an offset to compass heading once it reads over 180 degree. This is to help next calculation.---*/
		if(geo_data_.compass_heading > 180)
			compass_new = (-1)*(360 - geo_data_.compass_heading);
		else
			compass_new = geo_data_.compass_heading;

		/*---Checks which side (East or West) next destination is and then calculate the angle the car needs to turn (left or right).---*/
		if(angle < 0)
		{
			angle_abs_difference = fabs(angle - compass_new);

			if(angle_abs_difference > 180)
				angle_final = 360.0 - angle_abs_difference;
			else
				angle_final = angle - compass_new;
		}
		else
		{
			angle_abs_difference = fabs(angle - compass_new);

			if(angle_abs_difference > 180)
				angle_final = angle_abs_difference - 360.0;
			else
				angle_final = angle - compass_new;
		}

		return angle_final;
		}
		else
			return 0;
}

bool Geo_CalculationEngine::angleOfError_wCalculatedGeoData(calculated_geo_data &calculated_geo_data_, master_data master_data_, geo_data geo_data_){


	struct Coordinates{
		double x;
		double y;
	} current_A, projection_B, destination_C, vector_P, vector_R;

	if(calculated_geo_data_.counter >= arr_size)
	{
	double angle = 0;
	double angle_abs_difference = 0;
	double angle_final = 0;
	double compass_new = 0;
	double productVectors = 0 ;
	double magnitude_R = 0;
	double magnitude_P = 0;

	/*---Reassigning input to better visual them as coordinates---*/
	current_A.x = calculated_geo_data_.c_longitude;
	current_A.y = calculated_geo_data_.c_latitude;
	destination_C.x = master_data_.m_longitude;
	destination_C.y = master_data_.m_latitude;
	projection_B.x = calculated_geo_data_.c_longitude;
	projection_B.y = calculated_geo_data_.c_latitude + 0.1;

	/*---Vector AB-> (vector of car position to its destination)  is equal to A-C ---*/
	vector_R.x = projection_B.x - current_A.x;
	vector_R.y = projection_B.y - current_A.y;

	/*---Vector AC-> (vector of car position to north) is equal to B-C ---*/
	vector_P.x = destination_C.x - current_A.x;
	vector_P.y = destination_C.y - current_A.y;

	/*---Product of both Vectors R & P (CA * CB)---*/
	productVectors = (vector_R.x * vector_P.x) + (vector_R.y * vector_P.y);
	//printf("Product of Vectors = %f\n",productVectors);

	/*---Magnitude of vector R (||CA||)---*/
	magnitude_R = sqrt(pow(vector_R.x,2) + pow(vector_R.y,2));
	//printf("Magnitude_R = %f\n", magnitude_R);

	/*---Magnitude of vector P (||CB||)---*/
	magnitude_P = sqrt(pow(vector_P.x,2) + pow(vector_P.y,2));
	this->distance_Magnitude = magnitude_P;
	//printf("Magnitude_p = %f\n\n", magnitude_P);

	/*---Computes Arc-Cosine and converts results to degrees. Result is angle between north vector and next destination vector---*/
	angle = acos(productVectors/(magnitude_R*magnitude_P)) * (180.0 / M_PI);

	/*---Next 20 lines of code determines if next destination is East or West then determines to turn left or right---*/
	/*---Checks if destination_longitude is less than current_longitude and change sign of angle to mark if destination is on East(+) or West(-) side.---*/
	if(destination_C.x < current_A.x)
		angle = angle*(-1);
	this->angleError_ToNorth = angle;
//	printf("Angle with respect to North = %f\n", angle);
	/*---Perform an offset to compass heading once it reads over 180 degree. This is to help next calculation.---*/
	if(geo_data_.compass_heading > 180)
		compass_new = (-1)*(360 - geo_data_.compass_heading);
	else
		compass_new = geo_data_.compass_heading;

	/*---Checks which side (East or West) next destination is and then calculate the angle the car needs to turn (left or right).---*/
	if(angle < 0)
	{
		angle_abs_difference = fabs(angle - compass_new);

		if(angle_abs_difference > 180)
			angle_final = 360.0 - angle_abs_difference;
		else
			angle_final = angle - compass_new;
	}
	else
	{
		angle_abs_difference = fabs(angle - compass_new);

		if(angle_abs_difference > 180)
			angle_final = angle_abs_difference - 360.0;
		else
			angle_final = angle - compass_new;
	}

	calculated_geo_data_.c_errorAngle = angle_final;
	return true;
	}
	else
		calculated_geo_data_.c_errorAngle = 0.0;
		return false;

}
