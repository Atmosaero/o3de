/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzCore/RTTI/BehaviorContext.h>
#include <AzFramework/Translation/TranslationDef.h>
#include <PostProcess/Outline/EditorOutlineComponent.h>

namespace AZ
{
    namespace Render
    {
        void EditorOutlineComponent::Reflect(AZ::ReflectContext* context)
        {
            BaseClass::Reflect(context);

            if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
            {
                serializeContext->Class<EditorOutlineComponent, BaseClass>()->Version(0);

                if (AZ::EditContext* editContext = serializeContext->GetEditContext())
                {
                    editContext
                        ->Class<EditorOutlineComponent>(
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Outline"),
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Draws stylized lines at depth discontinuities"))
                        ->ClassElement(Edit::ClassElements::EditorData, "")
                        ->Attribute(Edit::Attributes::Category, "Graphics/PostFX")
                        ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                        ->Attribute(AZ::Edit::Attributes::ViewportIcon, "Icons/Components/Viewport/Component_Placeholder.svg")
                        ->Attribute(Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                        ->Attribute(Edit::Attributes::AutoExpand, true);

                    editContext->Class<OutlineComponentController>("OutlineComponentController", "")
                        ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ->DataElement(
                            AZ::Edit::UIHandlers::Default,
                            &OutlineComponentController::m_configuration,
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Configuration"),
                            "")
                        ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly);

                    editContext->Class<OutlineComponentConfig>("OutlineComponentConfig", "")
                        ->DataElement(
                            Edit::UIHandlers::CheckBox,
                            &OutlineComponentConfig::m_enabled,
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Enable Outline"),
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Enable Outline."))
                        ->Attribute(Edit::Attributes::ChangeNotify, Edit::PropertyRefreshLevels::ValuesOnly)

                        ->DataElement(
                            Edit::UIHandlers::Slider,
                            &OutlineComponentConfig::m_thickness,
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Thickness"),
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Sampling radius in output pixels."))
                        ->Attribute(Edit::Attributes::Min, 1.0f)
                        ->Attribute(Edit::Attributes::Max, 8.0f)
                        ->Attribute(Edit::Attributes::ReadOnly, &OutlineComponentConfig::ArePropertiesReadOnly)

                        ->DataElement(
                            Edit::UIHandlers::Slider,
                            &OutlineComponentConfig::m_depthThreshold,
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Depth Threshold"),
                            QT_TRANSLATE_NOOP(
                                "AtomLyIntegration", "Relative depth discontinuity required for a line. Lower values reveal more detail."))
                        ->Attribute(Edit::Attributes::Min, 0.001f)
                        ->Attribute(Edit::Attributes::Max, 1.0f)
                        ->Attribute(Edit::Attributes::ReadOnly, &OutlineComponentConfig::ArePropertiesReadOnly)

                        ->DataElement(
                            Edit::UIHandlers::Slider,
                            &OutlineComponentConfig::m_opacity,
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Opacity"),
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Opacity of the outline."))
                        ->Attribute(Edit::Attributes::Min, 0.0f)
                        ->Attribute(Edit::Attributes::Max, 1.0f)
                        ->Attribute(Edit::Attributes::ReadOnly, &OutlineComponentConfig::ArePropertiesReadOnly)

                        ->DataElement(
                            Edit::UIHandlers::Color,
                            &OutlineComponentConfig::m_color,
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Color"),
                            QT_TRANSLATE_NOOP("AtomLyIntegration", "Linear RGB outline color."))
                        ->Attribute(Edit::Attributes::ReadOnly, &OutlineComponentConfig::ArePropertiesReadOnly)

                        // Overrides
                        ->ClassElement(AZ::Edit::ClassElements::Group, QT_TRANSLATE_NOOP("AtomLyIntegration", "Overrides"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, false)

                    // Auto-gen editor context settings for overrides
#define EDITOR_CLASS OutlineComponentConfig
                    // clang-format off
#include <Atom/Feature/ParamMacros/StartOverrideEditorContext.inl>
#include <Atom/Feature/PostProcess/Outline/OutlineParams.inl>
#include <Atom/Feature/ParamMacros/EndParams.inl>
// clang-format on
#undef EDITOR_CLASS
                        ;
                }
            }

            if (auto behaviorContext = azrtti_cast<BehaviorContext*>(context))
            {
                behaviorContext->Class<EditorOutlineComponent>()->RequestBus("OutlineRequestBus");

                behaviorContext
                    ->ConstantProperty("EditorOutlineComponentTypeId", BehaviorConstant(Uuid(Outline::EditorOutlineComponentTypeId)))
                    ->Attribute(AZ::Script::Attributes::Module, "render")
                    ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Automation);
            }
        }

        EditorOutlineComponent::EditorOutlineComponent(const OutlineComponentConfig& config)
            : BaseClass(config)
        {
        }

        u32 EditorOutlineComponent::OnConfigurationChanged()
        {
            m_controller.OnConfigChanged();
            return Edit::PropertyRefreshLevels::AttributesAndValues;
        }
    } // namespace Render
} // namespace AZ
