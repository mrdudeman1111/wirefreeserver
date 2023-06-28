#pragma once

#include <Display.h>
#include <openvr/openvr_driver.h>

class HeadsetController : public vr::ITrackedDeviceServerDriver
{
public:

  HeadsetController(){};

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

private:
  VirtDisplay* VirtDis;

  std::string HeadsetMN;
  std::string HeadsetSN;

  uint32_t Width = 3664;
  uint32_t Height = 1920;

  vr::VRInputComponentHandle_t InputHandles[16];

  std::atomic_int FrameNumber;
  std::atomic_bool Active;
  std::atomic_uint32_t DeviceIndex;

  uint64_t ObjectId;
};
