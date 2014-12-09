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

#include <pthread.h>

#include <string>
#include <vector>

#include <stout/check.hpp>
#include <stout/foreach.hpp>
#include <stout/nothing.hpp>
#include <stout/strings.hpp>
#include <stout/try.hpp>

#include "common/lock.hpp"
#include "hook/hook.hpp"
#include "hook/manager.hpp"
#include "module/hook.hpp"
#include "module/manager.hpp"

using std::string;
using std::vector;

using mesos::modules::ModuleManager;

namespace mesos {
namespace internal {

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static hashmap<HookManager::Hooks, vector<Hook*>> availableHooks;


Try<Nothing> HookManager::initialize(const string& hookList)
{
  Lock lock(&mutex);

  vector<string> hooks = strings::split(hookList, ",");
  foreach (const string& hook, hooks) {
    if (!ModuleManager::contains<Hook>(hook)) {
      return Error("No hook module named '" + hook + "' available");
    }
    Try<Hook*> module = ModuleManager::create<Hook>(hook);
    if (!module.isSome()) {
      return Error(
          "Failed to instantiate hook module '" + hook + "': " +
          module.error());
    }

    if (module.get()->masterLaunchTaskLabelDecorator != NULL) {
      availableHooks[MASTER_LAUNCH_TASK_LABEL_DECORATOR]
        .push_back(module.get());
    }
    if (module.get()->slaveLaunchExecutorEnvironmentDecorator != NULL) {
      availableHooks[SLAVE_LAUNCH_EXECUTOR_ENVIRONMENT_DECORATOR]
        .push_back(module.get());
    }
    if (module.get()->slaveRemoveExecutorHook != NULL) {
      availableHooks[SLAVE_REMOVE_EXECUTOR_HOOK].push_back(module.get());
    }
  }
  return Nothing();
}


Labels HookManager::masterLaunchTaskLabelDecorator(
    const TaskInfo& taskInfo,
    const FrameworkInfo& frameworkInfo,
    const SlaveInfo& slaveInfo)
{
  Labels labels;
  if (!availableHooks.contains(MASTER_LAUNCH_TASK_LABEL_DECORATOR)) {
    return labels;
  }

  foreach (Hook* hook, availableHooks[MASTER_LAUNCH_TASK_LABEL_DECORATOR]) {
    labels.MergeFrom(hook->masterLaunchTaskLabelDecorator(
          taskInfo,
          frameworkInfo,
          slaveInfo));
  }
  return labels;
}


Option<Environment> HookManager::slaveLaunchExecutorEnvironmentDecorator(
      const ExecutorInfo& executorInfo,
      const TaskInfo& taskInfo)
{
  if (!availableHooks.contains(SLAVE_LAUNCH_EXECUTOR_ENVIRONMENT_DECORATOR)) {
    return None();
  }

  Environment environment;
  foreach (
      Hook* hook,
      availableHooks[SLAVE_LAUNCH_EXECUTOR_ENVIRONMENT_DECORATOR]) {
    Environment result =
      hook->slaveLaunchExecutorEnvironmentDecorator(executorInfo, taskInfo);
    environment.MergeFrom(result);
  }
  return environment;
}


void HookManager::slaveRemoveExecutorHook(
    const FrameworkInfo& frameworkInfo,
    const ExecutorInfo& executorInfo)
{
  if (!availableHooks.contains(SLAVE_REMOVE_EXECUTOR_HOOK)) {
    return;
  }

  foreach (Hook* hook, availableHooks[SLAVE_REMOVE_EXECUTOR_HOOK]) {
    hook->slaveRemoveExecutorHook(frameworkInfo, executorInfo);
  }
}

} // namespace internal {
} // namespace mesos {
