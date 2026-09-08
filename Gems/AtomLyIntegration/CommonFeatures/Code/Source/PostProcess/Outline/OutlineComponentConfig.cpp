/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AtomLyIntegration/CommonFeatures/PostProcess/Outline/OutlineComponentConfig.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace AZ
{
    namespace Render
    {
        void OutlineComponentConfig::Reflect(ReflectContext* context)
        {
            if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serializeContext->Class<OutlineComponentConfig, ComponentConfig>()->Version(0)

                // Auto-gen serialize context code...
#define SERIALIZE_CLASS OutlineComponentConfig
                // clang-format off
#include <Atom/Feature/ParamMacros/StartParamSerializeContext.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
// clang-format on
#undef SERIALIZE_CLASS
                    ;
            }
        }

        void OutlineComponentConfig::CopySettingsFrom(OutlineSettingsInterface* settings)
        {
            if (!settings)
            {
                return;
            }

#define COPY_SOURCE settings
            // clang-format off
#include <Atom/Feature/ParamMacros/StartParamCopySettingsFrom.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
// clang-format on
#undef COPY_SOURCE
        }

        void OutlineComponentConfig::CopySettingsTo(OutlineSettingsInterface* settings)
        {
            if (!settings)
            {
                return;
            }

#define COPY_TARGET settings
            // clang-format off
#include <Atom/Feature/ParamMacros/StartParamCopySettingsTo.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
// clang-format on
#undef COPY_TARGET
        }

    } // namespace Render
} // namespace AZ
