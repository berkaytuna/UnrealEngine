// Written by Berkay Tuna, August 2020

#pragma once

#include "Modules/ModuleManager.h"

class FAnimationWarpingEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
