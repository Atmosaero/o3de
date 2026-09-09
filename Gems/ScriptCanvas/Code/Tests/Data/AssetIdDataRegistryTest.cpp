/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/IO/ByteContainerStream.h>
#include <AzCore/Script/ScriptContext.h>
#include <AzCore/Serialization/Utils.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>

#include <ScriptCanvas/Core/Datum.h>
#include <ScriptCanvas/Core/ModifiableDatumView.h>
#include <ScriptCanvas/Data/DataRegistry.h>
#include <ScriptCanvas/Data/DataTypeUtils.h>
#include <ScriptCanvas/Translation/GraphToLuaUtility.h>
#include <ScriptCanvas/Variable/GraphVariableManagerComponent.h>
#include <Tests/Framework/ScriptCanvasUnitTestFixture.h>

namespace ScriptCanvasUnitTest
{
    class AssetIdDataRegistryTest
        : public ScriptCanvasUnitTestFixture
    {
    protected:
        void SetUp() override
        {
            ScriptCanvasUnitTestFixture::SetUp();
            ScriptCanvas::InitDataRegistry();
        }

        void TearDown() override
        {
            ScriptCanvas::ResetDataRegistry();
            ScriptCanvasUnitTestFixture::TearDown();
        }
    };

    TEST_F(AssetIdDataRegistryTest, AssetId_IsAvailableInTheVariablePaletteAndSlots)
    {
        const auto registry = ScriptCanvas::GetDataRegistry();
        const auto type = ScriptCanvas::Data::Type::AssetId();
        EXPECT_EQ(ScriptCanvas::Data::FromAZType(azrtti_typeid<AZ::Data::AssetId>()), type);

        // The variable palette enumerates native types through this table.
        const auto traits = registry->m_typeIdTraitMap.find(ScriptCanvas::Data::eType::AssetId);
        ASSERT_NE(traits, registry->m_typeIdTraitMap.end());
        EXPECT_EQ(traits->second.m_dataTraits.GetAZType(), azrtti_typeid<AZ::Data::AssetId>());
        EXPECT_EQ(traits->second.m_dataTraits.GetSCType(), type);

        const auto creatable = registry->m_creatableTypes.find(type);
        ASSERT_NE(creatable, registry->m_creatableTypes.end());
        EXPECT_FALSE(creatable->second.m_isTransient);
        EXPECT_TRUE(registry->IsUseableInSlot(type));
        EXPECT_FALSE(registry->m_slottableTypes.contains(type));
    }

    TEST_F(AssetIdDataRegistryTest, AssetIdVariable_CanBeDeclaredAssignedAndReset)
    {
        ScriptCanvas::GraphVariableManagerComponent manager;
        const ScriptCanvas::Datum initialValue(
            ScriptCanvas::Data::Type::AssetId(), ScriptCanvas::Datum::eOriginality::Original);
        const auto added = manager.AddVariable("Asset", initialValue, false);
        ASSERT_TRUE(added.IsSuccess());

        auto* variable = manager.FindVariableById(added.GetValue());
        ASSERT_NE(variable, nullptr);
        EXPECT_EQ(manager.GetVariableType(added.GetValue()), ScriptCanvas::Data::Type::AssetId());
        ASSERT_NE(variable->GetDatum()->GetAs<AZ::Data::AssetId>(), nullptr);
        EXPECT_FALSE(variable->GetDatum()->GetAs<AZ::Data::AssetId>()->IsValid());

        const AZ::Data::AssetId expected(AZ::Uuid::CreateRandom(), 0x1234abcd);
        ScriptCanvas::ModifiableDatumView view;
        variable->ConfigureDatumView(view);
        ASSERT_TRUE(view.IsValid());
        view.SetAs(expected);
        EXPECT_EQ(*variable->GetDatum()->GetAs<AZ::Data::AssetId>(), expected);

        view.SetToDefaultValueOfType();
        EXPECT_EQ(*variable->GetDatum()->GetAs<AZ::Data::AssetId>(), AZ::Data::AssetId());
    }

    TEST_F(AssetIdDataRegistryTest, AssetIdVariable_CloneHasIndependentIdentityAndValue)
    {
        ScriptCanvas::GraphVariableManagerComponent manager;
        const AZ::Data::AssetId expected(AZ::Uuid::CreateRandom(), 42);
        const auto added = manager.AddVariable("Asset", ScriptCanvas::Datum(expected), false);
        ASSERT_TRUE(added.IsSuccess());
        const auto* original = manager.FindVariableById(added.GetValue());
        ASSERT_NE(original, nullptr);

        const auto cloned = manager.CloneVariable(*original);
        ASSERT_TRUE(cloned.IsSuccess());
        EXPECT_NE(cloned.GetValue(), added.GetValue());
        auto* copy = manager.FindVariableById(cloned.GetValue());
        ASSERT_NE(copy, nullptr);
        ASSERT_NE(copy->GetDatum()->GetAs<AZ::Data::AssetId>(), nullptr);
        EXPECT_EQ(*copy->GetDatum()->GetAs<AZ::Data::AssetId>(), expected);

        ScriptCanvas::ModifiableDatumView view;
        copy->ConfigureDatumView(view);
        ASSERT_TRUE(view.IsValid());
        view.SetAs(AZ::Data::AssetId(expected.m_guid, 43));
        EXPECT_EQ(copy->GetDatum()->GetAs<AZ::Data::AssetId>()->m_subId, 43);
        original = manager.FindVariableById(added.GetValue());
        ASSERT_NE(original, nullptr);
        EXPECT_EQ(*original->GetDatum()->GetAs<AZ::Data::AssetId>(), expected);

        EXPECT_TRUE(manager.RemoveVariable(added.GetValue()));
        copy = manager.FindVariableById(cloned.GetValue());
        ASSERT_NE(copy, nullptr);
        EXPECT_EQ(*copy->GetDatum()->GetAs<AZ::Data::AssetId>(), AZ::Data::AssetId(expected.m_guid, 43));
    }

    struct AssetIdValueCase
    {
        const char* m_name;
        bool m_isValid;
        AZ::u32 m_subId;
    };

    class AssetIdValueTest
        : public AssetIdDataRegistryTest
        , public ::testing::WithParamInterface<AssetIdValueCase>
    {
    protected:
        AZ::Data::AssetId GetValue() const
        {
            return GetParam().m_isValid
                ? AZ::Data::AssetId(AZ::Uuid("{F67CC648-EA51-464C-9F5D-4A9CE41A7F86}"), GetParam().m_subId)
                : AZ::Data::AssetId();
        }

        void CheckSerialization(AZ::DataStream::StreamType format)
        {
            AZ::SerializeContext serializeContext;
            AZ::Data::AssetId::Reflect(&serializeContext);
            ScriptCanvas::Data::Type::Reflect(&serializeContext);
            ScriptCanvas::RuntimeVariable::Reflect(&serializeContext);
            ScriptCanvas::Datum::Reflect(&serializeContext);
            ScriptCanvas::VariableId::Reflect(&serializeContext);
            ScriptCanvas::GraphVariable::Reflect(&serializeContext);
            ScriptCanvas::VariableData::Reflect(&serializeContext);

            ScriptCanvas::GraphVariableManagerComponent manager;
            const AZ::Data::AssetId expected = GetValue();
            const auto added = manager.AddVariable("Asset", ScriptCanvas::Datum(expected), false);
            ASSERT_TRUE(added.IsSuccess());

            AZStd::vector<AZ::u8> buffer;
            AZ::IO::ByteContainerStream<AZStd::vector<AZ::u8>> stream(&buffer);
            ASSERT_TRUE(AZ::Utils::SaveObjectToStream(stream, format, manager.GetVariableDataConst(), &serializeContext));
            stream.Seek(0, AZ::IO::GenericStream::ST_SEEK_BEGIN);
            const AZStd::unique_ptr<ScriptCanvas::VariableData> restored(
                AZ::Utils::LoadObjectFromStream<ScriptCanvas::VariableData>(stream, &serializeContext));
            ASSERT_NE(restored, nullptr);

            const auto* variable = restored->FindVariable(added.GetValue());
            ASSERT_NE(variable, nullptr);
            EXPECT_EQ(variable->GetVariableName(), "Asset");
            EXPECT_EQ(variable->GetDatum()->GetType(), ScriptCanvas::Data::Type::AssetId());
            const auto* value = variable->GetDatum()->GetAs<AZ::Data::AssetId>();
            ASSERT_NE(value, nullptr);
            EXPECT_EQ(*value, expected);
            EXPECT_EQ(value->IsValid(), expected.IsValid());
        }
    };

    TEST_P(AssetIdValueTest, XmlSerializationPreservesVariableIdentityAndValue)
    {
        CheckSerialization(AZ::ObjectStream::ST_XML);
    }

    TEST_P(AssetIdValueTest, BinarySerializationPreservesVariableIdentityAndValue)
    {
        CheckSerialization(AZ::ObjectStream::ST_BINARY);
    }

    TEST_P(AssetIdValueTest, GeneratedLuaValueEvaluatesToTheOriginalAssetId)
    {
        const AZ::Data::AssetId expected = GetValue();
        const AZStd::string expression = ScriptCanvas::Translation::ToValueString(
            ScriptCanvas::Datum(expected), ScriptCanvas::Translation::Configuration{});
        ASSERT_FALSE(expression.empty());

        // Execute the generated expression, rather than only checking its spelling.
        AZ::BehaviorContext behaviorContext;
        AZ::Data::AssetId::Reflect(&behaviorContext);
        AZ::ScriptContext scriptContext;
        scriptContext.BindTo(&behaviorContext);
        const AZStd::string script = AZStd::string::format("assetIdValue = %s\n", expression.c_str());
        ASSERT_TRUE(scriptContext.Execute(script.c_str()));
        AZ::Data::AssetId actual;
        ASSERT_TRUE(scriptContext.ReadGlobal("assetIdValue", actual));
        EXPECT_EQ(actual, expected);
    }

    INSTANTIATE_TEST_SUITE_P(
        AssetIdValues,
        AssetIdValueTest,
        ::testing::Values(
            AssetIdValueCase{ "Empty", false, 0 },
            AssetIdValueCase{ "ZeroSubId", true, 0 },
            AssetIdValueCase{ "NonzeroSubId", true, 42 },
            AssetIdValueCase{ "HighBitSubId", true, 0x80000000u },
            AssetIdValueCase{ "MaxSubId", true, 0xffffffffu }),
        [](const ::testing::TestParamInfo<AssetIdValueCase>& info) { return info.param.m_name; });
} // namespace ScriptCanvasUnitTest
