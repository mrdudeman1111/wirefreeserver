#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <vulkan/vk_enum_string_helper.h>
#include <VulkanBackend.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

std::vector<const char*> InstanceLayers = {};
std::vector<const char*> DeviceExtensions = {};

VkBackend::VkBackend()
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
      break;
    }
  }
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
  uint32_t TargetIndex;
  if(TexCache[FD].Image)
  {
    return &TexCache[FD];
  }

  Texture ExtImage;
  ExtImage.SharedHandle = FD;

  VkExtent3D Extent = {1280, 720, 1};

  VkExternalMemoryImageCreateInfo ExtImageCI{};
  ExtImageCI.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
  ExtImageCI.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;

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

  if(vkCreateImage(Device, &ImageCI, nullptr, &ExtImage.Image) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create image");
  }

  VkMemoryRequirements ExtImgReq;

  vkGetImageMemoryRequirements(Device, ExtImage.Image, &ExtImgReq);

  VkMemoryDedicatedAllocateInfoKHR DedicatedAllocInfo{};
  DedicatedAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
  DedicatedAllocInfo.image = ExtImage.Image;

  VkImportMemoryFdInfoKHR ImportInfo{};
  ImportInfo.sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR;
  ImportInfo.pNext = &DedicatedAllocInfo;
  ImportInfo.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
  ImportInfo.fd = FD;

  VkMemoryAllocateInfo AllocInfo{};
  AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  AllocInfo.pNext = &ImportInfo;
  AllocInfo.allocationSize = ExtImgReq.size;
  AllocInfo.memoryTypeIndex = GetMemIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  if(vkAllocateMemory(Device, &AllocInfo, nullptr, &ExtImage.Memory) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to allocate external image");
  }

  vkBindImageMemory(Device, ExtImage.Image, ExtImage.Memory, 0);

  TexCache[FD] = ExtImage;

  return &TexCache[FD];
}

