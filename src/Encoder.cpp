#include <vector>
#include <iostream>
#include <uuid/uuid.h>

#include <AMF/core/Context.h>
#include <AMF/core/VulkanAMF.h>
#include <AMF/core/Buffer.h>
#include <AMF/components/VideoEncoderHEVC.h>
#include <AMF/core/Factory.h>

static amf::AMF_MEMORY_TYPE MemoryType= amf::AMF_MEMORY_VULKAN;

static const int CodecIndex = 1; // 0 = AVC (h.264)  1 = HEVC (h.265)  2 = AV1

static const amf_int32 FrameRate = 90;

amf::AMFPlanePtr RightColor1;
amf::AMFPlanePtr RightColor2;

amf::AMFPlanePtr LeftColor1;
amf::AMFPlanePtr LeftColor2;

static void FillVkSurface(amf::AMFContext* Context, amf::AMFSurface* Surface)
{
    amf::AMFComputePtr Compute;
    Context->GetCompute(amf::AMF_MEMORY_VULKAN, &Compute);

    // for(amf_size p = 0; p < RightColor1->GetPlanesCount(); p++)
    // {
    // }
}

class Encoder
{
    public:
    amf::AMFContextPtr Context;
    amf::AMFComponentPtr Encoder;
    // Source object (Grabbed from our Display Driver in driver.cpp)

    void Run()
    {

    }
};