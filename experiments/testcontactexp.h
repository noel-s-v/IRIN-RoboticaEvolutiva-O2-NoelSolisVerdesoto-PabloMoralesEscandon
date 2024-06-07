#ifndef TESTCONTACTEXP_H
#define TESTCONTACTEXP_H

/******************************************************************************/
/******************************************************************************/

#include "experiment.h"

/******************************************************************************/
/******************************************************************************/

class CTestContactExp : public CExperiment
{
public:
    CTestContactExp(const char* pch_name, const char* paramsFile);

protected:
    // Overwritten from the superclasses:
    CArena* CreateArena();
		void AddActuators(CEpuck* pc_epuck);
    void AddSensors(CEpuck* pc_epuck);
    void SetController(CEpuck* pc_epuck);
    void CreateAndAddEpucks(CSimulator* pc_simulator);
    
private:
    int m_nRobotsNumber;
};

/******************************************************************************/
/******************************************************************************/

#endif
