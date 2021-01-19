// Written by Berkay Tuna, June 2020

#pragma once

#include "Modules/ModuleManager.h"

class FPassiveRagdollEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
