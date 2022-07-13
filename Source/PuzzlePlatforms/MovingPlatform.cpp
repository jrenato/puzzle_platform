// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"


AMovingPlatform::AMovingPlatform()
{
    PrimaryActorTick.bCanEverTick = true;

    SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        SetReplicates(true);
        SetReplicateMovement(true);
    }

    GlobalStartLocation = GetActorLocation();
    GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
}

void AMovingPlatform::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ActiveTriggers > 0 || AlwaysActive)
    {
        if (HasAuthority())
        {
            FVector Location = GetActorLocation();

            float DistanceToTarget = (GlobalTargetLocation - GlobalStartLocation).Size();
            float DistanceTravelled = (Location - GlobalStartLocation).Size();

            if (DistanceTravelled >= DistanceToTarget)
            {
                FVector Swap = GlobalStartLocation;
                GlobalStartLocation = GlobalTargetLocation;
                GlobalTargetLocation = Swap;
            }
            else
            {
                FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
                Location += Direction * Speed * DeltaTime;
                SetActorLocation(Location);
            }
        }   
    }
}

void AMovingPlatform::AddActiveTrigger()
{
    ActiveTriggers++;
}

void AMovingPlatform::RemoveActiveTrigger()
{
    if (ActiveTriggers > 0)
    {
        ActiveTriggers--;
    }
}
