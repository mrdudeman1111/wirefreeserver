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

class VirtDisplay : public vr::IVRVirtualDisplay
{
public:
  VirtDisplay(uint32_t inWidth, uint32_t inHeight);

  ~VirtDisplay();

// IVRVirtualDisplay
  void Present(const vr::PresentInfo_t *pPresentInfo, uint32_t unPresentInfoSize) override;

  void WaitForPresent() override;

  bool GetTimeSinceLastVsync(float* pfSecondsSinceLastVsync, uint64_t* pulFrameCounter) override;

  void RunFrame();

  const std::string GetSerialNumber();

  void Cleanup();

  bool IsValid();


private:
  VkBackend* Vk;

  uint32_t Width;
  uint32_t Height;

  uint32_t ObjectId;

  std::string SerialNumber = "WDis-001";
  std::string ModelNumber = "Rev-001";
};
