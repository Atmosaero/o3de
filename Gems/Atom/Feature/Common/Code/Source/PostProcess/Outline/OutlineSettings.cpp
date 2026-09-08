/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <PostProcess/Outline/OutlineSettings.h>
#include <PostProcess/PostProcessSettings.h>

namespace AZ
{
    namespace Render
    {
        OutlineSettings::OutlineSettings(PostProcessFeatureProcessor* featureProcessor)
            : PostProcessBase(featureProcessor)
        {
        }

        void OutlineSettings::OnConfigChanged()
        {
            if (m_parentSettings)
            {
                m_parentSettings->OnConfigChanged();
            }
        }

        void OutlineSettings::ApplySettingsTo(OutlineSettings* target, float alpha) const
        {
            AZ_Assert(target != nullptr, "OutlineSettings::ApplySettingsTo called with nullptr as argument.");

            // Auto-gen code to blend individual params based on their override value onto target settings
#define OVERRIDE_TARGET target
#define OVERRIDE_ALPHA alpha
            // clang-format off
#include <Atom/Feature/ParamMacros/StartOverrideBlend.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
// clang-format on
#undef OVERRIDE_TARGET
#undef OVERRIDE_ALPHA
        }

    } // namespace Render
} // namespace AZ
