// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/task_scheduler/task_scheduler.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/task_scheduler/scheduler_worker_pool_params.h"
#include "base/task_scheduler/task_scheduler_impl.h"
#include "base/threading/platform_thread.h"
#include "base/time/time.h"

namespace base {

namespace {

// |g_task_scheduler| is intentionally leaked on shutdown.
TaskScheduler* g_task_scheduler = nullptr;

}  // namespace

// static
void TaskScheduler::CreateAndSetSimpleTaskScheduler(int max_threads) {
  std::vector<SchedulerWorkerPoolParams> worker_pool_params_vector;
  worker_pool_params_vector.emplace_back(
      "Simple", ThreadPriority::NORMAL,
      SchedulerWorkerPoolParams::StandbyThreadPolicy::LAZY, max_threads,
      TimeDelta::FromSeconds(30));
  CreateAndSetDefaultTaskScheduler(
      worker_pool_params_vector,
      Bind([](const TaskTraits&) -> size_t { return 0; }));
}

// static
void TaskScheduler::CreateAndSetDefaultTaskScheduler(
    const std::vector<SchedulerWorkerPoolParams>& worker_pool_params_vector,
    const WorkerPoolIndexForTraitsCallback&
        worker_pool_index_for_traits_callback) {
  SetInstance(internal::TaskSchedulerImpl::Create(
      worker_pool_params_vector, worker_pool_index_for_traits_callback));
}

// static
void TaskScheduler::SetInstance(std::unique_ptr<TaskScheduler> task_scheduler) {
  delete g_task_scheduler;
  g_task_scheduler = task_scheduler.release();
}

// static
TaskScheduler* TaskScheduler::GetInstance() {
  return g_task_scheduler;
}

}  // namespace base
