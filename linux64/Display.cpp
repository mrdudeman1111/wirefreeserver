#include <Display.h>
#include <cstring>

#ifdef _WIN32
#include <openvr_driver.h>
#else
#include <openvr/openvr_driver.h>
#endif

vr::IVRDriverLog* driLogger;

void DriverLog(const char* LogMessage)
{
  driLogger->Log(LogMessage);
}

VirtDisplay::VirtDisplay()
{
  Vk = new VkBackend();

  char tmpSN[512];
  char tmpMN[512];

  vr::VRSettings()->GetString("driver_W1reless_Display", "serialNumber", tmpSN, 512);
  vr::VRSettings()->GetString("driver_W1reless_Display", "modelNumber", tmpMN, 512);

  SerialNumber = tmpSN;
  ModelNumber = tmpMN;

  SerialNumber.shrink_to_fit();
  ModelNumber.shrink_to_fit();
}

VirtDisplay::~VirtDisplay()
{}

// Virtual display

  void VirtDisplay::Present(const vr::PresentInfo_t *pPresentInfo, uint32_t unPresentInfoSize)
  {
    DriverLog("Presenting");
    Texture* Tx = Vk->GetImageFromFD(pPresentInfo->backbufferTextureHandle);
  }

  void VirtDisplay::WaitForPresent()
  {
  }

  bool VirtDisplay::GetTimeSinceLastVsync(float* pfSecondsSinceLastVsync, uint64_t* pulFrameCounter)
  {
    *pfSecondsSinceLastVsync = 0.33;
    *pulFrameCounter = 1;
    return false;
  }

// Tracked Device Driver
  vr::EVRInitError VirtDisplay::Activate(uint32_t unObjectId)
  {
    DriverLog("Virtual display activate");
    ObjectId = unObjectId;

    Vk->Init();

    vr::PropertyContainerHandle_t Container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);
   
    const float IPD = vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_IPD_Float);
    vr::VRProperties()->SetFloatProperty(Container, vr::Prop_UserIpdMeters_Float, IPD);

    vr::VRProperties()->SetFloatProperty(Container, vr::Prop_DisplayFrequency_Float, 90.f);

    // the distance between eyes and display
    vr::VRProperties()->SetFloatProperty(Container, vr::Prop_UserHeadToEyeDepthMeters_Float, 0.f);

    // the time between present() and actual presentation.
    vr::VRProperties()->SetFloatProperty(Container, vr::Prop_SecondsFromVsyncToPhotons_Float, 0.11);

    vr::VRProperties()->SetBoolProperty(Container, vr::Prop_IsOnDesktop_Bool, false);

    vr::VRProperties()->SetBoolProperty(Container, vr::Prop_DisplayDebugMode_Bool, true);

    return vr::VRInitError_None;
  }

  void VirtDisplay::Deactivate()
  {
    ObjectId = vr::k_unTrackedDeviceIndexInvalid;

    Vk->Cleanup();
  }

  void VirtDisplay::EnterStandby()
  {
    DriverLog("W1reless: Virtual Display enterying standby\n");
  }

  void* VirtDisplay::GetComponent(const char* pchComponentNameAndVersion)
  {
    if(strcmp(pchComponentNameAndVersion, vr::IVRVirtualDisplay_Version) == 0)
    {
      return static_cast<vr::IVRVirtualDisplay*>(this);
    }
    else return nullptr;
  }

  void VirtDisplay::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
  {
    if(unResponseBufferSize > 0)
    {
      pchResponseBuffer[0] = 0;
    }
  }

  vr::DriverPose_t VirtDisplay::GetPose()
  {
    vr::DriverPose_t Ret{};
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

  void VirtDisplay::RunFrame()
  {}

  bool VirtDisplay::IsValid()
  {
    if(Vk != nullptr)
    {
      return true;
    }

    return false;
  }

  const std::string VirtDisplay::GetSerialNumber()
  {
    return SerialNumber;
  }

  void VirtDisplay::Cleanup()
  {
    Vk->Cleanup();
  }

