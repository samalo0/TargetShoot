#include "Core/TargetShootGameModeBase.h"

#include "Weapon/HitDecalActor.h"
#include "Core/PoolDataAsset.h"
#include "Core/PoolInterface.h"
#include "Engine/World.h"

ATargetShootGameModeBase::ATargetShootGameModeBase()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
}

void ATargetShootGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    SpawnActorsForPool();

    HitDecalActors.Empty(MaximumNumberOfDecals);
}

#pragma region Decal

void ATargetShootGameModeBase::RegisterDecal(AHitDecalActor* Decal)
{
    FScopeLock Lock(&HitDecalActorsCriticalSection);
    
    if(IsValid(Decal))
    {
        HitDecalActors.Add(Decal);
    }

    if(HitDecalActors.Num() > MaximumNumberOfDecals)
    {
        AHitDecalActor* OverflowActor = HitDecalActors[0];
        bool bPooled = false;
        if(IsValid(OverflowActor))
        {
            bPooled = OverflowActor->GetIsPooling();
            OverflowActor->PoolInterface_DestroyOrReturnToPool();
        }

        if(!bPooled)
        {
            // If not pooled, remove the first element. The pool will unregister the decal otherwise.
            HitDecalActors.RemoveAt(0, 1, false);
        }
    }
}

void ATargetShootGameModeBase::UnregisterDecal(AHitDecalActor* Decal)
{
    FScopeLock Lock(&HitDecalActorsCriticalSection);
    
    if(IsValid(Decal))
    {
        HitDecalActors.Remove(Decal);
    }
}

#pragma endregion

#pragma region Pool

AActor* ATargetShootGameModeBase::PullActorFromPool(UClass* Class, const FTransform Transform)
{
    UWorld* World = GetWorld();
    if(!IsValid(World))
    {
        UE_LOG(LogTemp, Error, TEXT("%s invalid world."), *GetNameSafe(this));
        return nullptr;
    }
    
    int32 PoolIndex = -1;
    for(int32 Index = 0; Index < Pool.Num(); Index++)
    {
        if(Pool[Index].ActorClass == Class)
        {
            PoolIndex = Index;
            break;
        }
    }

    if(PoolIndex == -1)
    {
        // Actor doesn't support the pool, just spawn it.
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.Owner = this;
        SpawnParameters.ObjectFlags |= RF_Transient;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        return World->SpawnActor<AActor>(Class, Transform, SpawnParameters);
    }

    FPooledActor& PooledActor = Pool[PoolIndex];
    
    if(PooledActor.Inactive.Num() > 0)
    {
        AActor* Actor = PooledActor.Inactive[0];
        IPoolInterface* PoolInterface = Cast<IPoolInterface>(Actor);
        PooledActor.Inactive.RemoveAt(0, 1, false);
        if(PoolInterface != nullptr)
        {
            #if WITH_EDITOR
                Actor->SetFolderPath(PooledActor.ActiveFolderName);
            #endif
            PooledActor.Active.Add(Actor);
            PoolInterface->PoolInterface_OnPullFromPool(Transform);
            return Actor;
        }
    }
    else if(PooledActor.bSpawnMoreWhenDepleted)
    {
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.Owner = this;
        SpawnParameters.ObjectFlags |= RF_Transient;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AActor* Actor = World->SpawnActor<AActor>(PooledActor.ActorClass, SpawnParameters);
        IPoolInterface* PoolInterface = Cast<IPoolInterface>(Actor);    
        if(IsValid(Actor) && PoolInterface != nullptr)
        {
            #if WITH_EDITOR
                Actor->SetFolderPath(PooledActor.ActiveFolderName);
            #endif
            PoolInterface->PoolInterface_Initialize();
            PooledActor.Active.Add(Actor);
            return Actor;
        }
    }

    return nullptr;
}

void ATargetShootGameModeBase::ReturnActorToPool(AActor* Actor)
{
    IPoolInterface* PoolInterface = Cast<IPoolInterface>(Actor);
    if(PoolInterface == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s can't return %s to the pool, it doesn't support the interface."), *GetNameSafe(this), *GetNameSafe(Actor));
        return;
    }

    for(FPooledActor& PooledActor : Pool)
    {
        if(PooledActor.ActorClass == Actor->GetClass())
        {
            #if WITH_EDITOR
                Actor->SetFolderPath(PooledActor.InactiveFolderName);
            #endif
            PooledActor.Active.Remove(Actor);
            PoolInterface->PoolInterface_OnReturnToPool();
            PooledActor.Inactive.Add(Actor);

            return;
        }
    }
}

void ATargetShootGameModeBase::SpawnActorsForPool()
{
    UWorld* World = GetWorld();    
    if(!IsValid(PoolDataAsset) || !IsValid(World))
    {
        return;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = this;
    SpawnParameters.ObjectFlags |= RF_Transient;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    for(const FPoolData Data : PoolDataAsset->PooledActors)
    {
        if(!Data.bEnabled)
        {
            continue;
        }

        FPooledActor PooledActor;
        PooledActor.ActorClass = Data.Class;
        PooledActor.bSpawnMoreWhenDepleted = Data.bSpawnMoreWhenDepleted;

        #if WITH_EDITORONLY_DATA
            PooledActor.ActiveFolderName = *FString::Printf(TEXT("Pool/%s/Active"), *PooledActor.ActorClass->GetName());
            PooledActor.InactiveFolderName = *FString::Printf(TEXT("Pool/%s/Inactive"), *PooledActor.ActorClass->GetName());;
        #endif

        for(int32 Count = 0; Count < Data.Count; Count++)
        {
            AActor* Actor = World->SpawnActor<AActor>(PooledActor.ActorClass, SpawnParameters);
            IPoolInterface* PoolInterface = Cast<IPoolInterface>(Actor);    
            if(IsValid(Actor) && PoolInterface != nullptr)
            {
                #if WITH_EDITOR
                    Actor->SetFolderPath(PooledActor.InactiveFolderName);
                #endif
                PoolInterface->PoolInterface_Initialize();
                PoolInterface->PoolInterface_OnReturnToPool();                
                PooledActor.Inactive.Add(Actor);
            }
        }

        Pool.Add(PooledActor);
    }
}

#pragma endregion

#pragma region Score

void ATargetShootGameModeBase::AddScore(const int32 Score)
{
    TotalScore += Score;
    TotalShotsFired++;

    OnScoreUpdate.Broadcast(TotalScore, TotalShotsFired);
}

#pragma endregion