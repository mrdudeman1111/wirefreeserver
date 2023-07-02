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
    VirtDis = new VirtDisplay();

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

// Display Component
  void GetWindowBounds(int32_t* pX, int32_t* pY, uint32_t* pWidth, uint32_t* pHeight) override;
  bool IsDisplayOnDesktop() override { return false; }
  bool IsDisplayRealDisplay() override { return true; }
  void GetRecommendedRenderTargetSize(uint32_t* pWidth, uint32_t* pHeight) override;
  void GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pX, uint32_t* pY, uint32_t* pWidth, uint32_t* pHeight) override;
  void GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom) override;
  vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) override;

  const std::string GetSerialNumber();
  void RunFrame();
  void ProcessEvent(const vr::VREvent_t& vrEvent);

  VirtDisplay* VirtDis;
private:

  std::string HeadsetMN;
  std::string HeadsetSN;

  uint32_t Width = 3664;
  uint32_t Height = 1920;

  float IPD = 0.04;

  vr::VRInputComponentHandle_t InputHandles[16];

  std::atomic_int FrameNumber;
  std::atomic_bool Active;
  std::atomic_uint32_t DeviceIndex;

  uint32_t ObjectId;
};

