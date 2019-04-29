// Copyright 2018 The gVisor Authors.
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

#include <stdio.h>
#include <sys/un.h>
#include "gtest/gtest.h"
#include "gtest/gtest.h"
#include "test/syscalls/linux/socket_test_util.h"
#include "test/syscalls/linux/unix_domain_socket_test_util.h"
#include "test/util/test_util.h"

namespace gvisor {
namespace testing {

namespace {

// Test fixture for tests that apply to pairs of connected non-blocking dgram
// unix sockets.
using NonBlockingDgramUnixSocketPairTest = SocketPairTest;

TEST_P(NonBlockingDgramUnixSocketPairTest, ReadOneSideClosed) {
  if (IsRunningOnGvisor()) {
    // FIXME(b/70803293): gVisor datagram sockets return 0 instead of
    // EAGAIN.
    return;
  }

  auto sockets = ASSERT_NO_ERRNO_AND_VALUE(NewSocketPair());
  ASSERT_THAT(close(sockets->release_first_fd()), SyscallSucceeds());
  char data[10] = {};
  ASSERT_THAT(read(sockets->second_fd(), data, sizeof(data)),
              SyscallFailsWithErrno(EAGAIN));
}

INSTANTIATE_TEST_SUITE_P(
    AllUnixDomainSockets, NonBlockingDgramUnixSocketPairTest,
    ::testing::ValuesIn(IncludeReversals(std::vector<SocketPairKind>{
        UnixDomainSocketPair(SOCK_DGRAM | SOCK_NONBLOCK),
        FilesystemBoundUnixDomainSocketPair(SOCK_DGRAM | SOCK_NONBLOCK),
        AbstractBoundUnixDomainSocketPair(SOCK_DGRAM | SOCK_NONBLOCK),
    })));

}  // namespace

}  // namespace testing
}  // namespace gvisor
