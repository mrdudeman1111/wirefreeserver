#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdarg.h>

#include <openvr_driver.h>

#include <VulkanBackend.h>

#if defined( _WIN32 )
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined( __GNUC__ ) || defined( COMPILER_GCC ) || defined( __APPLE__ )
#define HMD_DLL_EXPORT extern "C" __attribute__( ( visibility( "default" ) ) )
#define HMD_DLL_IMPORT extern "C"
#else
#error "Unsupported Platform."
#endif

vr::IVRDriverLog* Logger;

void DriverLog(const char* LogMessage)
{
    Logger->Log(LogMessage);
}

class wHeadset : public vr::IVRVirtualDisplay, public vr::ITrackedDeviceServerDriver
{
public:
// ITrackedDeviceServerDriver

  vr::EVRInitError Activate(uint32_t unObjectId) override
  {
    Vk = new VkBackend();
    Vk->Init();
    ObjectID = unObjectId;
    return vr::VRInitError_None;
  }

  void Deactivate() override
  {
    Vk->Cleanup();
    ObjectID = vr::k_unTrackedDeviceIndexInvalid;
  }

  void EnterStandby() override
  {
    return;
  }

  void* GetComponent(const char *pchComponentNameAndVersion) override
  {
    if(strcmp(pchComponentNameAndVersion, vr::IVRVirtualDisplay_Version) == 0)
    {
      return static_cast<vr::IVRVirtualDisplay*>(this);
    }
    return NULL;
  }

  bool IsValid()
  {
    if(Vk)
    {
      return true;
    }

    return false;
  }

  void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) override
  {
    if(unResponseBufferSize > 0)
    {
      pchResponseBuffer[0] = 0;
    }
  }

  vr::DriverPose_t GetPose() override
  {
    vr::DriverPose_t Ret{0};
    Ret.poseIsValid = true;
    Ret.result = vr::TrackingResult_Running_OK;
    Ret.deviceIsConnected = true;
    Ret.qWorldFromDriverRotation.w = 1;
    Ret.qWorldFromDriverRotation.x = 0;
    Ret.qWorldFromDriverRotation.y = 0;
    Ret.qWorldFromDriverRotation.z = 0;
    Ret.qDriverFromHeadRotation.w = 1;
    Ret.qDriverFromHeadRotation.x = 0;
    Ret.qDriverFromHeadRotation.y = 0;
    Ret.qDriverFromHeadRotation.z = 0;
    return Ret;
  }

  std::string GetSerialNumber()
  {
    return "W1reless Headset";
  }

// IVRVirtualDisplay
  void Present(const vr::PresentInfo_t *pPresentInfo, uint32_t unPresentInfoSize) override
  {
    DriverLog("Presenting");
    Texture* Tx = Vk->GetImageFromFD(pPresentInfo->backbufferTextureHandle);
  }

  void WaitForPresent() override
  {
  }

  bool GetTimeSinceLastVsync(float* pfSecondsSinceLastVsync, uint64_t* pulFrameCounter) override
  {
    *pfSecondsSinceLastVsync = 0.33;
    *pulFrameCounter = 1;
    return false;
  }

  void Cleanup()
  {
    Vk->Cleanup();
    ObjectID = 0;
  }

private:
  VkBackend* Vk;

  uint64_t ObjectID;
};

class W1relessProvider : public vr::IServerTrackedDeviceProvider
{
public:
  W1relessProvider(){}
  ~W1relessProvider(){}

  vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext) override
  {
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

    Logger = vr::VRDriverLog();
    
    Headset = new wHeadset();

    if(Headset->IsValid())
    {
      vr::VRServerDriverHost()->TrackedDeviceAdded(Headset->GetSerialNumber().c_str(), vr::TrackedDeviceClass_DisplayRedirect, (vr::ITrackedDeviceServerDriver*)Headset);
    }

    // Init Headset driver ( Logical headset, video, etc )

    return vr::VRInitError_None;
  }

  const char *const* GetInterfaceVersions() override
  {
    return vr::k_InterfaceVersions;
  }

  void RunFrame() override
  {
  }

  bool ShouldBlockStandbyMode() override
  {
    return false;
  }

  void EnterStandby() override
  {
    DriverLog("EnterStandby");
  }

  void LeaveStandby() override
  {
    DriverLog("LeaveStandby");
  }

  void Cleanup() override
  {
    Headset->Cleanup();
    VR_CLEANUP_SERVER_DRIVER_CONTEXT();
    DriverLog("cleaning up now");
  }

private:
  wHeadset* Headset;
};

W1relessProvider Provider;

HMD_DLL_EXPORT void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode)
{
  if(strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName) == 0)
  {
    return &Provider;
  }

  DriverLog("HmdDriverFactory failed to create a device provider for W1reless Driver");
  return nullptr;
}

