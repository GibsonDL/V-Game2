#pragma once

#include "vk_types.h"
#include <vector>
#include "vk_mem_alloc.h"
#include <deque>
#include <functional>
#include "vk_descriptors.h"



struct ComputePushConstants
{
    glm::vec4 data1;
    glm::vec4 data2;
    glm::vec4 data3;
    glm::vec4 data4;
};

struct ComputeEffect {
    const char* name;

    VkPipeline pipeline;
    VkPipelineLayout layout;

    ComputePushConstants data;
};

struct DeletionQueue
{
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        // reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)(); //call functors
        }

        deletors.clear();
    }
};
    
struct FrameData
{
    VkCommandPool _commandPool;
    VkCommandBuffer _mainCommandBuffer;
    
    VkSemaphore _swapchainSemaphore, _renderSemaphore;
    VkFence _renderFence;
    DeletionQueue _deletionQueue;
};

constexpr unsigned int FRAME_OVERLAP = 2;


class VulkanEngine {
public:
    
    AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
    void destroy_buffer(const AllocatedBuffer& buffer);
    GPUMeshBuffers uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);
    
    bool _isInitialized{ false };
    int _frameNumber {0};
    bool stop_rendering{false};
    VkExtent2D _windowExtent{ 1700 , 900 };

    struct SDL_Window* _window{ nullptr };

    VkInstance _instance;
    VkDebugUtilsMessengerEXT _debug_messenger;
    VkPhysicalDevice _chosenGPU;
    VkDevice _device;

    FrameData _frames[FRAME_OVERLAP];

    FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };


    VkQueue _graphicsQueue;
    uint32_t _graphicsQueueFamily;	

    VkSurfaceKHR _surface;
    VkSwapchainKHR _swapchain;
    VkFormat _swapchainImageFormat;
    VkExtent2D _swapchainExtent;
    VkExtent2D _drawExtent;

    DescriptorAllocator globalDescriptorAllocator;

    VkPipeline _gradientPipeline;
    VkPipelineLayout _gradientPipelineLayout;
  
    
    
    std::vector<VkFramebuffer> _framebuffers;
    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;

    VkDescriptorSet _drawImageDescriptors;
    VkDescriptorSetLayout _drawImageDescriptorLayout;

    DeletionQueue _mainDeletionQueue;

    VmaAllocator _allocator; //vma lib allocator
    // immediate submit structures
    VkFence _immFence;
    VkCommandBuffer _immCommandBuffer;
    VkCommandPool _immCommandPool;
    
    void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

    
   
    
    //draw resources
    AllocatedImage _drawImage;

    std::vector<ComputeEffect> backgroundEffects;

    int currentBackgroundEffect{ 0 };

    VkPipelineLayout _trianglePipelineLayout;
    VkPipeline _trianglePipeline;

    void init_triangle_pipeline();

    VkPipelineLayout _meshPipelineLayout;
    VkPipeline _meshPipeline;

    GPUMeshBuffers rectangle;

    void init_mesh_pipeline();




    
    void draw_geometry(VkCommandBuffer cmd);

    
    //initializes everything in the engine
    void init();

    //shuts down the engine
    void cleanup();

    //draw loop
    void draw();

    void draw_background(VkCommandBuffer cmd);
    //void draw_imgui(VkCommandBuffer cmd,  VkImageView targetImageView);
    void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);


    //run main loop
    void run();

   // void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

private:
    //void rebuild_swapchain();

    void init_vulkan();

    void init_swapchain();
    void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();

    void init_commands();

    void init_pipelines();
   void init_background_pipelines();
    void init_descriptors();

    void init_sync_structures();

    void init_imgui();

    void init_default_data();
   

  
};