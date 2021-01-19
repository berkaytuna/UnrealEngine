// Written by Berkay Tuna, October 2020

#pragma once

#include "Modules/ModuleManager.h"

class FRidingSystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
