#include "vk_descriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace rvk {

// *************** Descriptor Set Layout Builder *********************

VK_DescriptorSetLayout::Builder &VK_DescriptorSetLayout::Builder::addBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count) {
  assert(bindings.count(binding) == 0 && "Binding already in use");
  VkDescriptorSetLayoutBinding layoutBinding{};
  layoutBinding.binding = binding;
  layoutBinding.descriptorType = descriptorType;
  layoutBinding.descriptorCount = count;
  layoutBinding.stageFlags = stageFlags;
  bindings[binding] = layoutBinding;
  return *this;
}

std::unique_ptr<VK_DescriptorSetLayout> VK_DescriptorSetLayout::Builder::build() const {
  return std::make_unique<VK_DescriptorSetLayout>(vk_device, bindings);
}

// *************** Descriptor Set Layout *********************

VK_DescriptorSetLayout::VK_DescriptorSetLayout(
    VK_Device &vk_device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : vk_device{vk_device}, bindings{bindings} {
  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
  for (auto kv : bindings) {
    setLayoutBindings.push_back(kv.second);
  }

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
  descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
  descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

  if (vkCreateDescriptorSetLayout(
          vk_device.device(),
          &descriptorSetLayoutInfo,
          nullptr,
          &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

VK_DescriptorSetLayout::~VK_DescriptorSetLayout() {
  vkDestroyDescriptorSetLayout(vk_device.device(), descriptorSetLayout, nullptr);
}

// *************** Descriptor Pool Builder *********************

VK_DescriptorPool::Builder &VK_DescriptorPool::Builder::addPoolSize(
    VkDescriptorType descriptorType, uint32_t count) {
  poolSizes.push_back({descriptorType, count});
  return *this;
}

VK_DescriptorPool::Builder &VK_DescriptorPool::Builder::setPoolFlags(
    VkDescriptorPoolCreateFlags flags) {
  poolFlags = flags;
  return *this;
}
VK_DescriptorPool::Builder &VK_DescriptorPool::Builder::setMaxSets(uint32_t count) {
  maxSets = count;
  return *this;
}

std::unique_ptr<VK_DescriptorPool> VK_DescriptorPool::Builder::build() const {
  return std::make_unique<VK_DescriptorPool>(vk_device, maxSets, poolFlags, poolSizes);
}

// *************** Descriptor Pool *********************

VK_DescriptorPool::VK_DescriptorPool(
    VK_Device &vk_device,
    uint32_t maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    const std::vector<VkDescriptorPoolSize> &poolSizes)
    : vk_device{vk_device} {
  VkDescriptorPoolCreateInfo descriptorPoolInfo{};
  descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  descriptorPoolInfo.pPoolSizes = poolSizes.data();
  descriptorPoolInfo.maxSets = maxSets;
  descriptorPoolInfo.flags = poolFlags;

  if (vkCreateDescriptorPool(vk_device.device(), &descriptorPoolInfo, nullptr, &descriptorPool) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

VK_DescriptorPool::~VK_DescriptorPool() {
  vkDestroyDescriptorPool(vk_device.device(), descriptorPool, nullptr);
}

bool VK_DescriptorPool::allocateDescriptor(
    const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const {
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.pSetLayouts = &descriptorSetLayout;
  allocInfo.descriptorSetCount = 1;

  // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
  // a new pool whenever an old pool fills up. But this is beyond our current scope
  if (vkAllocateDescriptorSets(vk_device.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
    return false;
  }
  return true;
}

void VK_DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const {
  vkFreeDescriptorSets(
      vk_device.device(),
      descriptorPool,
      static_cast<uint32_t>(descriptors.size()),
      descriptors.data());
}

void VK_DescriptorPool::resetPool() {
  vkResetDescriptorPool(vk_device.device(), descriptorPool, 0);
}

// *************** Descriptor Writer *********************

VK_DescriptorWriter::VK_DescriptorWriter(VK_DescriptorSetLayout &setLayout, VK_DescriptorPool &pool)
    : setLayout{setLayout}, pool{pool} {}

VK_DescriptorWriter &VK_DescriptorWriter::writeBuffer(
    uint32_t binding, VkDescriptorBufferInfo *bufferInfo) {
  assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

  auto &bindingDescription = setLayout.bindings[binding];

  assert(
      bindingDescription.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pBufferInfo = bufferInfo;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

VK_DescriptorWriter &VK_DescriptorWriter::writeImage(
    uint32_t binding, VkDescriptorImageInfo *imageInfo) {
  assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

  auto &bindingDescription = setLayout.bindings[binding];

  assert(
      bindingDescription.descriptorCount == 1 &&
      "Binding single descriptor info, but binding expects multiple");

  VkWriteDescriptorSet write{};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.descriptorType = bindingDescription.descriptorType;
  write.dstBinding = binding;
  write.pImageInfo = imageInfo;
  write.descriptorCount = 1;

  writes.push_back(write);
  return *this;
}

bool VK_DescriptorWriter::build(VkDescriptorSet &set) {
  bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
  if (!success) {
    return false;
  }
  overwrite(set);
  return true;
}

void VK_DescriptorWriter::overwrite(VkDescriptorSet &set) {
  for (auto &write : writes) {
    write.dstSet = set;
  }
  vkUpdateDescriptorSets(pool.vk_device.device(), writes.size(), writes.data(), 0, nullptr);
}

}  // namespace VK_
