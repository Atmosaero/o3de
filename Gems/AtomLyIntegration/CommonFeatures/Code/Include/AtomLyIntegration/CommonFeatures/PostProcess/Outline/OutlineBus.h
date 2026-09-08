/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Atom/Feature/PostProcess/Outline/OutlineConstants.h>
#include <AzCore/Component/Component.h>

namespace AZ
{
    namespace Render
    {
        class OutlineRequests : public ComponentBus
        {
        public:
            AZ_RTTI(AZ::Render::OutlineRequests, "{7248C6AA-72C2-4A44-8F87-4E5F2A710539}");

            /// Overrides the default AZ::EBusTraits handler policy to allow one listener only.
            static const EBusHandlerPolicy HandlerPolicy = EBusHandlerPolicy::Single;
            virtual ~OutlineRequests()
            {
            }

            // Auto-gen virtual getters/setters...
            // clang-format off
#include <Atom/Feature/ParamMacros/StartParamFunctionsVirtual.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
            // clang-format on
        };

        typedef AZ::EBus<OutlineRequests> OutlineRequestBus;
    } // namespace Render
} // namespace AZ
