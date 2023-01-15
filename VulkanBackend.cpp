#include <vulkan/vulkan.h>

void InitateRenderableSurface(RenderableSurface* Surface)
{

}

struct RenderableSurface
{
    public:
    VkSurface Surface;
    VkDevice Device;
    VkPhysicalDevice PDev;
    VkInstance Instance;
}