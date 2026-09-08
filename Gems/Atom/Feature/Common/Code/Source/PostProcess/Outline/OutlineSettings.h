/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Atom/Feature/PostProcess/Outline/OutlineSettingsInterface.h>
#include <PostProcess/PostProcessBase.h>

namespace AZ
{
    namespace Render
    {
        class PostProcessSettings;

        class OutlineSettings final
            : public OutlineSettingsInterface
            , public PostProcessBase
        {
            friend class PostProcessSettings;
            friend class PostProcessFeatureProcessor;

        public:
            AZ_RTTI(
                AZ::Render::OutlineSettings,
                "{8290D74D-692E-42BF-9E68-988E09AD1459}",
                AZ::Render::OutlineSettingsInterface,
                AZ::Render::PostProcessBase);
            AZ_CLASS_ALLOCATOR(OutlineSettings, SystemAllocator, 0);

            OutlineSettings(PostProcessFeatureProcessor* featureProcessor);
            ~OutlineSettings() = default;

            // OutlineSettingsInterface overrides...
            void OnConfigChanged() override;

            // Applies settings from this onto target using override settings and passed alpha value for blending
            void ApplySettingsTo(OutlineSettings* target, float alpha) const;

            // Generate getters and setters.
            // clang-format off
#include <Atom/Feature/ParamMacros/StartParamFunctionsOverrideImpl.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
            // clang-format on

        private:
            // Generate members...
            // clang-format off
#include <Atom/Feature/ParamMacros/StartParamMembers.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
            // clang-format on

            void Simulate([[maybe_unused]] float deltaTime)
            {
            }

            PostProcessSettings* m_parentSettings = nullptr;
        };

    } // namespace Render
} // namespace AZ
