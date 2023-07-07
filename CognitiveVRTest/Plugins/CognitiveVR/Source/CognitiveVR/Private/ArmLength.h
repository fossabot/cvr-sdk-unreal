#pragma once

#include "CognitiveVR/Private/util/util.h"
#include "CognitiveVR/Public/CognitiveVR.h"
#include "ArmLength.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UArmLength : public UActorComponent
{
	GENERATED_BODY()

	public:
		UArmLength();

	private:
		virtual void BeginPlay() override;

		UFUNCTION()
		void OnSessionBegin();
		UFUNCTION()
			void OnSessionEnd();
		FTimerHandle IntervalHandle;
		void EndInterval();
		float IntervalDuration = 1;
		int32 CurrentSampleCount = 0; 
		int32 NumberOfSamples = 60;
		int32 IntermediateSampleCount = 10;

		float EyeToShoulderHeight = 18.6f;
		float ArmLength = 0;
		virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};