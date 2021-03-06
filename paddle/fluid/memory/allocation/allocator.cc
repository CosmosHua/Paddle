// Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "paddle/fluid/memory/allocation/allocator.h"

#include <functional>

namespace paddle {
namespace memory {
namespace allocation {
Allocation::~Allocation() {}

Allocator::~Allocator() {}

bool Allocator::IsAllocThreadSafe() const { return false; }

AllocationPtr Allocator::Allocate(size_t size, Allocator::Attr attr) {
  auto ptr = AllocateImpl(size, attr);
  ptr->RegisterDecoratedAllocator(this);
  return AllocationPtr(ptr);
}

void Allocator::FreeImpl(Allocation* allocation) {
  Allocator* allocator = allocation->TopDecoratedAllocator();
  allocator->Free(allocation);
}

void Allocator::Free(Allocation* allocation) {
  allocation->PopDecoratedAllocator();
  FreeImpl(allocation);
}

const char* BadAlloc::what() const noexcept { return msg_.c_str(); }

void AllocationDeleter::operator()(Allocation* allocation) const {
  Allocator* allocator = allocation->TopDecoratedAllocator();
  allocator->Free(allocation);
}

}  // namespace allocation
}  // namespace memory
}  // namespace paddle
