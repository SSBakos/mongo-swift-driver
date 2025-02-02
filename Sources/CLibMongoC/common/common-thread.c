/*
 * Copyright 2020-present MongoDB, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "CLibMongoC_common-thread-private.h"

#if defined(BSON_OS_UNIX)
int COMMON_PREFIX (thread_create) (bson_thread_t *thread,
                                   BSON_THREAD_FUN_TYPE (func),
                                   void *arg)
{
   return pthread_create (thread, NULL, func, arg);
}
int COMMON_PREFIX (thread_join) (bson_thread_t thread)
{
   return pthread_join (thread, NULL);
}

#if defined(MONGOC_ENABLE_DEBUG_ASSERTIONS) && defined(BSON_OS_UNIX)
bool COMMON_PREFIX (mutex_is_locked) (bson_mutex_t *mutex)
{
   return mutex->valid_tid &&
          pthread_equal (pthread_self (), mutex->lock_owner);
}
#endif

#else
int COMMON_PREFIX (thread_create) (bson_thread_t *thread,
                                   BSON_THREAD_FUN_TYPE (func),
                                   void *arg)
{
   *thread = (HANDLE) _beginthreadex (NULL, 0, func, arg, 0, NULL);
   if (0 == *thread) {
      return 1;
   }
   return 0;
}
int COMMON_PREFIX (thread_join) (bson_thread_t thread)
{
   int ret;

   /* zero indicates success for WaitForSingleObject. */
   ret = WaitForSingleObject (thread, INFINITE);
   if (WAIT_OBJECT_0 != ret) {
      return ret;
   }
   /* zero indicates failure for CloseHandle. */
   ret = CloseHandle (thread);
   if (0 == ret) {
      return 1;
   }
   return 0;
}
#endif
