/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <Atom/Feature/PostProcess/Outline/OutlineConstants.h>
#include <Atom/Feature/PostProcess/Outline/OutlineSettingsInterface.h>
#include <Atom/Feature/PostProcess/PostProcessFeatureProcessorInterface.h>
#include <Atom/Feature/PostProcess/PostProcessSettingsInterface.h>

#include <AtomLyIntegration/CommonFeatures/PostProcess/Outline/OutlineBus.h>
#include <AtomLyIntegration/CommonFeatures/PostProcess/Outline/OutlineComponentConfig.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TransformBus.h>

namespace AZ
{
    namespace Render
    {
        class OutlineComponentController final : public OutlineRequestBus::Handler
        {
        public:
            friend class EditorOutlineComponent;

            AZ_TYPE_INFO(AZ::Render::OutlineComponentController, "{36B6AA11-FEE3-43FD-B8E6-FA6D8EF670A1}");
            static void Reflect(AZ::ReflectContext* context);
            static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
            static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
            static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

            OutlineComponentController() = default;
            OutlineComponentController(const OutlineComponentConfig& config);

            void Activate(EntityId entityId);
            void Deactivate();
            void SetConfiguration(const OutlineComponentConfig& config);
            const OutlineComponentConfig& GetConfiguration() const;

            // Auto-gen function override declarations (functions definitions in .cpp)...
            // clang-format off
#include <Atom/Feature/ParamMacros/StartParamFunctionsOverride.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
            // clang-format on

        private:
            AZ_DISABLE_COPY(OutlineComponentController);

            void OnConfigChanged();

            PostProcessSettingsInterface* m_postProcessInterface = nullptr;
            OutlineSettingsInterface* m_settingsInterface = nullptr;
            OutlineComponentConfig m_configuration;
            EntityId m_entityId;
        };
    } // namespace Render
} // namespace AZ
