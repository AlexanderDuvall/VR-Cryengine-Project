#pragma once
#include <CryEntitySystem/IEntityComponent.h>

class VRMovementComponent : public IEntityComponent
{
public:
	VRMovementComponent() {};
	~VRMovementComponent() {};

	static void ReflectType(Schematyc::CTypeDesc<VRMovementComponent>& desc) {
		desc.SetGUID("{81B0BAC7-145C-45F6-A04D-AE12D2A7E619}"_cry_guid);
		desc.SetLabel("VRMovementComponent");
		desc.AddMember(&VRMovementComponent::m_Force, 'for', "Force", "Force", "Jump Force", 5.0F);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::Transform });
	}
	virtual void Initialize() override;

	virtual void ProcessEvent(const SEntityEvent& event) override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
 
	float m_Force = 10.0F;
};

