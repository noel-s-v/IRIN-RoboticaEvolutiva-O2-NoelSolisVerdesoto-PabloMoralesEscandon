#include "irifitnessfunction.h"
#include "collisionmanager.h"

/******************************************************************************/
/******************************************************************************/

CIriFitnessFunction::CIriFitnessFunction(const char* pch_name, 
                                                                 CSimulator* pc_simulator, 
                                                                 unsigned int un_collisions_allowed_per_epuck)
    :
    CFitnessFunction(pch_name, pc_simulator)
{

	/* Check number of robots */
	m_pcSimulator = pc_simulator;
	TEpuckVector* pvecEpucks=m_pcSimulator->GetEpucks();
	
	if ( pvecEpucks->size() == 0 )
	{
		printf("No Robot, so fitness function can not be computed.\n Exiting...\n");
		fflush(stdout);
		exit(0);
	}
	else if  (pvecEpucks->size()>1)
	{
		printf("More than 1 robot, and fitness is not prepared for it.\n Exiting...\n");
	}
    
	m_pcEpuck=(*pvecEpucks)[0];

	m_unNumberOfSteps = 0;
	m_fComputedFitness = 0.0;
	m_unCollisionsNumber 	= 0;
	m_unNumberOfLaps = 0;
	m_currentColor = 0;
	m_fTimesOrientedToRed = 0;

}

/******************************************************************************/
/******************************************************************************/

CIriFitnessFunction::~CIriFitnessFunction(){
}
/******************************************************************************/
/******************************************************************************/

double CIriFitnessFunction::GetFitness()
{    

	/* If you need to check the collisions of the robot, here are the total number of 
	 * collisions done by the robot in the simulations */
	int coll = (CCollisionManager::GetInstance()->GetTotalNumberOfCollisions());

	/* Get the fitness divided by the number of steps */
	double fit = ( m_fComputedFitness / (double) m_unNumberOfSteps ) * (1 - ((double) (fmin(coll,10.0)/10.0))) * (1 - ((double) (fmin(m_fTimesOrientedToRed, 30.0)/30.0))) * ((double) (fmax(m_unNumberOfLaps,4.0)/4.0));

	/* If fitness less than 0, put it to 0 */
	if ( fit < 0.0 ) fit = 0.0;

	return fit;
}

/******************************************************************************/
/******************************************************************************/
void CIriFitnessFunction::SimulationStep(unsigned int n_simulation_step, double f_time, double f_step_interval)
{
	/* See Evolutionary Robotics Book */
	/* This is the function to be implemented */
	/* f = V * ( 1 - sqrt(Delta(v)) ) * (1 - i)
	 * V relates to the maximum speed
	 * Delta(v) relates to the movement on the same direction
	 * i relates to the maximum sensor value
	 */

	/* Get actual SPEED of the left and right wheel */
	double leftSpeed = 0.0;
	double rightSpeed = 0.0;
	m_pcEpuck->GetWheelSpeed(&leftSpeed,&rightSpeed);
	leftSpeed = 0.5 + ( leftSpeed / ( 2.0 *  m_pcEpuck->GetMaxWheelSpeed()) );
	rightSpeed = 0.5 + ( rightSpeed / ( 2.0 *  m_pcEpuck->GetMaxWheelSpeed()) );

	/* Eval maximum speed partial fitness */
	double maxSpeedEval = (fabs(leftSpeed - 0.5) + fabs(rightSpeed - 0.5));

	/* Eval same direction partial fitness */
	double sameDirectionEval = 1 - sqrt(fabs(leftSpeed - rightSpeed));
	
	/* Eval SENSORS */

	/* Where the Max PROXIMITY sensor will be stored*/
	double maxProxSensorEval 		= 0.0;
	/* Where the Max LIGHT sensor will be stored*/
	double maxLightSensorEval 	= 0.0;
	/* Where the Max BLUE LIGHT sensor will be stored*/
	double maxBlueLightSensorEval 	= 0.0;
	/* Where the Max RED LIGHT sensor will be stored*/
	double maxRedLightSensorEval 	= 0.0;
	/* Where the Max CONTACT sensor will be stored*/
	double maxContactSensorEval = 0.0;

	/* Where the GROUND MEMORY will be stored */
	double* groundMemory;
	/* Where the GROUND will be stored */
	double* ground;
	/* whre the BATTERY will be sotored */
	double *battery;
	/* whre the BLUE BATTERY will be sotored */
	double *blueBattery;
	/* whre the RED BATTERY will be sotored */
	double *redBattery;

	double blueLightS0=0;
	double blueLightS7=0;
	double redLightS0=0;
	double redLightS1=0;
	double redLightS2=0;
	double redLightS3=0;
	double redLightS4=0;
	double redLightS5=0;
	double redLightS6=0;
	double redLightS7=0;
	double lightS0=0;
	double lightS1=0;
	double lightS2=0;
	double lightS3=0;
	double lightS4=0;
	double lightS5=0;
	double lightS6=0;
	double lightS7=0;

	/* Auxiluar variables */
	unsigned int unThisSensorsNumberOfInputs; 
	double* pfThisSensorInputs; 
	
	/* Go in all the sensors */
	TSensorVector vecSensors = m_pcEpuck->GetSensors();
	for (TSensorIterator i = vecSensors.begin(); i != vecSensors.end(); i++)
	{
		/* Check type of sensor */
		switch ( (*i)->GetType() )
		{
			/* If sensor is PROXIMITY */
			case SENSOR_PROXIMITY:
				/* Get the number of inputs */
				unThisSensorsNumberOfInputs = (*i)->GetNumberOfInputs();
				/* Get the actual values */
				pfThisSensorInputs = (*i)->GetComputedSensorReadings();

				/* For every input */
				for (int j = 0; j < unThisSensorsNumberOfInputs; j++)
				{
					/* If reading bigger than maximum */
					if ( pfThisSensorInputs[j] > maxProxSensorEval )
					{	
						/* Store maximum value */
						maxProxSensorEval = pfThisSensorInputs[j];
					}
				}
				break;

			/* If sensor is GROUND_MEMORY */
			case SENSOR_GROUND_MEMORY:
				/* Get the actual value */
				groundMemory = (*i)->GetComputedSensorReadings();
				break;
	
			/* If sensor is GROUND */
			case SENSOR_GROUND:
				/* Get actual values */
				ground = (*i)->GetComputedSensorReadings();
				break;	
			/* If sensor is LIGHT */
			case SENSOR_REAL_LIGHT:
				/* Get number of inputs */
				unThisSensorsNumberOfInputs = (*i)->GetNumberOfInputs();
				/* Get the actual values */
				pfThisSensorInputs = (*i)->GetComputedSensorReadings();

				/* For every input */
				for (int j = 0; j < unThisSensorsNumberOfInputs; j++)
				{
					/* If reading bigger than maximum */
					if ( pfThisSensorInputs[j] > maxLightSensorEval )
					{	
						/* Store maximum value */
						maxLightSensorEval = pfThisSensorInputs[j];
					}
					if (j==0)
						lightS0 = pfThisSensorInputs[j];
					else if (j==1)
						lightS1 = pfThisSensorInputs[j];
					else if (j==2)
						lightS2 = pfThisSensorInputs[j];
					else if (j==3)
						lightS3 = pfThisSensorInputs[j];
					else if (j==4)
						lightS4 = pfThisSensorInputs[j];
					else if (j==5)
						lightS5 = pfThisSensorInputs[j];
					else if (j==6)
						lightS6 = pfThisSensorInputs[j];
					else if (j==7)
						lightS7 = pfThisSensorInputs[j];
				}
				break;
			case SENSOR_REAL_BLUE_LIGHT:
				unThisSensorsNumberOfInputs = (*i)->GetNumberOfInputs();
				pfThisSensorInputs = (*i)->GetComputedSensorReadings();

				for (int j = 0; j < unThisSensorsNumberOfInputs; j++)
				{
					if ( pfThisSensorInputs[j] > maxBlueLightSensorEval )
					{	
						maxBlueLightSensorEval = pfThisSensorInputs[j];
					}
					if (j==0)
						blueLightS0 = pfThisSensorInputs[j];
					else if (j==7)
						blueLightS7 = pfThisSensorInputs[j];
			
				}
				break;
			
			case SENSOR_REAL_RED_LIGHT:
				unThisSensorsNumberOfInputs = (*i)->GetNumberOfInputs();
				pfThisSensorInputs = (*i)->GetComputedSensorReadings();

				for (int j = 0; j < unThisSensorsNumberOfInputs; j++)
				{
					if ( pfThisSensorInputs[j] > maxRedLightSensorEval )
					{	
						maxRedLightSensorEval = pfThisSensorInputs[j];
					}
					if (j==0)
						redLightS0 = pfThisSensorInputs[j];
					else if (j==1)
						redLightS1 = pfThisSensorInputs[j];
					else if (j==2)
						redLightS2 = pfThisSensorInputs[j];
					else if (j==3)
						redLightS3 = pfThisSensorInputs[j];
					else if (j==4)
						redLightS4 = pfThisSensorInputs[j];
					else if (j==5)
						redLightS5 = pfThisSensorInputs[j];
					else if (j==6)
						redLightS6 = pfThisSensorInputs[j];
					else if (j==7)
						redLightS7 = pfThisSensorInputs[j];
			
				}
				break;

			/* If sensor is BATTERY */
			case SENSOR_BATTERY:
         		battery = (*i)->GetComputedSensorReadings();
				 break;
			
			case SENSOR_BLUE_BATTERY:
				blueBattery = (*i)->GetComputedSensorReadings();
				break;
			
			case SENSOR_RED_BATTERY:
				redBattery = (*i)->GetComputedSensorReadings();
				break;
			
			/* If sensor is CONTACT */
			case SENSOR_CONTACT:
				/* Get number of inputs */
				unThisSensorsNumberOfInputs = (*i)->GetNumberOfInputs();
				/* Get actual values */
				pfThisSensorInputs = (*i)->GetComputedSensorReadings();

				/* For every input */
				for (int j = 0; j < unThisSensorsNumberOfInputs; j++)
				{
					/* If reading bigger than maximum */
					if ( pfThisSensorInputs[j] > maxContactSensorEval )
					{
						/* Store maximum value */
						maxContactSensorEval = pfThisSensorInputs[j];
					}
				}
				break;
		}
	}
	
	/* FROM HERE YOU NEED TO CREATE YOU FITNESS */	


	double fitness = 1.0;

	/* Eval maximum speed partial fitness */
    maxSpeedEval = (fabs(leftSpeed - 0.5) + fabs(rightSpeed - 0.5));
    /*Eval correct orientation*/
    double light = 0.7*lightS2 + 0.1*lightS3 + 0.2*lightS1;

	double redLight = 0.4*redLightS0 + 0.4*redLightS7 ;

	double Rbattery = redBattery[0];

	if(Rbattery < 0.3){
		redLight = fmax(redLight,0.1);

	} else{
		if(redLight > 0.3){
			m_fTimesOrientedToRed++;
		}
		// redLight = 1 - (0.8*redLight);
		Rbattery = 1.0;
	}

	// Podriamos poner un flag y que si se queda sin batería lo penalizamos o mantener este método que haría que cuando está sin batería no sume fitness

	/* Eval same direction partial fitness */
	// double sameDirectionEval = 1 - sqrt(fabs(leftSpeed - rightSpeed));
	
    fitness =  light * maxSpeedEval * sameDirectionEval * (leftSpeed * rightSpeed) * Rbattery; //* redLight;
	
	
	/* TO HERE YOU NEED TO CREATE YOU FITNESS */	

	m_unNumberOfSteps++;
	m_fComputedFitness += fitness;

	if(redLight > 0.3)
		m_fTimesOrientedToRed++;


	/* Get Collisions */
	if ( maxContactSensorEval == 1 )
		m_unCollisionsNumber++;		


	double color = ground[0];
	if(m_currentColor){
		if (color == 0.5){
			m_unNumberOfLaps+=0.5;
			m_currentColor = false;
		}
	} else{
		if(color == 0){
			m_unNumberOfLaps+=0.5;
			m_currentColor = true;
		}
	}
}

/******************************************************************************/
/******************************************************************************/