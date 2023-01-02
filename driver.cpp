#include <iostream>
#include <cstring>
#include <openvr/openvr_driver.h>

#if defined(_WIN32)
    #define HMD_DLL_EXPORT extern "C" __declspec(dllexport)
    #define HMD_DLL_IMPORT extern "C" __declspec(dllimport)
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined (___APPLE__)
    #define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
    #define HMD_DLL_IMPORT extern "C"
#endif

class W1relessProvider : vr::IServerTrackedDeviceProvider
{
    public:
    RightController RController;
    HeadsetDriver Hmd;

    virtual const char** GetInterfaceVersions()
    {
        return vr::k_InterfaceVersions;
    }

    virtual vr::EVRInitError Init(vr::IVRDriverContext* DriverContext)
    {
        VR_INIT_SERVER_DRIVER_CONTEXT(DriverContext);
        // Add Devices vr::VRServerDriverHost()->TrackedDeviceAdded()

        return vr::VRInitError_None;
    }

    virtual void Cleanup()
    {
        // Encoder.Destroy();
    }

    virtual void RunFrame()
    {
        // Encoder would take a frame, Encode, send, decode, and present
        if(RController)
        {
            RController.RunFrame();
        }
        if(Hmd)
        {
            Hmd.RunFrame();
        }

        vr::VREvent_t VrEvent;
        while(vr::VRServerDriverHost()->PollNextEvent(&VrEvent, sizeof(VrEvent)))
        {
            if(RController)
            {
                RController.ProcessEvent(VrEvent);
            }
        }
    }

    virtual void EnterStandby()
    {

    }

    virtual void LeaveStandby()
    {

    }
};

class RightController : vr::ITrackedDeviceServerDriver
{
    private:
    uint32_t DriverId;
    vr::VRInputComponentHandle_t JoystickY;
    vr::VRInputComponentHandle_t JoystickX;
    vr::VRInputComponentHandle_t JoystickClick;

    vr::VRInputComponentHandle_t ControllerPos;

    vr::VRInputComponentHandle_t AButtonClick;
    vr::VRInputComponentHandle_t BButtonClick;
    vr::VRInputComponentHandle_t AButtonHover;
    vr::VRInputComponentHandle_t BButtonHover;

    vr::CVRPropertyHelpers* Props;
    vr::IVRDriverInput* Input;

    vr::EVRInitError Activate(uint32_t ObjectId)
    {
        DriverId = ObjectId;
        Props = vr::VRProperties();
        Input = vr::VRDriverInput();

        vr::PropertyContainerHandle_t PropContainer = Props->TrackedDeviceToPropertyContainer(ObjectId);

        Props->SetInt32Property(PropContainer, vr::Prop_ControllerRoleHint_Int32, vr::TrackedControllerRole_RightHand);

        // Input->CreateScalarComponent(Props, "/input/right/thumbstick/y", &JoystickY, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
        // Input->CreateBooleanComponent(Props, "/input/right/thumbstick/click", &JoystickClick, vr::VRScalarType_Absolute);

        return vr::VRInitError_None;
    }

    vr::DriverPose_t GetPose()
    {
        vr::DriverPose_t Pose = (0);
        Pose.poseIsValid = true;
    }
};

class HeadsetDriver : vr::ITrackedDeviceServerDriver, vr::IVRDisplayComponent
{
    public:
    vr::TrackedDeviceIndex_t ObjId;
    vr::PropertyContainerHandle_t PropContainer;

    virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId)
    {
        ObjId = unObjectId;
        PropContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);
        vr::CVRPropertyHelpers* Prop = vr::VRProperties();

        Prop->SetBoolProperty(PropContainer, vr::Prop_IsOnDesktop_Bool, false);
    }
}

/* class DisplayComp : vr::IVRDisplayComponent
{
    void GetWindowBounds(int31_t* X, int32_t* Y, uint32_t* Width, uint32_t* Height)
    {

    }

    bool IsDisplayOnDesktop()
    {
        return false;
    }

    bool IsDisplayRealDisplay()
    {
        return true;
    }

    void GetRecommendedRenderTargetSize(uint31_t* Width, uint32_t* Height)
    {
        // return Encoder.ScreenSize();
    }
}; */

W1relessProvider DevProv;

HMD_DLL_EXPORT
void *HmdDriverFactory(const char* InterfaceName, int* ReturnCode)
{
    if(strcmp(InterfaceName, vr::IServerTrackedDeviceProvider_Version) == 0)
    {
        return &DevProv;
    }
    if(ReturnCode)
    {
        *ReturnCode = vr::VRInitError_Init_InterfaceNotFound;
    }
    return NULL;
}
