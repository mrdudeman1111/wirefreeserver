#pragma once

#include <atomic>
#include <iostream>
#include <memory>
#include <vector>
#include <thread>

#ifdef _WIN32
	#include <openvr_driver.h>
#else
	#include <openvr/openvr_driver.h>
#endif

#include <VulkanBackend.h>

void DriverLog(const char* LogMessage);

class VirtDisplay : public vr::IVRVirtualDisplay, public vr::ITrackedDeviceServerDriver
{
public:
  VirtDisplay();

  ~VirtDisplay();

// IVRVirtualDisplay
  void Present(const vr::PresentInfo_t *pPresentInfo, uint32_t unPresentInfoSize) override;

  void WaitForPresent() override;

  bool GetTimeSinceLastVsync(float* pfSecondsSinceLastVsync, uint64_t* pulFrameCounter) override;

// TrackedDeviceServer

  vr::EVRInitError Activate(uint32_t unObjectId) override;

  void Deactivate() override;

  void EnterStandby() override;

  void* GetComponent(const char* pchComponentNameAndVersion) override;

  void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;

  vr::DriverPose_t GetPose() override;




  void RunFrame();

  const std::string GetSerialNumber();

  void Cleanup();

  bool IsValid();


private:
  VkBackend* Vk;

  uint32_t ObjectId;

  std::string SerialNumber = "WDis-001";
  std::string ModelNumber = "Rev-001";
};

class W1relessDisplay : public vr::IVRDisplayComponent
{
public:
	// Display Component
	void GetWindowBounds(int32_t* pX, int32_t* pY, uint32_t* pWidth, uint32_t* pHeight) override;
	bool IsDisplayOnDesktop() override { return false; }
	bool IsDisplayRealDisplay() override { return true; }
	void GetRecommendedRenderTargetSize(uint32_t* pWidth, uint32_t* pHeight) override;
	void GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pX, uint32_t* pY, uint32_t* pWidth, uint32_t* pHeight) override;
	void GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom) override;
	vr::DistortionCoordinates_t ComputeDistortion(vr::EVREye eEye, float fU, float fV) override;

	const std::string GetSerial() { return SerialNumber; }
private:

	uint32_t Width = 3664;
	uint32_t Height = 1920;

	float IPD = 0.065;

	std::string SerialNumber = "W1reless-Display-Component";
	std::string ModelNumber = "W1reless-DComp-120";
};
