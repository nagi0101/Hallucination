// Copyright (C) 2018-2023 Mercuna Developments Limited - All rights reserved
// This source file is part of the Mercuna Middleware
// Use, modification and distribution of this file or any portion thereof
// is only permitted as specified in your Mercuna License Agreement.
#pragma once

#include "Runtime/Launch/Resources/Version.h"
#include "Async/TaskGraphInterfaces.h"

namespace mercuna
{
    template<typename TPTR, typename T, typename... Args>
    inline void BroadcastOnGameThread(const TPTR& _objectPtr, const T& _delegate, Args&&... args)
    {
        if (_objectPtr.IsValid())
        {
            if (IsInGameThread() && _objectPtr->GetWorld()->GetActiveLevelCollection() == _objectPtr->GetLevel()->GetCachedLevelCollection())
            {
                _delegate.Broadcast(std::forward<Args>(args)...);
            }
            else
            {
                gMercunaEnv->pJobSystem->RunOnNextTick([_objectPtr, _delegate, args...]() { if (_objectPtr.IsValid()) _delegate.Broadcast(args...); });
            }
        }
    }

    inline void RunOnGameThread(TUniqueFunction<void()> InFunction)
    {
        if (IsInGameThread())
        {
            InFunction();
        }
        else
        {
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 25
            TGraphTask<FFunctionGraphTask>::CreateTask().ConstructAndDispatchWhenReady(MoveTemp(InFunction), TStatId(), ENamedThreads::GameThread);
#else
            TGraphTask<TFunctionGraphTaskImpl<void(), ESubsequentsMode::TrackSubsequents>>::CreateTask().ConstructAndDispatchWhenReady(MoveTemp(InFunction), TStatId(), ENamedThreads::GameThread);
#endif
        }
    }
}
