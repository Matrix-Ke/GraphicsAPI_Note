#include "VulkanSetup.h"

VkAllocationCallbacks* VulkanSetup::g_Allocator = NULL;
VkInstance					VulkanSetup::g_Instance = VK_NULL_HANDLE;
VkPhysicalDevice			VulkanSetup::g_PhysicalDevice = VK_NULL_HANDLE;
VkDevice					VulkanSetup::g_Device = VK_NULL_HANDLE;
uint32_t					VulkanSetup::g_QueueFamily = (uint32_t)-1;
VkQueue						VulkanSetup::g_Queue = VK_NULL_HANDLE;
VkDebugReportCallbackEXT	VulkanSetup::g_DebugReport = VK_NULL_HANDLE;
VkPipelineCache				VulkanSetup::g_PipelineCache = VK_NULL_HANDLE;
VkDescriptorPool			VulkanSetup::g_DescriptorPool = VK_NULL_HANDLE;
ImGui_ImplVulkanH_Window	VulkanSetup::g_MainWindowData;
int							VulkanSetup::g_MinImageCount = 2;
bool						VulkanSetup::g_SwapChainRebuild = false;