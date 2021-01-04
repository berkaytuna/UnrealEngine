// Written by Berkay Tuna, November 2020

#pragma once

#include "Modules/ModuleManager.h"

class FInventorySystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
