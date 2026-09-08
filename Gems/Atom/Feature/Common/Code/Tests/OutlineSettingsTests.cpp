/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzCore/UnitTest/TestTypes.h>
#include <PostProcess/Outline/OutlineSettings.h>
#include <PostProcess/PostProcessSettings.h>

namespace UnitTest
{
    class OutlineSettingsTests : public LeakDetectionFixture
    {
    };

    TEST_F(OutlineSettingsTests, LayerBlendUsesPerParameterOverrides)
    {
        AZ::Render::OutlineSettings source(nullptr);
        AZ::Render::OutlineSettings target(nullptr);
        source.SetThickness(5.0f);
        source.SetThicknessOverride(0.5f);
        source.SetColor(AZ::Vector3(1.0f, 0.5f, 0.25f));
        source.SetOpacity(0.0f);
        source.SetOpacityOverride(0.0f);
        source.ApplySettingsTo(&target, 0.5f);
        EXPECT_FLOAT_EQ(target.GetThickness(), 2.0f);
        EXPECT_TRUE(target.GetColor().IsClose(AZ::Vector3(0.5f, 0.25f, 0.125f)));
        EXPECT_FLOAT_EQ(target.GetOpacity(), 1.0f);
    }

    TEST_F(OutlineSettingsTests, EnabledRequiresHalfLayerWeightAndBooleanOverride)
    {
        AZ::Render::OutlineSettings source(nullptr);
        AZ::Render::OutlineSettings target(nullptr);
        source.SetEnabled(true);
        source.ApplySettingsTo(&target, 0.49f);
        EXPECT_FALSE(target.GetEnabled());
        source.SetEnabledOverride(false);
        source.ApplySettingsTo(&target, 1.0f);
        EXPECT_FALSE(target.GetEnabled());
        source.SetEnabledOverride(true);
        source.ApplySettingsTo(&target, 0.5f);
        EXPECT_TRUE(target.GetEnabled());
    }

    TEST_F(OutlineSettingsTests, PostProcessSettingsOwnAndRemoveOutline)
    {
        AZ::Render::PostProcessSettings settings(nullptr);
        EXPECT_EQ(settings.GetOutlineSettings(), nullptr);
        auto* outline = settings.GetOrCreateOutlineSettingsInterface();
        EXPECT_EQ(outline, settings.GetOrCreateOutlineSettingsInterface());
        outline->SetEnabled(true);
        outline->OnConfigChanged();
        EXPECT_TRUE(settings.GetOutlineSettings()->GetEnabled());
        settings.RemoveOutlineSettingsInterface();
        EXPECT_EQ(settings.GetOutlineSettings(), nullptr);
    }
} // namespace UnitTest
