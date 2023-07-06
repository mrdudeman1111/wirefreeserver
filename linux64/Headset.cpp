#include <Headset.h>

#include <cstring>
#include <math.h>

#ifdef _WIN32
#include <openvr_driver.h>
#else
#include <openvr/openvr_driver.h>
#endif

// Tracked Server Device
  vr::EVRInitError HeadsetController::Activate(uint32_t unObjectId)
  {
    ObjectId = unObjectId;

    vr::PropertyContainerHandle_t Container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);

  // ITrackedServerDevice
    // tell the frontend the model number
    vr::VRProperties()->SetStringProperty(Container, vr::Prop_ModelNumber_String, HeadsetMN.c_str());

    // set the input profile
    vr::VRProperties()->SetStringProperty(Container, vr::Prop_InputProfilePath_String, "{W1reless}/input/W1reless_profile.json");

    DriverLog("Headset Activated");

    // Setup Inputs

    return vr::VRInitError_None;
  }

  void HeadsetController::Deactivate()
  {
    VirtDis->Cleanup();
    delete VirtDis;
    ObjectId = vr::k_unTrackedDeviceIndexInvalid;
  }

  void HeadsetController::EnterStandby()
  {
    DriverLog("Headset entering standby");
    return;
  }

  void* HeadsetController::GetComponent(const char *pchComponentNameAndVersion)
  {
    if(strcmp(pchComponentNameAndVersion, vr::IVRVirtualDisplay_Version) == 0)
    {
        return VirtDis;
    }
    if(strcmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version) == 0)
    {
        return (vr::IVRDisplayComponent*)this;
    }

    return nullptr;
  }

  void HeadsetController::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize)
  {
    if(unResponseBufferSize > 0)
    {
      pchResponseBuffer[0] = 0;
    }
  }

  vr::DriverPose_t HeadsetController::GetPose()
  {
    vr::DriverPose_t Ret{0};

    Ret.poseIsValid = true;
    Ret.result = vr::TrackingResult_Running_OK;
    Ret.deviceIsConnected = true;
    
    Ret.qWorldFromDriverRotation.w = 1.f;
    Ret.qDriverFromHeadRotation.w = 1.f;

    Ret.qRotation.w = 1.f;

    Ret.vecPosition[0] = 0.f;
    Ret.vecPosition[1] = sin(FrameNumber * 0.01) * 0.1f + 1.f;
    Ret.vecPosition[0] = 0.f;

    Ret.shouldApplyHeadModel = true;

    return Ret;
  }

  const std::string HeadsetController::GetSerialNumber()
  {
    return HeadsetSN;
  }

  void HeadsetController::RunFrame()
  {
    FrameNumber++;
    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ObjectId, GetPose(), sizeof(vr::DriverPose_t));
  }

  void HeadsetController::ProcessEvent(const vr::VREvent_t& vrEvent)
  {}


  void HeadsetController::GetWindowBounds(int32_t* pX, int32_t* pY, uint32_t* pWidth, uint32_t* pHeight)
  {
      *pWidth = Width;
      *pHeight = Height;
      *pX = 0;
      *pY = 0;
  }

  void HeadsetController::GetRecommendedRenderTargetSize(uint32_t* pWidth, uint32_t* pHeight)
  {
      *pWidth = Width;
      *pHeight = Height;
  }
  void HeadsetController::GetEyeOutputViewport(vr::EVREye eEye, uint32_t* pX, uint32_t* pY, uint32_t* pWidth, uint32_t* pHeight)
  {
      *pWidth = Width / 2;
      *pHeight = Height;
      *pY = 0;
      *pX = (eEye == vr::Eye_Left) ? 0 : (Width / 2);
  }

  void HeadsetController::GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom)
  {
      *pfLeft = -1.f;
      *pfRight = 1.f;
      *pfTop = -1.f;
      *pfBottom = 1.f;
  }

  vr::DistortionCoordinates_t HeadsetController::ComputeDistortion(vr::EVREye eEye, float fU, float fV)
  {
      vr::DistortionCoordinates_t Coords{};
      Coords.rfRed[0] = fU;
      Coords.rfRed[1] = fV;
      Coords.rfGreen[0] = fU;
      Coords.rfGreen[1] = fV;
      Coords.rfBlue[0] = fU;
      Coords.rfBlue[1] = fV;

      return Coords;
  }
