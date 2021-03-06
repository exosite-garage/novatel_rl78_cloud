/*-------------------------------------------------------------------------*
 * File:  LightSensor.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Light sensor driver using the ADT7420 over I2C.
 *-------------------------------------------------------------------------*/
#ifndef LIGHTSENSOR_H_
#define LIGHTSENSOR_H_

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LightSensor_Init(void);
int16_t LightSensor_Get(void);

#endif // LIGHTSENSOR_H_
/*-------------------------------------------------------------------------*
 * End of File:  LightSensor.h
 *-------------------------------------------------------------------------*/



