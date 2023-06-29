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

  vr::VRSettings()->GetString("driver_W1reless", "serialNumber", tmpSN, 512);
  vr::VRSettings()->GetString("driver_W1reless", "modelNumber", tmpMN, 512);

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
    ObjectId = unObjectId;

    Vk = new VkBackend();
    Vk->Init();

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

