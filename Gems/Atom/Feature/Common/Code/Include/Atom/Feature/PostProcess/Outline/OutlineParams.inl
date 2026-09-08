/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

// Screen-space outline controls and PostFX layer overrides.
AZ_GFX_BOOL_PARAM(Enabled, m_enabled, false)
AZ_GFX_ANY_PARAM_BOOL_OVERRIDE(bool, Enabled, m_enabled)

AZ_GFX_FLOAT_PARAM(Thickness, m_thickness, Outline::DefaultThickness)
AZ_GFX_FLOAT_PARAM_FLOAT_OVERRIDE(float, Thickness, m_thickness)
AZ_GFX_FLOAT_PARAM(DepthThreshold, m_depthThreshold, Outline::DefaultDepthThreshold)
AZ_GFX_FLOAT_PARAM_FLOAT_OVERRIDE(float, DepthThreshold, m_depthThreshold)
AZ_GFX_FLOAT_PARAM(Opacity, m_opacity, 1.0f)
AZ_GFX_FLOAT_PARAM_FLOAT_OVERRIDE(float, Opacity, m_opacity)
AZ_GFX_VEC3_PARAM(Color, m_color, AZ::Vector3::CreateZero())
AZ_GFX_FLOAT_PARAM_FLOAT_OVERRIDE(AZ::Vector3, Color, m_color)
