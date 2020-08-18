#pragma once
#include <CryEntitySystem/IEntityComponent.h>
class MyPlayerComponent : public IEntityComponent
{
public:
	MyPlayerComponent() {};
	~MyPlayerComponent() {};

	static void ReflectType(Schematyc::CTypeDesc<MyPlayerComponent>& desc) {
		desc.SetGUID("{39E9E352-FD79-4175-B4EB-432F35930A0C}"_cry_guid);
		desc.SetLabel("My Player Component");
		desc.AddMember(&MyPlayerComponent::m_Force, 'for', "Force", "Force", "Jump Force",5.0F);
		desc.SetComponentFlags({ IEntityComponent::EFlags::Singleton, IEntityComponent::EFlags::Transform });
	}
	virtual void Initialize();
	float m_Force = 10.0F;
};