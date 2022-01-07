#include "variant_const.h"
#include "instruction.h"

namespace  IFTYPE{
  const char* ActionTable="ActionTable";
  const char* CharacterParameterVersion="CharacterParameterVersion";
  const char* Detector="Detector";
  const char* FailureConfig="FailureConfig";
  const char* Failure="Failure";
  const char* FollowPhaseTable="FollowPhaseTable";
  const char* IdentificationCode="IdentificationCode";
  const char* LightStatus="LightStatus";
  const char* PedestrianDetector="PedestrianDetector";
  const char* Phase="Phase";
  const char* PhaseSequenceTable="PhaseSequenceTable";
  const char* PriorityConfig="PriorityConfig";
  const char* RemoteControl="RemoteControl";
  const char* Schedule="Schedule";
  const char* SchemaTable="SchemaTable";
  const char* SemaphoreGroup="SemaphoreGroup";
  const char* Time="Time";
  const char* TimeTable="TimeTable";
  const char* TimingScheme="TimingScheme";
  const char* TrafficInfo="TrafficInfo";
  const char* UnitParameter="UnitParameter";
  const char* Version="Version";
  const char* WorkMode="WorkMode";
  const char* WorkStatus="WorkStatus";
};


namespace IFACTION{
	const char* Query="query";
	const char* QueryRes="query_res";
	const char* Setting="setting";
	const char* SettingRes="setting_res";
};

namespace RESERVE
{
	uint8_t DEFAULT[5]={0x01,0x01,0x01,0x01,0x01};
	uint8_t SETTING[5]={0x05,0x04,0x03,0x02,0x01};
};
