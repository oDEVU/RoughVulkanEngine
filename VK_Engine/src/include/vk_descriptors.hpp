#pragma once

#include "vk_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace rvk {

class VK_DescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(VK_Device &vk_device) : vk_device{vk_device} {}

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    std::unique_ptr<VK_DescriptorSetLayout> build() const;

   private:
    VK_Device &vk_device;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  VK_DescriptorSetLayout(
      VK_Device &vk_device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~VK_DescriptorSetLayout();
  VK_DescriptorSetLayout(const VK_DescriptorSetLayout &) = delete;
  VK_DescriptorSetLayout &operator=(const VK_DescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

 private:
  VK_Device &vk_device;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class VK_DescriptorWriter;
};

class VK_DescriptorPool {
 public:
  class Builder {
   public:
    Builder(VK_Device &vk_device) : vk_device{vk_device} {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<VK_DescriptorPool> build() const;

   private:
    VK_Device &vk_device;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  VK_DescriptorPool(
      VK_Device &vk_device,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~VK_DescriptorPool();
  VK_DescriptorPool(const VK_DescriptorPool &) = delete;
  VK_DescriptorPool &operator=(const VK_DescriptorPool &) = delete;

  bool allocateDescriptor(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

 private:
  VK_Device &vk_device;
  VkDescriptorPool descriptorPool;

  friend class VK_DescriptorWriter;
};

class VK_DescriptorWriter {
 public:
  VK_DescriptorWriter(VK_DescriptorSetLayout &setLayout, VK_DescriptorPool &pool);

  VK_DescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  VK_DescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  VK_DescriptorSetLayout &setLayout;
  VK_DescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

}  // namespace VK_