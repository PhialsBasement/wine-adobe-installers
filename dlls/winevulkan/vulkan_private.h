/* Wine Vulkan ICD private data structures
 *
 * Copyright 2017 Roderick Colenbrander
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __WINE_VULKAN_PRIVATE_H
#define __WINE_VULKAN_PRIVATE_H

#include <pthread.h>
#include <stdbool.h>

#include "vulkan_loader.h"
#include "vulkan_thunks.h"

#include "wine/rbtree.h"

extern const struct vulkan_funcs *vk_funcs;

struct wine_cmd_buffer
{
    VULKAN_OBJECT_HEADER( VkCommandBuffer, command_buffer );
    struct vulkan_device *device;
};

static inline struct wine_cmd_buffer *wine_cmd_buffer_from_handle(VkCommandBuffer handle)
{
    struct vulkan_client_object *client = (struct vulkan_client_object *)handle;
    return (struct wine_cmd_buffer *)(UINT_PTR)client->unix_handle;
}

struct wine_semaphore;

struct pending_d3d12_fence_op
{
    /* Vulkan native local semaphore. */
    struct local_timeline_semaphore local_sem;

    /* Operation values. */
    struct list entry;
    uint64_t virtual_value;
    uint64_t shared_physical_value;
    struct wine_semaphore *semaphore;
};

struct wine_debug_utils_messenger;

struct wine_debug_report_callback
{
    VULKAN_OBJECT_HEADER( VkDebugReportCallbackEXT, debug_callback );
    struct vulkan_instance *instance;

    UINT64 user_callback; /* client pointer */
    UINT64 user_data; /* client pointer */
};

struct wine_phys_dev
{
    struct vulkan_physical_device obj;

    VkPhysicalDeviceMemoryProperties memory_properties;
    VkExtensionProperties *extensions;
    uint32_t extension_count;

    uint32_t external_memory_align;
    uint32_t map_placed_align;
};

struct wine_debug_report_callback;

struct wine_instance
{
    struct vulkan_instance obj;

    VkBool32 enable_win32_surface;

    struct wine_debug_utils_messenger *utils_messengers;
    uint32_t utils_messenger_count;

    struct wine_debug_report_callback default_callback;

    unsigned int quirks;

    struct rb_tree objects;
    pthread_rwlock_t objects_lock;

    pthread_key_t transient_object_handle;

    /* We cache devices as we need to wrap them as they are dispatchable objects. */
    uint32_t phys_dev_count;
    struct wine_phys_dev phys_devs[];
};

C_ASSERT(sizeof(struct wine_instance) == offsetof(struct wine_instance, phys_devs[0]));

struct wine_cmd_pool
{
    VULKAN_OBJECT_HEADER( VkCommandPool, command_pool );
};

static inline struct wine_cmd_pool *wine_cmd_pool_from_handle(VkCommandPool handle)
{
    struct vulkan_client_object *client = &command_pool_from_handle(handle)->obj;
    return (struct wine_cmd_pool *)(UINT_PTR)client->unix_handle;
}

struct keyed_mutex_shm
{
    pthread_mutex_t mutex;
    uint64_t instance_id_counter;
    uint64_t acquired_to_instance;
    uint64_t key;
    UINT64 timeline_value;
    uint64_t timeline_queued_release;
};

struct wine_device_memory
{
    VULKAN_OBJECT_HEADER( VkDeviceMemory, device_memory );
    VkDeviceSize size;
    void *vm_map;
    VkExternalMemoryHandleTypeFlagBits handle_types;
    BOOL inherit;
    DWORD access;
    HANDLE handle;
    struct keyed_mutex_shm *keyed_mutex_shm;
    VkSemaphore keyed_mutex_sem;
    uint64_t keyed_mutex_instance_id;
};

static inline struct wine_device_memory *wine_device_memory_from_handle(VkDeviceMemory handle)
{
    return (struct wine_device_memory *)(uintptr_t)handle;
}

struct wine_debug_utils_messenger
{
    VULKAN_OBJECT_HEADER( VkDebugUtilsMessengerEXT, debug_messenger );
    struct vulkan_instance *instance;

    UINT64 user_callback; /* client pointer */
    UINT64 user_data; /* client pointer */
};

static inline struct wine_debug_utils_messenger *wine_debug_utils_messenger_from_handle(
        VkDebugUtilsMessengerEXT handle)
{
    return (struct wine_debug_utils_messenger *)(uintptr_t)handle;
}

static inline struct wine_debug_report_callback *wine_debug_report_callback_from_handle(
        VkDebugReportCallbackEXT handle)
{
    return (struct wine_debug_report_callback *)(uintptr_t)handle;
}

BOOL wine_vk_device_extension_supported(const char *name);
BOOL wine_vk_instance_extension_supported(const char *name);
BOOL wine_vk_is_host_surface_extension(const char *name);

BOOL wine_vk_is_type_wrapped(VkObjectType type);

NTSTATUS init_vulkan(void *args);

NTSTATUS vk_is_available_instance_function(void *arg);
NTSTATUS vk_is_available_device_function(void *arg);
NTSTATUS vk_is_available_instance_function32(void *arg);
NTSTATUS vk_is_available_device_function32(void *arg);

struct conversion_context
{
    char buffer[2048];
    uint32_t used;
    struct list alloc_entries;
};

static inline void init_conversion_context(struct conversion_context *pool)
{
    pool->used = 0;
    list_init(&pool->alloc_entries);
}

static inline void free_conversion_context(struct conversion_context *pool)
{
    struct list *entry, *next;
    LIST_FOR_EACH_SAFE(entry, next, &pool->alloc_entries)
        free(entry);
}

struct wine_semaphore
{
    struct vulkan_semaphore obj;
    VkSemaphore semaphore;

    VkExternalSemaphoreHandleTypeFlagBits export_types;

    /* mutable members */
    VkExternalSemaphoreHandleTypeFlagBits handle_type;
    struct list poll_entry;
    struct list pending_waits;
    struct list pending_signals;
    HANDLE handle;
    struct
    {
        /* Shared mem access mutex. The non-shared parts access is guarded with device global signaller_mutex. */
        pthread_mutex_t mutex;
        UINT64 virtual_value, physical_value;
        uint64_t last_reset_physical;
        uint64_t last_dropped_reset_physical;
        struct
        {
            uint64_t physical_at_reset;
            uint64_t virtual_before_reset;
        }
        reset_backlog[16];
        uint32_t reset_backlog_count;
    } *d3d12_fence_shm;
    /* The Vulkan shared semaphore is only waited or signaled in signaller_worker(). */
    VkSemaphore fence_timeline_semaphore;
};

static inline void *conversion_context_alloc(struct conversion_context *pool, size_t size)
{
    if (pool->used + size <= sizeof(pool->buffer))
    {
        void *ret = pool->buffer + pool->used;
        pool->used += (size + sizeof(UINT64) - 1) & ~(sizeof(UINT64) - 1);
        return ret;
    }
    else
    {
        struct list *entry;
        if (!(entry = malloc(sizeof(*entry) + size)))
            return NULL;
        list_add_tail(&pool->alloc_entries, entry);
        return entry + 1;
    }
}

struct wine_deferred_operation
{
    VULKAN_OBJECT_HEADER( VkDeferredOperationKHR, deferred_operation );
    struct conversion_context ctx; /* to keep params alive. */
};

static inline struct wine_deferred_operation *wine_deferred_operation_from_handle(
        VkDeferredOperationKHR handle)
{
    return (struct wine_deferred_operation *)(uintptr_t)handle;
}

typedef UINT32 PTR32;

typedef struct
{
    VkStructureType sType;
    PTR32 pNext;
} VkBaseInStructure32;

typedef struct
{
    VkStructureType sType;
    PTR32 pNext;
} VkBaseOutStructure32;

static inline void *find_next_struct32(void *s, VkStructureType t)
{
    VkBaseOutStructure32 *header;

    for (header = s; header; header = UlongToPtr(header->pNext))
    {
        if (header->sType == t)
            return header;
    }

    return NULL;
}

static inline void *find_next_struct(const void *s, VkStructureType t)
{
    VkBaseOutStructure *header;

    for (header = (VkBaseOutStructure *)s; header; header = header->pNext)
    {
        if (header->sType == t)
            return header;
    }

    return NULL;
}

static inline void init_unicode_string( UNICODE_STRING *str, const WCHAR *data )
{
    str->Length = lstrlenW(data) * sizeof(WCHAR);
    str->MaximumLength = str->Length + sizeof(WCHAR);
    str->Buffer = (WCHAR *)data;
}

#define MEMDUP(ctx, dst, src, count) dst = conversion_context_alloc((ctx), sizeof(*(dst)) * (count)); \
    memcpy((void *)(dst), (src), sizeof(*(dst)) * (count));
#define MEMDUP_VOID(ctx, dst, src, size) dst = conversion_context_alloc((ctx), size); \
    memcpy((void *)(dst), (src), size);

#endif /* __WINE_VULKAN_PRIVATE_H */
