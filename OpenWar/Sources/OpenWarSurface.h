/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef OpenWarSurface_H
#define OpenWarSurface_H

#include "Surface.h"

class BattleModel;
class BattleView;
class SimulationRules;
class SimulationState;
class terrain;


class OpenWarSurface : public Surface
{
	SimulationState* _simulationState;
	SimulationRules* _simulationRules;
	BattleModel* _battleModel;
	BattleView* _battleView;
	terrain* _terrain;

public:
	OpenWarSurface(glm::vec2 size, float pixelDensity);
	virtual ~OpenWarSurface();
    
	virtual void ScreenSizeChanged();
    
	virtual void Update(double secondsSinceLastUpdate);
    
	virtual void Render();
    
};


#endif
