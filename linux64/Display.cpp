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

VirtDisplay::VirtDisplay(uint32_t inWidth, uint32_t inHeight) : Width(inWidth), Height(inHeight)
{
  Vk = new VkBackend(Width, Height);
  Vk->Init();

  char tmpSN[512];
  char tmpMN[512];

  vr::VRSettings()->GetString("driver_W1reless_Display", "serialNumber", tmpSN, 512);
  vr::VRSettings()->GetString("driver_W1reless_Display", "modelNumber", tmpMN, 512);

  SerialNumber = tmpSN;
  ModelNumber = tmpMN;

  SerialNumber.shrink_to_fit();
  ModelNumber.shrink_to_fit();

  std::cout << "W1reless: Virtual Display Serial Number: " << SerialNumber.data() << "\nW1reless: Virtual Display Number: " << ModelNumber.data() << '\n';
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
