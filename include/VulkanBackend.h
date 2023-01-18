#include <vulkan/vulkan.h>
#include <iostream>
#include <string>

class VkRenderer
{
    public:
    VkDevice Device;
    VkPhysicalDevice PDev;
    VkInstance Instance;

    VkRenderer();
    ~VkRenderer();

    /// Eventually I will add Params Height, Width, BitRate, NetProto.
    void Init();
};