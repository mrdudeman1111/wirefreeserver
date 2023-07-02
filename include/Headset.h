#pragma once

#include <Display.h>
#include <charconv>

#ifdef _WIN32
	#include <openvr_driver.h>
#else
	#include <openvr/openvr_driver.h>
#endif

class HeadsetController : public vr::ITrackedDeviceServerDriver
{
public:

  HeadsetController()
  {
    VirtDis = new VirtDisplay();
    Display = new W1relessDisplay();

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

  const std::string GetSerialNumber();
  void RunFrame();
  void ProcessEvent(const vr::VREvent_t& vrEvent);

  VirtDisplay* VirtDis;
  W1relessDisplay* Display;
private:

  std::string HeadsetMN;
  std::string HeadsetSN;

  vr::VRInputComponentHandle_t InputHandles[16];

  std::atomic_int FrameNumber;
  std::atomic_bool Active;
  std::atomic_uint32_t DeviceIndex;

  uint32_t ObjectId;
};

