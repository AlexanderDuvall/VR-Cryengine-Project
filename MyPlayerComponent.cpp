#include "StdAfx.h"
#include "MyPlayerComponent.h"
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


static void RegisterMyPlayerComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(MyPlayerComponent));
		// Functions	
		{
		}
	}
}
CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterMyPlayerComponent)

void MyPlayerComponent::Initialize() {
	//Create Input component from entity
	CryLog("we made it thsi far ");
	Cry::DefaultComponents::CInputComponent* input = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();

	input->RegisterAction("myplayer", "Jump", [this](int activationMode, float value) 
	{

		Cry::DefaultComponents::CRigidBodyComponent* rigid = m_pEntity->GetOrCreateComponent <Cry::DefaultComponents::CRigidBodyComponent>();

		rigid->ApplyImpulse(Vec3(0, 0, 10) * m_Force);
	});

	input->BindAction("myplayer", "Jump", eAID_KeyboardMouse, eKI_Mouse1);
}
