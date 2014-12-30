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

#include <mesos/module.hpp>

#include <process/clock.hpp>
#include <process/future.hpp>
#include <process/gmock.hpp>
#include <process/owned.hpp>
#include <process/pid.hpp>

#include <stout/option.hpp>
#include <stout/os.hpp>
#include <stout/try.hpp>

#include "common/parse.hpp"
#include "hook/manager.hpp"
#include "module/manager.hpp"
#include "master/flags.hpp"
#include "master/master.hpp"
#include "slave/flags.hpp"
#include "slave/slave.hpp"

//#include "slave/containerizer/mesos/containerizer.hpp"

#include "tests/containerizer.hpp"
#include "tests/flags.hpp"
#include "tests/mesos.hpp"

using std::string;

using namespace mesos;
using namespace mesos::internal;
using namespace mesos::internal::tests;
using namespace mesos::modules;

using mesos::internal::master::Master;

using mesos::internal::slave::Slave;
//using mesos::internal::slave::Containerizer;
//using mesos::internal::slave::MesosContainerizer;
//using mesos::internal::slave::MesosContainerizerProcess;

//using process::Clock;
using process::Future;
//using process::Message;
using process::Owned;
using process::PID;

//using std::map;
using std::string;
using std::vector;

using testing::_;
//using testing::AtMost;
using testing::DoAll;
//using testing::Eq;
//using testing::Invoke;
//using testing::InvokeWithoutArgs;
using testing::Return;
using testing::SaveArg;

const char* HOOK_MODULE_LIBRARY_NAME = "testhook";
const char* HOOK_MODULE_NAME = "org_apache_mesos_TestHook";

const char* testLabelKey = "MESOS_Test_Label";
const char* testLabelValue = "ApacheMesos";
const char* testEnvironmentVariableName = "MESOS_TEST_ENVIRONMENT_VARIABLE";

class HookTest : public MesosTest {};


// static Modules getModules()
// {
//   Modules modules;
//   Modules::Library* library = modules.add_libraries();
//   library->set_file(path::join(
//       tests::flags.build_dir,
//       "src",
//       ".libs",
//       os::libraries::expandName(HOOK_MODULE_LIBRARY_NAME)));
//   Modules::Library::Module* module = library->add_modules();
//   module->set_name(HOOK_MODULE_NAME);
//   return modules;
// }


TEST_F(HookTest, VerifyMasterLaunchTaskHook)
{
  EXPECT_SOME(HookManager::initialize(HOOK_MODULE_NAME));

  Try<PID<Master> > master = StartMaster(CreateMasterFlags());
  ASSERT_SOME(master);

  TestContainerizer containerizer;

  StandaloneMasterDetector detector(master.get());

  MockSlave slave(CreateSlaveFlags(), &detector, &containerizer);
  process::spawn(slave);

  MockScheduler sched;
  MesosSchedulerDriver driver(
      &sched, DEFAULT_FRAMEWORK_INFO, master.get(), DEFAULT_CREDENTIAL);

  EXPECT_CALL(sched, registered(&driver, _, _))
    .Times(1);

  Future<vector<Offer> > offers;
  EXPECT_CALL(sched, resourceOffers(&driver, _))
    .WillOnce(FutureArg<1>(&offers))
    .WillRepeatedly(Return()); // Ignore subsequent offers.

  driver.start();

  AWAIT_READY(offers);
  EXPECT_NE(0u, offers.get().size());

  CommandInfo command;
  command.set_value("sleep 10");

  // Launch a task with the command executor.
  TaskInfo task;
  task.set_name("");
  task.mutable_task_id()->set_value("1");
  task.mutable_slave_id()->MergeFrom(offers.get()[0].slave_id());
  task.mutable_resources()->MergeFrom(offers.get()[0].resources());
  task.mutable_command()->MergeFrom(command);

  vector<TaskInfo> tasks;
  tasks.push_back(task);

  Future<Nothing> _runTask;
  TaskInfo taskInfo;
  EXPECT_CALL(slave, runTask(_, _, _, _, _))
    .Times(1)
    .WillOnce(DoAll(FutureSatisfy(&_runTask),
                    SaveArg<4>(&taskInfo)));

  driver.launchTasks(offers.get()[0].id(), tasks);

  AWAIT_READY(_runTask);

  Option<string> labelValue;
  foreach (const Label& label, taskInfo.labels().labels()) {
    if (label.key() == testLabelKey) {
      labelValue = label.value();
    }
  }
  EXPECT_SOME(labelValue);
  EXPECT_EQ(labelValue.get(), testLabelValue);

  driver.stop();
  driver.join();

  Shutdown(); // Must shutdown before 'containerizer' gets deallocated.
}


//TEST_F(HookTest, VerifySlaveLaunchExecutorHook)
//{
//  EXPECT_SOME(HookManager::initialize(HOOK_MODULE_NAME));
//
//  master::Flags masterFlags = CreateMasterFlags();
//
//  Try<PID<Master> > master = StartMaster(masterFlags);
//  ASSERT_SOME(master);
//
//  slave::Flags slaveFlags = CreateSlaveFlags();
//
//  MockExecutor exec(DEFAULT_EXECUTOR_ID);
//
//  TestContainerizer containerizer(&exec);
//
//  Try<PID<Slave> > slave = StartSlave(&containerizer);
//  ASSERT_SOME(slave);
//
//  MockScheduler sched;
//  MesosSchedulerDriver driver(
//      &sched, DEFAULT_FRAMEWORK_INFO, master.get(), DEFAULT_CREDENTIAL);
//
//  EXPECT_CALL(sched, registered(&driver, _, _))
//    .Times(1);
//
//  Future<vector<Offer> > offers;
//  EXPECT_CALL(sched, resourceOffers(&driver, _))
//    .WillOnce(FutureArg<1>(&offers))
//    .WillRepeatedly(Return()); // Ignore subsequent offers.
//
//  driver.start();
//
//  AWAIT_READY(offers);
//  EXPECT_NE(0u, offers.get().size());
//
//  // Launch a task with the command executor.
//  TaskInfo task;
//  task.set_name("");
//  task.mutable_task_id()->set_value("1");
//  task.mutable_slave_id()->MergeFrom(offers.get()[0].slave_id());
//  task.mutable_resources()->MergeFrom(offers.get()[0].resources());
//  task.mutable_executor()->MergeFrom(DEFAULT_EXECUTOR_INFO);
//
//  vector<TaskInfo> tasks;
//  tasks.push_back(task);
//
//  Future<Nothing> registered;
//  ExecutorInfo executorInfo;
//  EXPECT_CALL(exec, registered(_, _, _, _))
//    .Times(1)
//    .WillOnce(DoAll(FutureSatisfy(&registered),
//                    SaveArg<1>(&executorInfo)));
//
//  driver.launchTasks(offers.get()[0].id(), tasks);
//
//  AWAIT_READY(registered);
//
//  Option<string> path;
//  foreach (const Environment::Variable& variable,
//           executorInfo.command().environment().variables()) {
//    if (variable.name() == testEnvironmentVariableName) {
//      path = variable.value();
//      break;
//    }
//  }
//
//  EXPECT_SOME(path);
//  EXPECT_TRUE(os::isfile(path.get()));
//
//  driver.stop();
//  driver.join();
//
//  Shutdown(); // Must shutdown before 'containerizer' gets deallocated.
//
//  EXPECT_FALSE(os::isfile(path.get()));
//}
