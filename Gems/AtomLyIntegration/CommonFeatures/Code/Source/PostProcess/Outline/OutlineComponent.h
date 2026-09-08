/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Atom/Feature/PostProcess/Outline/OutlineConstants.h>
#include <AtomLyIntegration/CommonFeatures/PostProcess/Outline/OutlineComponentConfig.h>
#include <AzCore/Component/Component.h>
#include <AzFramework/Components/ComponentAdapter.h>
#include <PostProcess/Outline/OutlineComponentController.h>

namespace AZ
{
    namespace Render
    {
        namespace Outline
        {
            static constexpr const char* const OutlineComponentTypeId = "{20EB0127-CB75-48A0-BC95-49384EA8F274}";
        }

        class OutlineComponent final : public AzFramework::Components::ComponentAdapter<OutlineComponentController, OutlineComponentConfig>
        {
        public:
            using BaseClass = AzFramework::Components::ComponentAdapter<OutlineComponentController, OutlineComponentConfig>;
            AZ_COMPONENT(AZ::Render::OutlineComponent, Outline::OutlineComponentTypeId, BaseClass);

            OutlineComponent() = default;
            OutlineComponent(const OutlineComponentConfig& config);

            static void Reflect(AZ::ReflectContext* context);
        };
    } // namespace Render
} // namespace AZ
