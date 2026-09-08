/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Atom/Feature/PostProcess/Outline/OutlineSettingsInterface.h>
#include <AzCore/Component/Component.h>

namespace AZ
{
    namespace Render
    {
        class OutlineComponentConfig final : public ComponentConfig
        {
        public:
            AZ_RTTI(AZ::Render::OutlineComponentConfig, "{2D7D02EF-5B56-4096-842C-190C5601E84F}", AZ::ComponentConfig);

            static void Reflect(ReflectContext* context);

            // Generate members...
            // clang-format off
#include <Atom/Feature/ParamMacros/StartParamMembers.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
            // clang-format on

            // Generate Getters/Setters...
            // clang-format off
#include <Atom/Feature/ParamMacros/StartParamFunctions.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
            // clang-format on

            void CopySettingsFrom(OutlineSettingsInterface* settings);
            void CopySettingsTo(OutlineSettingsInterface* settings);

            bool ArePropertiesReadOnly() const
            {
                return !m_enabled;
            }
        };
    } // namespace Render
} // namespace AZ
