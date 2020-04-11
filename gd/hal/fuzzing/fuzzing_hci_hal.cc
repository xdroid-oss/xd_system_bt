/*
 * Copyright 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hal/fuzzing/fuzzing_hci_hal.h"
#include "fuzzing/helpers.h"
#include "hci/hci_packets.h"

namespace bluetooth {
namespace hal {
namespace fuzzing {

void FuzzingHciHal::registerIncomingPacketCallback(HciHalCallbacks* callbacks) {
  callbacks_ = callbacks;
}

void FuzzingHciHal::unregisterIncomingPacketCallback() {
  callbacks_ = nullptr;
}

int FuzzingHciHal::injectFuzzInput(const uint8_t* data, size_t size) {
  const uint8_t separator[] = {0xDE, 0xAD, 0xBE, 0xEF};
  auto inputs = ::bluetooth::fuzzing::SplitInput(data, size, separator, sizeof(separator));
  for (auto const& sdata : inputs) {
    auto packet = packet::PacketView<packet::kLittleEndian>(std::make_shared<std::vector<uint8_t>>(sdata));
    hci::AclPacketView aclPacket = hci::AclPacketView::Create(packet);
    if (!aclPacket.IsValid()) {
      continue;
    }

    callbacks_->aclDataReceived(sdata);
    sentinel_work_item_.WaitUntilFinishedOn(GetHandler());
  }
  return 0;
}

}  // namespace fuzzing
}  // namespace hal
}  // namespace bluetooth