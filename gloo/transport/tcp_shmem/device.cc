/**
 * Copyright (c) 2017-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "gloo/transport/tcp_shmem/device.h"
#include "gloo/transport/tcp/device.h"

#include <array>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>


#include "gloo/common/linux.h"
#include "gloo/common/logging.h"
#include "gloo/common/error.h"
#include "gloo/transport/tcp_shmem/context.h"
#include "gloo/transport/tcp_shmem/pair.h"

namespace gloo {
namespace transport {
namespace tcp_shmem {

std::shared_ptr<transport::Device> CreateDevice(const struct transport::tcp::attr& src, const std::string& shmem_path) {
  auto device = std::make_shared<Device>(::gloo::transport::tcp::CreateDeviceAttr(src), shmem_path);
  return std::shared_ptr<transport::Device>(device);
}

Device::Device(const struct transport::tcp::attr& attr, const std::string shmem_path)
    : attr_(attr),
      loop_(std::make_shared<transport::tcp::Loop>()),
      interfaceName_(transport::tcp::sockaddrToInterfaceName(attr_)),
      interfaceSpeedMbps_(getInterfaceSpeedByName(interfaceName_)),
      pciBusID_(interfaceToBusID(interfaceName_)),
      shmem_path_(shmem_path) {
}

Device::~Device() {
}

std::string Device::str() const {
  std::stringstream ss;
  ss << "tcp";
  ss << ", pci=" << pciBusID_;
  ss << ", iface=" << interfaceName_;
  ss << ", speed=" << interfaceSpeedMbps_;
  ss << ", addr=" << tcp::Address(attr_.ai_addr).str();
  return ss.str();
}

const std::string& Device::getPCIBusID() const {
  return pciBusID_;
}

int Device::getInterfaceSpeed() const {
  return interfaceSpeedMbps_;
}

std::shared_ptr<transport::Context> Device::createContext(
    int rank, int size) {
  return std::shared_ptr<transport::Context>(
      new tcp_shmem::Context(shared_from_this(), rank, size, shmem_path_));
}

void Device::registerDescriptor(int fd, int events, tcp::Handler* h) {
  loop_->registerDescriptor(fd, events, h);
}

void Device::unregisterDescriptor(int fd, tcp::Handler* h) {
  loop_->unregisterDescriptor(fd, h);
}

} // namespace tcp_shmem
} // namespace transport
} // namespace gloo
