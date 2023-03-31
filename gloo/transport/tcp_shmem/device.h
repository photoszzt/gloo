/**
 * Copyright (c) 2017-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <array>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <gloo/transport/device.h>
#include <gloo/transport/tcp/attr.h>
#include <gloo/transport/tcp/loop.h>

namespace gloo {
namespace transport {
namespace tcp_shmem {

std::shared_ptr<::gloo::transport::Device> CreateDevice(
    const struct tcp::attr&, const std::string shmem_path);

// Forward declarations
class Pair;
class Buffer;

class Device : public ::gloo::transport::Device,
               public std::enable_shared_from_this<Device> {
 public:
  Device() = delete;
  explicit Device(const struct tcp::attr& attr, const std::string shmem_path);
  virtual ~Device();

  virtual std::string str() const override;

  virtual const std::string& getPCIBusID() const override;

  virtual int getInterfaceSpeed() const override;

  virtual std::shared_ptr<::gloo::transport::Context> createContext(
      int rank, int size) override;

  void registerDescriptor(int fd, int events, tcp::Handler* h);
  void unregisterDescriptor(int fd, tcp::Handler* h);

 protected:
  const struct tcp::attr attr_;

  friend class Pair;
  friend class Buffer;

 private:
  std::shared_ptr<tcp::Loop> loop_;

  std::string interfaceName_;
  int interfaceSpeedMbps_;
  std::string pciBusID_;
  std::string shmem_path_;
};

} // namespace tcp_shmem
} // namespace transport
} // namespace gloo
