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
    /*
    if (VirtDis->IsValid())
    {
      DriverLog("Virtual Display is valid\n");
      std::string VirtDisSerial = VirtDis->GetSerialNumber().c_str();
      if(vr::VRServerDriverHost()->TrackedDeviceAdded(VirtDisSerial.data(), vr::TrackedDeviceClass_DisplayRedirect, VirtDis))
      {
          std::cout << "W1reless : Succeeded in adding virtual display\n";
          DriverLog("W1reless : Succeeded in adding virtual display");
      }
      else
      {
          std::cout << "W1reless : Failed to add virtual display component\n";
          DriverLog("Failed to add virtual display component");
      }
    }
    else
    {
        std::cout << "W1reless : Failed to add virtual display component\n";
        DriverLog("Failed to add virtual display component");
    }
    */

    vr::PropertyContainerHandle_t Container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);

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

