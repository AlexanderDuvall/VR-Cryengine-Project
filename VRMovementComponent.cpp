#include "StdAfx.h"
#include "VRMovementComponent.h"
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Physics/RigidBodyComponent.h>
#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Utils/EnumFlags.h>
#include <CrySchematyc/Env/IEnvRegistry.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/Elements/EnvFunction.h>
#include <CrySchematyc/Env/Elements/EnvSignal.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CryCore/StaticInstanceList.h>
#include <CrySystem/VR/IHMDDevice.h>
#include <CrySystem/VR/IHMDManager.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <iostream>
#include <math.h>
#include <CryMath/Cry_Matrix34.h>
#include <CryPhysics/physinterface.h>
//PLACE IN CORRECT SPOT ... INITIALIZE?
void PhysicalizeLiving(IEntity& entity) {
	SEntityPhysicalizeParams physParams;
	// Set the physics type to PE_LIVING
	physParams.type = PE_LIVING;
	// Set the mass to 90 kilograms
	physParams.mass = 10;
	// Living entities have to set the SEntityPhysicalizeParams::pPlayerDimensions field
	pe_player_dimensions playerDimensions;
	// Prefer usage of a cylinder instead of capsule
	playerDimensions.bUseCapsule = 0;
	// Specify the size of our cylinder
	playerDimensions.sizeCollider = Vec3(0.3f, 0.3f, 0.935f * 0.5f);
	// Keep pivot at the player's feet (defined in player geometry) 
	playerDimensions.heightPivot = 0.f;
	// Offset collider upwards
	playerDimensions.heightCollider = 1.f;
	physParams.pPlayerDimensions = &playerDimensions;
	// Living entities have to set the SEntityPhysicalizeParams::pPlayerDynamics field
	pe_player_dynamics playerDynamics;
	// Mass needs to be repeated in the pe_player_dynamics structure
	playerDynamics.mass = physParams.mass;
	physParams.pPlayerDynamics = &playerDynamics;
	// Now physicalize the entity
	entity.Physicalize(physParams);
}
void MoveLiving(IPhysicalEntity& physicalEntity, double x, double y)
{
	CryLog("asdf");
	pe_action_move moveAction;
	// Apply movement request directly to velocity
	moveAction.iJump = 1;
	moveAction.dir = Vec3((float)x, (float)y, 0);
	physicalEntity.Action(&moveAction);

	CryLog("fdas");
}
struct Point {
	double x;
	double y;

};
/*
Return radians of analog stick
Mult by 180/PI to get angles
*/
double VRtoPolar(double x, double y) {
	double radians = atan2(y, x);
	return radians;
}
/*
Transform angle to x and y coordinates
*/
Point VRtoRect(double angle) {
	Point p;
	angle *= 3.14 / 180;
	p.x = cos(angle);
	p.y = sin(angle);
	return p;
}
double VRnewAngle(double controllerAngle, double cameraAngle) {
	CryLog("Controller Angle " + ToString(controllerAngle));
	CryLog("Camera Angle " + ToString(cameraAngle));
	double offset = 90 - controllerAngle;
	CryLog("Offset " + ToString(offset));
	CryLog("New Angle " + ToString(cameraAngle - offset));

	return cameraAngle - offset;
}


static void RegisterVrMovementComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(VRMovementComponent));
		// Functions	
		{
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterVrMovementComponent)


void VRMovementComponent::Initialize() {

	Cry::DefaultComponents::CInputComponent* input = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();

	input->RegisterAction("myplayer", "Jump", [this](int activationMode, float value)
	{
		Cry::DefaultComponents::CRigidBodyComponent* rigid = m_pEntity->GetOrCreateComponent <Cry::DefaultComponents::CRigidBodyComponent>();
		rigid->ApplyImpulse(Vec3(0, 0, 10) * m_Force);
	});

	input->BindAction("myplayer", "Jump", eAID_KeyboardMouse, eKI_Mouse1);

	Cry::DefaultComponents::CRigidBodyComponent* rigid = m_pEntity->GetOrCreateComponent <Cry::DefaultComponents::CRigidBodyComponent>();
	IEntity* newEntity = rigid->GetEntity();
	PhysicalizeLiving(*newEntity);
}
void VRMovementComponent::ProcessEvent(const SEntityEvent& event) {
	if (event.event == ENTITY_EVENT_UPDATE)
	{
		if (IHmdManager* pHmdManager = gEnv->pSystem->GetHmdManager()) // Check, if the HMD Manager exists
		{
			if (IHmdDevice* pDevice = pHmdManager->GetHmdDevice()) // Check, if a valid HMD device is connected
			{
				if (pDevice->GetClass() == EHmdClass::eHmdClass_OpenVR) // Check, if the connected device is an Oculus device)
				{
					const IHmdController* pController = pDevice->GetController();
					// Make sure the desired controller is connected (the OpenVR implementation in CRYENGINE currently supports controller ID 1 and 2)
					if (pController->IsConnected(eHmdController_OpenVR_1))
					{
						//bool bGripTouched = pController->IsButtonTouched(eHmdController_OpenVR_1, eKI_Motion_OpenVR_TriggerBtn);
						Vec2 vTouchPad = pController->GetThumbStickValue(eHmdController_OpenVR_1, eKI_Motion_OpenVR_TouchPad_X);
						// Is the Application Menu button pressed?
						//bool bAppMenuPressed = pController->IsButtonPressed(eHmdController_OpenVR_1, eKI_Motion_OpenVR_ApplicationMenu);
						/** or is it touched?
						bool bAppMenuTouched = pController->IsButtonTouched(eHmdController_OpenVR_1, eKI_Motion_OpenVR_ApplicationMenu);
						// Is the Grip pressed?
						bool bGripPressed = pController->IsButtonPressed(eHmdController_OpenVR_1, eKI_Motion_OpenVR_Grip);
						// or is it touched?
						bool bGripTouched = pController->IsButtonTouched(eHmdController_OpenVR_1, eKI_Motion_OpenVR_Grip);
						// Get Trigger value
						float fTrigger = pController->GetTriggerValue(eHmdController_OpenVR_1, eKI_Motion_OpenVR_Trigger); // the provided KeyID is irrelevant!
						// Get the Touch Pad coordinates
						Vec2 vTouchPad = pController->GetThumbStickValue(eHmdController_OpenVR_1, eKI_Motion_OpenVR_TouchPad_X); // the provided KeyID is irrelevant!
						**/


						if (vTouchPad.x != 0 || vTouchPad.y != 0) {
							CryLog("True");
							double x = vTouchPad.x;
							double y = vTouchPad.y;
							double StickAngle = VRtoPolar(x, y) * 180 / 3.14;
							HmdTrackingState state = pDevice->GetLocalTrackingState();
							//Matrix34 HMDtm = Matrix34::Create(Vec3(1, 1, 1), state.pose.orientation, state.pose.position).GetColumn0;
							//3 is translation in... meters?
							Vec3 a = Matrix34::Create(Vec3(1, 1, 1), state.pose.orientation, state.pose.position).GetColumn(1);//1y
							float yawy = a.y;
							float yawx = a.x;
							float yaw = float(atan2(yawy, yawx) * 180 / 3.14 + 180.0);
							//TODO check ^^
							double Direction = VRnewAngle(StickAngle, yaw);

							Point p = VRtoRect(Direction);
							Cry::DefaultComponents::CRigidBodyComponent* rigid = m_pEntity->GetOrCreateComponent <Cry::DefaultComponents::CRigidBodyComponent>();
							IEntity* newEntity = rigid->GetEntity();

							IPhysicalEntity* phys = newEntity->GetPhysicalEntity();
							MoveLiving(*phys, p.x, p.y);
							string cx = ToString(p.x) + "," + ToString(p.y);
							CryLog(cx);
							//	CryLog(ToString(yaw));
						}
						//Cry::DefaultComponents::CRigidBodyComponent* rigid = m_pEntity->GetOrCreateComponent <Cry::DefaultComponents::CRigidBodyComponent>();
						//rigid->ApplyImpulse(Vec3(0, 0, 10) * m_Force);
					}
				}
				else {

				}
			}
			else {

			}
		}
		else {
		}
	}
	else {
	}
}

Cry::Entity::EventFlags VRMovementComponent::GetEventMask() const {
	return (ENTITY_EVENT_UPDATE);
}



