#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
	#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vk_enum_string_helper.h>
#include <VulkanBackend.h>

#include <Query.h>

std::vector<const char*> InstanceExtensions = { "VK_KHR_get_physical_device_properties2", "VK_KHR_external_memory_capabilities"};

#ifdef _WIN32
std::vector<const char*> DeviceExtensions = {"VK_KHR_external_memory_win32", "VK_KHR_win32_keyed_mutex"};
#else
std::vector<const char*> DeviceExtensions = {};
#endif

extern void DriverLog(const char* LogMessage);

VkBackend::VkBackend(uint32_t inWidth, uint32_t inHeight) : Width(inWidth), Height(inHeight), TexCache({})
{}

VkBackend::~VkBackend()
{
    for (uint32_t i = 0; i < TexCache.size(); i++)
    {
      vkDestroyImage(Device, TexCache[i].Image, nullptr);
      vkFreeMemory(Device, TexCache[i].Memory, nullptr);
    }

    vkDestroyDevice(Device, nullptr);
    vkDestroyInstance(Instance, nullptr);
}

void VkBackend::Init()
{
  VkResult Res;

  VkApplicationInfo AppInfo{};
  AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  AppInfo.applicationVersion = VK_VERSION_1_1;

  VkInstanceCreateInfo InstanceCI{};
  InstanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  InstanceCI.pApplicationInfo = &AppInfo;
  InstanceCI.enabledExtensionCount= InstanceExtensions.size();
  InstanceCI.ppEnabledExtensionNames = InstanceExtensions.data();

  if((Res = vkCreateInstance(&InstanceCI, nullptr, &Instance)) != VK_SUCCESS)
  {
    std::cout << "-----------__VULKAN__------------\nError:\nuh oh " << string_VkResult(Res) << "\n";
  }

  uint32_t PDevCount;
  vkEnumeratePhysicalDevices(Instance, &PDevCount, nullptr);
  std::vector<VkPhysicalDevice> PDevices(PDevCount);
  vkEnumeratePhysicalDevices(Instance, &PDevCount, PDevices.data());

  for(uint32_t i = 0; i < PDevCount; i++)
  {
    VkPhysicalDeviceProperties DevProps;
    vkGetPhysicalDeviceProperties(PDevices[i], &DevProps);
    if(DevProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
      PDev = PDevices[i];
      DriverLog("Found suitable Physical Device");
      break;
    }
  }

  VkDeviceCreateInfo DevCI{};
  DevCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  if((Res = vkCreateDevice(PDev, &DevCI, nullptr, &Device)) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create VkDevice with error : " + std::to_string(Res));
  }

  QueryFormats(&PDev);

  DriverLog("Ran VkBackend Init");
}

void VkBackend::Cleanup()
{
}

int VkBackend::GetMemIndex(uint32_t MemType)
{
  VkPhysicalDeviceMemoryProperties MemProps;
  vkGetPhysicalDeviceMemoryProperties(PDev, &MemProps);

  for(uint32_t i = 0; i < MemProps.memoryTypeCount; i++)
  {
    if(MemProps.memoryTypes[i].propertyFlags & MemType)
    {
      return i;
    }
  }

  std::cout << "Failed to find suitable memory index for memory type " << MemType << '\n';
  return -1;
}

Texture* VkBackend::GetImageFromFD(uint64_t FD)
{
    VkResult Res;

    uint32_t TargetIndex;
    if (TexCache.count(FD))
    {
        return &TexCache[FD];
    }

    Texture ExtImage;
    ExtImage.SharedHandle = FD;

    VkExtent3D Extent = { Width, Height, 1 };

    VkExternalMemoryImageCreateInfo ExtImageCI{};
    ExtImageCI.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
    ExtImageCI.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

    VkImageCreateInfo ImageCI{};
    ImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCI.pNext = &ExtImageCI;
    ImageCI.tiling = VK_IMAGE_TILING_LINEAR;
    ImageCI.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    ImageCI.format = VK_FORMAT_R8G8B8A8_UNORM;
    ImageCI.extent = Extent;
    ImageCI.samples = VK_SAMPLE_COUNT_1_BIT;
    ImageCI.imageType = VK_IMAGE_TYPE_2D;
    ImageCI.mipLevels = 1;
    ImageCI.arrayLayers = 1;
    ImageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(Device, &ImageCI, nullptr, &ExtImage.Image) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create image");
    }

    VkMemoryRequirements MemReq{};

    vkGetImageMemoryRequirements(Device, ExtImage.Image, &MemReq);

    VkMemoryDedicatedAllocateInfoKHR DedAlloc{};
    DedAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO_KHR;
    DedAlloc.image = ExtImage.Image;

#ifdef _WIN32
    VkImportMemoryWin32HandleInfoKHR ImportInfo{};
    ImportInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_WIN32_HANDLE_INFO_KHR;
    ImportInfo.pNext = &DedAlloc;
    ImportInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
    ImportInfo.handle = (HANDLE)FD;
#else
    VkImportMemoryFdInfoKHR ImportInfo{};
    ImportInfo.sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR;
    ImportInfo.pNext = &DedAlloc;
    ImportInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
    ImportInfo.fd = FD;
#endif

    VkMemoryAllocateInfo AllocInfo{};
    AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    AllocInfo.pNext = &ImportInfo;
    AllocInfo.allocationSize = MemReq.size;
    AllocInfo.memoryTypeIndex = GetMemIndex(MemReq.memoryTypeBits);

    if ((Res = vkAllocateMemory(Device, &AllocInfo, nullptr, &ExtImage.Memory)) != VK_SUCCESS)
    {
        DriverLog("W1reless : Failed to allocate memory with VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR handle type\n");

        ImportInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT_KHR;
        AllocInfo.pNext = &ImportInfo;

        if ((Res = vkAllocateMemory(Device, &AllocInfo, nullptr, &ExtImage.Memory)) != VK_SUCCESS)
        {
            DriverLog("W1reless : Failed to allocate memory with VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT_KHR handle type\n");

            ImportInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_HEAP_BIT_KHR;
            AllocInfo.pNext = &ImportInfo;

            if ((Res = vkAllocateMemory(Device, &AllocInfo, nullptr, &ExtImage.Memory)) != VK_SUCCESS)
            {
                DriverLog("W1reless : Failed to allocate memory with VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_HEAP_BIT_KHR handle type with error ");

                ImportInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT_KHR;
                AllocInfo.pNext = &ImportInfo;

                if ((Res = vkAllocateMemory(Device, &AllocInfo, nullptr, &ExtImage.Memory)) != VK_SUCCESS)
                {
                    DriverLog("W1reless : Failed to allocate memory with VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT_KHR handle type\n");
                }
                else
                {
                    DriverLog("W1reless : allocated memory with VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_RESOURCE_BIT_KHR handle type\n");
                }
            }
            else
            {
                DriverLog("W1reless : allocated memory with VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D12_HEAP_BIT_KHR handle type\n");
            }
        }
        else
        {
            DriverLog("W1reless : allocated memory with VK_EXTERNAL_MEMORY_HANDlE_TYPE_D3D11_TEXTURE_BIT_KHR handle type\n");
        }
    }
    else
    {
        DriverLog("W1reless : allocated memory with VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR handleType\n");
    }

    vkBindImageMemory(Device, ExtImage.Image, ExtImage.Memory, 0);

    TexCache[FD] = ExtImage;

  return &TexCache[FD];
}

void VkBackend::SyncImage(uint64_t FD)
{
}
