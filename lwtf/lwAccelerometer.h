#ifndef __LW_ACCELEROMETER_H__
#define __LW_ACCELEROMETER_H__

namespace lw{

	void initAccelerometer(float updateInterval);
	void exitAccelerometer();
	const cml::Vector3& getAccelerometer();

} //namespace lw



#endif //__LW_ACCELEROMETER_H__