#include <VulkanBackend.h>
#include <string>
#include <vector>

std::vector<const char**> InstanceLayers = {"VK_KHRONOS_LAYER_validation"};
std::vector<const char**> DeviceExtensions = {"VK_KHR_external_memory"};

void Init()
{
    VkInstanceCreateInfo InstanceCI{};
    InstanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
}
