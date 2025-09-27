// Copyright (c) 2025 Sawnoff Games. All rights reserved.


#include "Input/CharacterPlayerController.h"

#include "Character/MainCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void ACharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	if (ACharacter* ControlledCharacter = GetCharacter())
	{
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ACharacterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this,	&ACharacterPlayerController::StopMove);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacterPlayerController::StopJumping);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACharacterPlayerController::StopSprint);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ACharacterPlayerController::Attack);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACharacterPlayerController::ToggleAim);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::ToggleCrouch);

		EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::CameraZoom);

		EnhancedInputComponent->BindAction(EnterVehicleAction, ETriggerEvent::Triggered, this, &ACharacterPlayerController::EnterVehicle);
	}
}

void ACharacterPlayerController::Move(const FInputActionValue& Value)
{
	if (GetCharacter())
	{
		if (ACharacter* ControlledCharacter = GetCharacter())
		{
			ControlledCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

			const FRotator Rotation = GetControlRotation();
			const FRotator CameraRotation(0, Rotation.Yaw, 0);

			FVector2D MovementVector = Value.Get<FVector2D>();

			const FVector ForwardDirection = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X);
			ControlledCharacter->AddMovementInput(ForwardDirection, MovementVector.Y);

			const FVector RightDirection = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
			ControlledCharacter->AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void ACharacterPlayerController::StopMove()
{
	if (ACharacter* ControlledCharacter = CastChecked<ACharacter>(GetCharacter()))
	{
		ControlledCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void ACharacterPlayerController::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ACharacterPlayerController::Jump()
{
	if (ACharacter* ControlledCharacter = GetCharacter())
	{
		ControlledCharacter->Jump();
	}
}

void ACharacterPlayerController::StopJumping()
{
	if (ACharacter* ControlledCharacter = GetCharacter())
	{
		ControlledCharacter->StopJumping();
	}
}

void ACharacterPlayerController::StartSprint()
{
	if (ACharacter* ControlledCharacter = GetCharacter())
	{
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

void ACharacterPlayerController::StopSprint()
{
	if (ACharacter* ControlledCharacter = GetCharacter())
	{
		ControlledCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ACharacterPlayerController::Attack()
{
	if (AMainCharacter* ControlledCharacter = CastChecked<AMainCharacter>(GetCharacter()))
	{
		ControlledCharacter->Attack();
	}
}

void ACharacterPlayerController::ToggleAim()
{
	if (AMainCharacter* ControlledCharacter = CastChecked<AMainCharacter>(GetCharacter()))
	{
		if (ControlledCharacter->IsAttacking())
		{
			return;
		}

		if (ControlledCharacter->bIsAiming)
		{
			ControlledCharacter->Aim(false);
		}
		else
		{
			ControlledCharacter->Aim(true);
		}
	}
}

void ACharacterPlayerController::ToggleCrouch()
{
	if (AMainCharacter* ControlledCharacter = CastChecked<AMainCharacter>(GetCharacter()))
	{
		if (ControlledCharacter->IsCrouched())
		{
			ControlledCharacter->UnCrouch();
		}
		else
		{
			ControlledCharacter->Crouch();
		}
	}
}

void ACharacterPlayerController::CameraZoom(const FInputActionValue& Value)
{
	if (AMainCharacter* ControlledCharacter = CastChecked<AMainCharacter>(GetCharacter()))
	{
		float ActionValue = Value.Get<float>();

		ControlledCharacter->ZoomCamera(ActionValue);
	}
}

void ACharacterPlayerController::EnterVehicle()
{
	if (AMainCharacter* ControlledCharacter = CastChecked<AMainCharacter>(GetCharacter()))
	{
		if (ControlledCharacter->ControllableVehicle)
		{
			ControlledCharacter->ControllableVehicle->EnterVehicle();

			ControlledCharacter->Destroy();
		}
	}
}
