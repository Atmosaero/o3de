/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */
#pragma once

#include <Atom/RPI.Public/Pass/ComputePass.h>

namespace AZ
{
    namespace Render
    {
        class OutlineSettings;

        class OutlinePass final : public RPI::ComputePass
        {
            AZ_RPI_PASS(OutlinePass);

        public:
            AZ_RTTI(OutlinePass, "{537EDD3D-690F-441C-988C-FC3FCBF2829E}", AZ::RPI::ComputePass);
            AZ_CLASS_ALLOCATOR(OutlinePass, SystemAllocator, 0);

            ~OutlinePass() = default;
            static RPI::Ptr<OutlinePass> Create(const RPI::PassDescriptor& descriptor);

            bool IsEnabled() const override;

        protected:
            void FrameBeginInternal(FramePrepareParams params) override;

        private:
            OutlinePass(const RPI::PassDescriptor& descriptor);
            const OutlineSettings* GetSettings() const;

            AZ::RHI::ShaderInputNameIndex m_constantsIndex = "m_constants";
        };
    } // namespace Render
} // namespace AZ
