/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzToolsFramework/ToolsComponents/EditorComponentAdapter.h>
#include <PostProcess/Outline/OutlineComponent.h>

namespace AZ
{
    namespace Render
    {
        namespace Outline
        {
            static constexpr const char* const EditorOutlineComponentTypeId = "{84D85DFE-8B82-4A75-A83C-4B402D6F8C95}";
        }

        class EditorOutlineComponent final
            : public AzToolsFramework::Components::
                  EditorComponentAdapter<OutlineComponentController, OutlineComponent, OutlineComponentConfig>
        {
        public:
            using BaseClass =
                AzToolsFramework::Components::EditorComponentAdapter<OutlineComponentController, OutlineComponent, OutlineComponentConfig>;
            AZ_EDITOR_COMPONENT(AZ::Render::EditorOutlineComponent, Outline::EditorOutlineComponentTypeId, BaseClass);

            static void Reflect(AZ::ReflectContext* context);

            EditorOutlineComponent() = default;
            EditorOutlineComponent(const OutlineComponentConfig& config);

            AZ::u32 OnConfigurationChanged() override;
        };

    } // namespace Render
} // namespace AZ
