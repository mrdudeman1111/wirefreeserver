#pragma once

#include <Display.h>
#include <charconv>

#ifdef _WIN32
	#include <openvr_driver.h>
#else
	#include <openvr/openvr_driver.h>
#endif

class HeadsetController : public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent
{
public:

  HeadsetController()
  {
    VirtDis = new VirtDisplay(Width, Height);

    char tempSN[512];
    char tempMN[512];

    vr::VRSettings()->GetString("driver_W1reless", "serialNumber", tempSN, 512);
    vr::VRSettings()->GetString("driver_W1reless", "modelNumber", tempMN, 512);

    HeadsetSN = tempSN;
    HeadsetMN = tempMN;
  };

  bool IsValid(){ return VirtDis->IsValid(); }

// Device Server Driver
  vr::EVRInitError Activate(uint32_t unObjectId) override;
  void Deactivate() override;
  void EnterStandby() override;
  void* GetComponent(const char* pchComponentNameAndVersion) override;
  void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;
  vr::DriverPose_t GetPose() override;


  void GetWindowBounds(int32_t* pnX, int32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override;

  bool IsDisplayOnDesktop() { return false; }
  bool IsDisplayRealDisplay() { return true; }

  void GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight)override;
  void GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pnX, uint32_t* pnY, uint32_t* pnWidth, uint32_t* pnHeight) override;
  void GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom) override;
  vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) override;

  const std::string GetSerialNumber();
  void RunFrame();
  void ProcessEvent(const vr::VREvent_t& vrEvent);

  VirtDisplay* VirtDis;
private:

  std::string HeadsetMN;
  std::string HeadsetSN;

  vr::VRInputComponentHandle_t InputHandles[16];

  uint32_t Width = 3664;
  uint32_t Height = 1920;

  float IPD = 0.065;

  std::atomic_int FrameNumber;
  std::atomic_bool Active;
  std::atomic_uint32_t DeviceIndex;

  uint32_t ObjectId;
};

