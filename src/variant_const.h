#ifndef VARIANT_CONST_H
#define VARIANT_CONST_H
#include <cstdint>
//
enum MachineType
{
  XINGZHI,
  HAIXIN,
  HAIKANG,
  XIMENZI,
  DEVICE5
};

//

// 接口指令类型字符串，用于匹配客户端的json请求
namespace  IFTYPE{
  extern const char* ActionTable;
  extern const char* CharacterParameterVersion;
  extern const char* Detector;
  extern const char* FailureConfig;
  extern const char* Failure;
  extern const char* FollowPhaseTable;
  extern const char* IdentificationCode;
  extern const char* LightStatus;
  extern const char* PedestrianDetector;
  extern const char* Phase;
  extern const char* PhaseSequenceTable;
  extern const char* PriorityConfig;
  extern const char* RemoteControl;
  extern const char* Schedule;
  extern const char* SchemaTable;
  extern const char* SemaphoreGroup;
  extern const char* Time;
  extern const char* TimeTable;
  extern const char* TimingScheme;
  extern const char* TrafficInfo;
  extern const char* UnitParameter;
  extern const char* Version;
  extern const char* WorkMode;
  extern const char* WorkStatus;
};

// 接口操作类型字符串，用于匹配客户端、写回客户端的json请求
namespace IFACTION{
	extern const char* Query;
	extern const char* QueryRes;
	extern const char* Setting;
	extern const char* SettingRes;
};


#endif
