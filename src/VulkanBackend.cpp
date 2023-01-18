#include <VulkanBackend.h>
#include <string>
#include <vector>

std::vector<const char**> InstanceLayers = {"VK_KHRONOS_LAYER_validation"};
std::vector<const char**> DeviceExtensions = {"VK_KHR_external_memory"};

void Init()
{
    VkInstanceCreateInfo InstanceCI{};
    InstanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCI.layerCount = InstanceLayers.size();
    InstanceCI.ppLayerNames = InstanceLayers.data();

    if(VkCreateInstance(&InstanceCI, nullptr, &Instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Instance");
    }

    uint32_t PDevCount;
    vkEnumeratePhysicalDevices(Instance, &PDevCount, nullptr);
    std::vector<VkPhysicalDevice> PDevices(PDevCount);
    vkEnumeratePhysicalDevices(Instance, &PDevCount, PDev.data());

    for(uint32_t i = 0; i < PDevCount; i++)
    {
        
    }
}
