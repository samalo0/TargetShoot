// Copyright Stephen Maloney, 2020

#pragma once

#include "CoreMinimal.h"

#include "TargetShootGameModeBase.h"
#include "UObject/Interface.h"
#include "PoolInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPoolInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors which can be pooled.
 */
class TARGETSHOOT_API IPoolInterface
{
	GENERATED_BODY()

public:

	FORCEINLINE bool GetIsPooling() const { return bIsPooling; }
	
	virtual void PoolInterface_DestroyOrReturnToPool() = 0;

	virtual void PoolInterface_Initialize() = 0;
	
	virtual void PoolInterface_OnPullFromPool(FTransform Transform) = 0;
	
	virtual void PoolInterface_OnReturnToPool() = 0;

protected:
	
	bool bIsPooling = false;
	
};
