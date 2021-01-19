// Written by Berkay Tuna, June 2020

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPassiveRagdollModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
