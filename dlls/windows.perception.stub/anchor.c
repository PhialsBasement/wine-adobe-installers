/*
 * Copyright (C) 2025 Paul Gofman for CodeWeavers
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

#include "private.h"
#include "wine/debug.h"

#include "private.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(perception);

struct exporter
{
    IActivationFactory IActivationFactory_iface;
    ISpatialAnchorExporterStatics ISpatialAnchorExporterStatics_iface;
    LONG ref;
};

static inline struct exporter *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct exporter, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct exporter *impl = impl_from_IActivationFactory( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ISpatialAnchorExporterStatics ))
    {
        *out = &impl->ISpatialAnchorExporterStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct exporter *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct exporter *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl factory_vtbl =
{
    factory_QueryInterface,
    factory_AddRef,
    factory_Release,
    /* IInspectable methods */
    factory_GetIids,
    factory_GetRuntimeClassName,
    factory_GetTrustLevel,
    /* IActivationFactory methods */
    factory_ActivateInstance,
};

struct async_access_status
{
    IAsyncOperation_SpatialPerceptionAccessStatus IAsyncOperation_SpatialPerceptionAccessStatus_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
};

DEFINE_IINSPECTABLE(async_info, IAsyncInfo, struct async_access_status, IAsyncOperation_SpatialPerceptionAccessStatus_iface)

static HRESULT WINAPI async_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    TRACE( "iface %p, id %p.\n", iface, id );

    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    TRACE( "iface %p, status %p.\n", iface, status );

    *status = Completed;
    return S_OK;
}

static HRESULT WINAPI async_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    TRACE( "iface %p, error_code %p.\n", iface, error_code );

    *error_code = S_OK;
    return S_OK;
}

static HRESULT WINAPI async_info_Cancel( IAsyncInfo *iface )
{
    FIXME( "iface %p.\n", iface );

    return S_OK;
}

static HRESULT WINAPI async_info_Close( IAsyncInfo *iface )
{
    FIXME( "iface %p.\n", iface );

    return S_OK;
}

static const struct IAsyncInfoVtbl async_info_vtbl =
{
    /* IUnknown methods */
    async_info_QueryInterface,
    async_info_AddRef,
    async_info_Release,
    /* IInspectable methods */
    async_info_GetIids,
    async_info_GetRuntimeClassName,
    async_info_GetTrustLevel,
    /* IAsyncInfo */
    async_info_get_Id,
    async_info_get_Status,
    async_info_get_ErrorCode,
    async_info_Cancel,
    async_info_Close,
};


static inline struct async_access_status *impl_from_IAsyncOperation_SpatialPerceptionAccessStatus( IAsyncOperation_SpatialPerceptionAccessStatus *iface )
{
    return CONTAINING_RECORD( iface, struct async_access_status, IAsyncOperation_SpatialPerceptionAccessStatus_iface );
}

static HRESULT WINAPI async_SpatialPerceptionAccessStatus_QueryInterface( IAsyncOperation_SpatialPerceptionAccessStatus *iface, REFIID iid, void **out )
{
    struct async_access_status *impl = impl_from_IAsyncOperation_SpatialPerceptionAccessStatus( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_SpatialPerceptionAccessStatus ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperation_SpatialPerceptionAccessStatus_iface) );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        *out = &impl->IAsyncInfo_iface;
        IAsyncInfo_AddRef( &impl->IAsyncInfo_iface );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI async_SpatialPerceptionAccessStatus_AddRef( IAsyncOperation_SpatialPerceptionAccessStatus *iface )
{
    struct async_access_status *impl = impl_from_IAsyncOperation_SpatialPerceptionAccessStatus( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI async_SpatialPerceptionAccessStatus_Release( IAsyncOperation_SpatialPerceptionAccessStatus *iface )
{
    struct async_access_status *impl = impl_from_IAsyncOperation_SpatialPerceptionAccessStatus( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );

    if (!ref)
        free( impl );

    return ref;
}

static HRESULT WINAPI async_SpatialPerceptionAccessStatus_GetIids( IAsyncOperation_SpatialPerceptionAccessStatus *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_SpatialPerceptionAccessStatus_GetRuntimeClassName( IAsyncOperation_SpatialPerceptionAccessStatus *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<SpatialPerceptionAccessStatus>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<SpatialPerceptionAccessStatus>"),
                                class_name );
}

static HRESULT WINAPI async_SpatialPerceptionAccessStatus_GetTrustLevel( IAsyncOperation_SpatialPerceptionAccessStatus *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_SpatialPerceptionAccessStatus_put_Completed( IAsyncOperation_SpatialPerceptionAccessStatus *iface,
    IAsyncOperationCompletedHandler_SpatialPerceptionAccessStatus *handler )
{
    TRACE( "iface %p, handler %p.\n", iface, handler );

    IAsyncOperationCompletedHandler_SpatialPerceptionAccessStatus_AddRef( handler );
    IAsyncOperationCompletedHandler_SpatialPerceptionAccessStatus_Invoke( handler, iface, Completed );
    IAsyncOperationCompletedHandler_SpatialPerceptionAccessStatus_Release( handler );
    return S_OK;
}

static HRESULT WINAPI async_SpatialPerceptionAccessStatus_get_Completed( IAsyncOperation_SpatialPerceptionAccessStatus *iface,
    IAsyncOperationCompletedHandler_SpatialPerceptionAccessStatus **handler )
{
    TRACE( "iface %p, handler %p.\n", iface, handler );

    *handler = NULL;
    return S_OK;
}

static HRESULT WINAPI async_SpatialPerceptionAccessStatus_GetResults( IAsyncOperation_SpatialPerceptionAccessStatus *iface, SpatialPerceptionAccessStatus *results )
{
    TRACE( "iface %p, results %p.\n", iface, results );

    *results = SpatialPerceptionAccessStatus_DeniedBySystem;
    return S_OK;
}

static const struct IAsyncOperation_SpatialPerceptionAccessStatusVtbl async_SpatialPerceptionAccessStatus_vtbl =
{
    /* IUnknown methods */
    async_SpatialPerceptionAccessStatus_QueryInterface,
    async_SpatialPerceptionAccessStatus_AddRef,
    async_SpatialPerceptionAccessStatus_Release,
    /* IInspectable methods */
    async_SpatialPerceptionAccessStatus_GetIids,
    async_SpatialPerceptionAccessStatus_GetRuntimeClassName,
    async_SpatialPerceptionAccessStatus_GetTrustLevel,
    /* IAsyncOperation<SpatialPerceptionAccessStatus> */
    async_SpatialPerceptionAccessStatus_put_Completed,
    async_SpatialPerceptionAccessStatus_get_Completed,
    async_SpatialPerceptionAccessStatus_GetResults,
};

static HRESULT async_operation_SpatialPerceptionAccessStatus_create( IAsyncOperation_SpatialPerceptionAccessStatus **out )
{
    struct async_access_status *impl;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_SpatialPerceptionAccessStatus_iface.lpVtbl = &async_SpatialPerceptionAccessStatus_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_info_vtbl;
    impl->ref = 1;

    *out = &impl->IAsyncOperation_SpatialPerceptionAccessStatus_iface;
    TRACE( "created IAsyncOperation_SpatialPerceptionAccessStatus_iface %p\n", *out );
    return S_OK;
}

DEFINE_IINSPECTABLE( exporter_statics, ISpatialAnchorExporterStatics, struct exporter, IActivationFactory_iface )

static HRESULT WINAPI exporter_statics_GetDefault( ISpatialAnchorExporterStatics *iface, ISpatialAnchorExporter **value )
{
    FIXME( "iface %p, value %p stub.\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI exporter_statics_RequestAccessAsync( ISpatialAnchorExporterStatics *iface, IAsyncOperation_SpatialPerceptionAccessStatus **result )
{
    TRACE( "iface %p, result %p stub.\n", iface, result );
    return async_operation_SpatialPerceptionAccessStatus_create( result );
}

static const struct ISpatialAnchorExporterStaticsVtbl exporter_statics_vtbl =
{
    exporter_statics_QueryInterface,
    exporter_statics_AddRef,
    exporter_statics_Release,
    /* IInspectable methods */
    exporter_statics_GetIids,
    exporter_statics_GetRuntimeClassName,
    exporter_statics_GetTrustLevel,
    /* ISpatialAnchorExporterStatics methods */
    exporter_statics_GetDefault,
    exporter_statics_RequestAccessAsync,
};

static struct exporter exporter_statics =
{
    {&factory_vtbl},
    {&exporter_statics_vtbl},
    1,
};

IActivationFactory *anchor_exporter_factory = &exporter_statics.IActivationFactory_iface;
