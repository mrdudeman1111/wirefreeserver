#pragma once

#include <cstdint>
#include <map>
#include <iostream>
#include <string>

#include <vulkan/vulkan.h>

struct Texture
{
  public:
    VkImage Image;
    VkDeviceMemory Memory;

    /* this is a Handle to the external texture to present, it can be a windows HANDLE, or a linux File descriptor, or a DX11/12 Texture ID
       it's really just a system dependent UUID. Check out OGL/Directx/Vulkan Texture Interop for more insight on how this works*/
    uint64_t SharedHandle;
};

class VkBackend
{
public:
    VkBackend(uint32_t inWidth, uint32_t inHeight);
    ~VkBackend();

    /// Eventually I will add Params Height, Width, BitRate, Networking Protocol.
    void Init();

    void Cleanup();

    int GetMemIndex(uint32_t MemType);

    Texture* GetImageFromFD(uint64_t FD);

    void SyncImage(uint64_t FD);

private:
    std::map<uint64_t, Texture> TexCache;

    uint32_t Width;
    uint32_t Height;

    VkDevice Device = VK_NULL_HANDLE;
    VkPhysicalDevice PDev = VK_NULL_HANDLE;
    VkInstance Instance = VK_NULL_HANDLE;
};

