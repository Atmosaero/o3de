/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Atom/Feature/PostProcess/Outline/OutlineConstants.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/RTTI/RTTI.h>

namespace AZ
{
    namespace Render
    {
        class OutlineSettingsInterface
        {
        public:
            AZ_RTTI(AZ::Render::OutlineSettingsInterface, "{0A319933-6930-4D2F-886D-5B7194EC17F6}");

            virtual ~OutlineSettingsInterface() = default;

            // Auto-gen virtual getter and setter functions...
            // clang-format off
#include <Atom/Feature/ParamMacros/StartParamFunctionsVirtual.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
            // clang-format on

            virtual void OnConfigChanged() = 0;
        };

    } // namespace Render
} // namespace AZ
