/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __HOOK_HOOK_HPP__
#define __HOOK_HOOK_HPP__

#include <mesos/mesos.hpp>

#include "master/master.hpp"
#include "slave/slave.hpp"

namespace mesos {
namespace internal {

class Hook
{
public:
  Hook()
    : masterLaunchTaskLabelDecorator(NULL),
      slaveLaunchExecutorEnvironmentDecorator(NULL),
      slaveRemoveExecutorHook(NULL)
  {}

  Labels (*masterLaunchTaskLabelDecorator)(
      const TaskInfo& taskInfo,
      const FrameworkInfo& frameworkInfo,
      const SlaveInfo& slaveInfo);

  Environment (*slaveLaunchExecutorEnvironmentDecorator)(
      const ExecutorInfo& executorInfo,
      const TaskInfo& taskInfo);

  void (*slaveRemoveExecutorHook)(
      const FrameworkInfo& frameworkInfo,
      const ExecutorInfo& executorInfo);
};

} // namespace internal {
} // namespace mesos {

#endif // __HOOK_HOOK_HPP__
