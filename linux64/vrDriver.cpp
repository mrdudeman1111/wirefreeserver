#include <cinttypes>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <vector>

#include <Headset.h>

#include <stdio.h>
#include <stdarg.h>

#if defined(__LINUX__) || defined(__unix__) || defined(__APPLE__)
  #include <openvr/openvr_driver.h>
#elif defined(_WIN32) || defined(_WIN64)
  #include <openvr_driver.h>
#endif

#if defined( _WIN32 )
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined( __GNUC__ ) || defined( COMPILER_GCC ) || defined( __APPLE__ )
#define HMD_DLL_EXPORT extern "C" __attribute__( ( visibility( "default" ) ) )
#define HMD_DLL_IMPORT extern "C"
#else
#error "Unsupported Platform."
#endif




/* For Debug, we just run the vrstartup.sh in ~/.steam/steam/steamapps/common/SteamVR/bin and look at it for std::cout output */

extern vr::IVRDriverLog* driLogger;

class W1relessProvider : public vr::IServerTrackedDeviceProvider
{
public:
  W1relessProvider(){}
  ~W1relessProvider(){}

  vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext) override
  {
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

    std::cout << "W1reless : Provider init called\n";

    driLogger = vr::VRDriverLog();

    Headset = new HeadsetController();

    if(Headset->IsValid())
    {
      std::string Serial = Headset->GetSerialNumber().data();
      DriverLog(Serial.data());
      if(vr::VRServerDriverHost()->TrackedDeviceAdded(Serial.data() , vr::TrackedDeviceClass_HMD, Headset))
      {
        std::cout << "W1reless : ---------------------__W1reless_DRIVER__------------------\nSuccess:\nHeadest WORKED!!!!!!!!!!!!\n";
      }
      else
      {
        std::cout << "W1reless : ---------------------__W1reless_DRIVER__------------------\nError:\nheadset invalid!!!!!!\n";
      }
    }

    // Init Headset driver ( Logical headset, video, etc)

    return vr::VRInitError_None;
  }

  const char *const* GetInterfaceVersions() override
  {
    return vr::k_InterfaceVersions;
  }

  void RunFrame() override
  {
    if(Headset)
    {
      Headset->RunFrame();
    }

    vr::VREvent_t Event{};
    while(vr::VRServerDriverHost()->PollNextEvent(&Event, sizeof(vr::VREvent_t)))
    {
      if(Headset)
      {
        Headset->ProcessEvent(Event);
      }
    }
  }

  bool ShouldBlockStandbyMode() override
  {
    return false;
  }

  void EnterStandby() override
  {
  }

  void LeaveStandby() override
  {
  }

  void Cleanup() override
  {
    delete Headset;
    Headset = nullptr;
    VR_CLEANUP_SERVER_DRIVER_CONTEXT();
  }

private:
  HeadsetController* Headset;
};

W1relessProvider Provider;

HMD_DLL_EXPORT void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode)
{
  std::cout << "\nW1reless : Library loaded\n";
  if(strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName) == 0)
  {
    return &Provider;
  }

  FILE* file = fopen("/run/media/ethanw/LinuxGames/Repos/wirefreeserver/Output.txt", "w");
  fwrite("Fuck", sizeof(char)*4, 1, file);
  return nullptr;
}

