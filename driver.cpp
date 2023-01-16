#include <iostream>
#include <cstring>
#include <string>
#include <stdlib.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define InPort 2402
#define OutPort 2403

#include <openvr/openvr_driver.h>

#if defined(_WIN32)
    #define HMD_DLL_EXPORT extern "C" __declspec(dllexport)
    #define HMD_DLL_IMPORT extern "C" __declspec(dllimport)
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined (___APPLE__)
    #define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
    #define HMD_DLL_IMPORT extern "C"
#endif

class QuestInterface
{
    public:
    int OutSockFd, InSockFd;
    int Opt = 1;
    sockaddr_in InAddress, OutAddress;

    void SetupPorts()
    {
        if(OutSockFd = socket(AF_INET, SOCK_DGRAM, 0) == -1)
        {
            throw std::runtime_error("Failed to create Out socket");
        }

        if(InSockFd = socket(AF_INET, SOCK_STREAM, 0) == -1)
        {
            throw std::runtime_error("Failed to create In socket");
        }

        OutAddress.sin_family = AF_INET;
        OutAddress.sin_addr.s_addr = INADDR_ANY;
        OutAddress.sin_port = htons(OutPort);

        InAddress.sin_family = AF_INET;
        InAddress.sin_addr.s_addr = INADDR_ANY;
        InAddress.sin_port = htons(InPort);

        if(bind(OutSockFd, (const sockaddr*)&InAddress, sizeof(InAddress)) == -1)
        {
            throw std::runtime_error("Failed to bind Output Socket");
        }

        if(bind(InSockFd, (const sockaddr*)&OutAddress, sizeof(OutAddress)) == -1)
        {
            throw std::runtime_error("Failed to bind Input socket");
        }

        // Send Image Sizes.
        // Recieve GtG
        // Pull the buffer from VkBuffer of VkImage
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
        vr::DriverPose_t Pose = vr::DriverPose_t();
        return Pose;
    }
};

class HeadsetDriver : vr::ITrackedDeviceServerDriver, vr::IVRDriverDirectModeComponent
{
    public:
    vr::TrackedDeviceIndex_t ObjId;
    vr::PropertyContainerHandle_t PropContainer;

// Inherited from vr::ITrackeDeviceServerDriver
    virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t unObjectId)
    {
        ObjId = unObjectId;
        PropContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);
        vr::CVRPropertyHelpers* Prop = vr::VRProperties();

        Prop->SetBoolProperty(PropContainer, vr::Prop_IsOnDesktop_Bool, false);
        return vr::VRInitError_None;
    }

// Inherited from vr::IVRDriverDirectModeComponent
    virtual void CreateSwapTextureSet(uint32_t unPid, const SwapTextureSet_t* pSwapTextureSetDesc, SwapTextureSet_t* pOutSwapTextureSet)
    {

    }

    virtual void DestroySwapTextureSet(vr::SharedTextureHandle_t sharedTextureHandle)
    {

    }

    virtual void DestroyAllSwapTextureSets(uint32_t unPid)
    {

    }

    virtual void GetNextSwapTextureSetIndex(vr::SharedTextureHandle_t sharedTextureHandles[2], uint32_t* pIndices[2])
    {

    }

    virtual void SubmitLayer(const SubmitLayerPerEye_t& EyeLayers[2])
    {

    }

    virtual void PostPresent()
    {

    }

    virtual void Present(vr::SharedTextureHandle_t syncTexture)
    {

    }

    virtual void GetFrameTiming(vr::DriverDirectMode_FrameTiming* pFrameTiming)
    {
        // 11ms Screen latency    >1ms network latency    33ms  Tracking Latency   12 ms Encode Latency
    }
};

class W1relessProvider : vr::IServerTrackedDeviceProvider
{
    public:
    RightController *RController;
    HeadsetDriver *Hmd;

    virtual const char* const* GetInterfaceVersions()
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
        // if(RController)
        // {
        //     RController->RunFrame();
        // }
        // if(Hmd)
        // {
        //     Hmd->RunFrame();
        // }

        // vr::VREvent_t VrEvent;
        // while(vr::VRServerDriverHost()->PollNextEvent(&VrEvent, sizeof(VrEvent)))
        // {
        //     if(RController)
        //     {
        //         RController->ProcessEvent(VrEvent);
        //     }
        // }
    }

    virtual void EnterStandby()
    {

    }

    virtual void LeaveStandby()
    {

    }

    virtual bool ShouldBlockStandbyMode() {return true;}
};

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
