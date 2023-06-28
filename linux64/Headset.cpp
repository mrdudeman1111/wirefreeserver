#include <Headset.h>

#include <cstring>
#include <math.h>
#include <openvr/openvr_driver.h>

// Tracked Server Device
  vr::EVRInitError HeadsetController::Activate(uint32_t unObjectId)
  {
    ObjectId = unObjectId;

    VirtDis = new VirtDisplay();

    if(VirtDis->IsValid())
    {
      vr::VRServerDriverHost()->TrackedDeviceAdded(VirtDis->GetSerialNumber().c_str(), vr::TrackedDeviceClass_DisplayRedirect, VirtDis);
    }
    else
    {
      std::cout << "W1reless : Failed to add Virtual display as component";
    }

    vr::PropertyContainerHandle_t Container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);

    // tell the frontend the model number
    vr::VRProperties()->SetStringProperty(Container, vr::Prop_ModelNumber_String, HeadsetMN.c_str());

    const float IPD = vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_IPD_Float);
    vr::VRProperties()->SetFloatProperty(Container, vr::Prop_UserIpdMeters_Float, IPD);

    vr::VRProperties()->SetFloatProperty(Container, vr::Prop_DisplayFrequency_Float, 90.f);

    // the distance between eyes and display
    vr::VRProperties()->SetFloatProperty(Container, vr::Prop_UserHeadToEyeDepthMeters_Float, 0.f);

    // the time between present() and actual presentation.
    vr::VRProperties()->SetFloatProperty(Container, vr::Prop_SecondsFromVsyncToPhotons_Float, 0.11);

    vr::VRProperties()->SetBoolProperty(Container, vr::Prop_IsOnDesktop_Bool, false);

    vr::VRProperties()->SetBoolProperty(Container, vr::Prop_DisplayDebugMode_Bool, true);

    // set the input profile
    vr::VRProperties()->SetStringProperty(Container, vr::Prop_InputProfilePath_String, "{W1reless}/input/W1reless_profile.json");

    // Setup Inputs

    return vr::VRInitError_None;
  }

  void HeadsetController::Deactivate()
  {
    VirtDis->Cleanup();
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
      return &VirtDis;
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

  const std::string HeadsetController::GetSerialNumber()
  {
    return HeadsetSN;
  }

  void HeadsetController::RunFrame()
  {
    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(ObjectId, GetPose(), sizeof(vr::DriverPose_t));
  }

  void HeadsetController::ProcessEvent(const vr::VREvent_t& vrEvent)
  {}

