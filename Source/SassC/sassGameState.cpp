// Fill out your copyright notice in the Description page of Project Settings.

#include "SassC.h"
#include "sassGameState.h"

AsassGameState::AsassGameState() {

}

void AsassGameState::Tick(float DeltaSeconds) {
	if (PreGameActive) {
		TimeKeeper += DeltaSeconds;
		if (TimeKeeper > WarmUpTime) {
			PreGameActive = false;
		}
	}
}

