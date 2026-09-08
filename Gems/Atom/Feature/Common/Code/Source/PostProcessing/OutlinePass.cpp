/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Atom/RPI.Public/RenderPipeline.h>
#include <Atom/RPI.Public/Scene.h>
#include <AzCore/std/algorithm.h>
#include <PostProcess/PostProcessFeatureProcessor.h>
#include <PostProcessing/OutlinePass.h>

namespace AZ::Render
{
    RPI::Ptr<OutlinePass> OutlinePass::Create(const RPI::PassDescriptor& descriptor)
    {
        return aznew OutlinePass(descriptor);
    }

    OutlinePass::OutlinePass(const RPI::PassDescriptor& descriptor)
        : RPI::ComputePass(descriptor)
    {
    }

    const OutlineSettings* OutlinePass::GetSettings() const
    {
        const auto* scene = GetScene();
        const auto view = GetView();
        if (!scene || !view)
        {
            return nullptr;
        }
        auto* fp = scene->GetFeatureProcessor<PostProcessFeatureProcessor>();
        auto* settings = fp ? fp->GetLevelSettingsFromView(view) : nullptr;
        return settings ? settings->GetOutlineSettings() : nullptr;
    }

    bool OutlinePass::IsEnabled() const
    {
        const auto* settings = GetSettings();
        return ComputePass::IsEnabled() && settings && settings->GetEnabled() && settings->GetOpacity() > 0.0f;
    }

    void OutlinePass::FrameBeginInternal(FramePrepareParams params)
    {
        // Must match Constants in Outline.azsl.
        struct Constants
        {
            AZStd::array<uint32_t, 2> m_outputSize;
            float m_thickness;
            float m_depthThreshold;
            AZStd::array<float, 3> m_color;
            float m_opacity;
        } constants{};
        static_assert(sizeof(Constants) == 32);

        const auto* settings = GetSettings();
        const auto* output = GetOutputBinding(0).GetAttachment().get();
        if (settings && output)
        {
            const auto& size = output->m_descriptor.m_image.m_size;
            constants.m_outputSize = { size.m_width, size.m_height };
            constants.m_thickness = AZStd::clamp(settings->GetThickness(), 1.0f, Outline::MaxThickness);
            constants.m_depthThreshold = AZStd::clamp(settings->GetDepthThreshold(), 0.001f, 1.0f);
            settings->GetColor().StoreToFloat3(constants.m_color.data());
            constants.m_opacity = AZStd::clamp(settings->GetOpacity(), 0.0f, 1.0f);
            m_shaderResourceGroup->SetConstant(m_constantsIndex, constants);
        }
        ComputePass::FrameBeginInternal(params);
    }
} // namespace AZ::Render
