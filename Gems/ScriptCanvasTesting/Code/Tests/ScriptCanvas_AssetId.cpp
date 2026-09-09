/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <Source/Framework/ScriptCanvasTestFixture.h>
#include <AzCore/IO/ByteContainerStream.h>
#include <Editor/View/Widgets/DataTypePalette/DataTypePaletteModel.h>
#include <Editor/View/Widgets/VariablePanel/VariableDockWidget.h>
#include <ScriptCanvas/Core/GraphSerialization.h>
#include <ScriptCanvas/Libraries/Core/GetVariable.h>
#include <ScriptCanvas/Libraries/Core/SetVariable.h>
#include <ScriptCanvas/Libraries/Core/Start.h>
#include <ScriptCanvas/Libraries/Core/Method.h>
#include <ScriptCanvas/Libraries/ScriptCanvasNodeRegistry.h>
#include <ScriptCanvas/Libraries/UnitTesting/UnitTestBus.h>
#include <QApplication>
#include <Editor/View/Widgets/NodePalette/NodePaletteModel.h>

namespace ScriptCanvasTests
{
    namespace
    {
        // Supply the graph-tab mapping normally owned by MainWindow. The palette,
        // dock widget, their signal connections, and variable manager are real.
        class AssetIdEditorRequests : public ScriptCanvasEditor::GeneralRequestBus::Handler
        {
        public:
            explicit AssetIdEditorRequests(ScriptCanvas::ScriptCanvasId id) : m_id(id) { BusConnect(); }
            ~AssetIdEditorRequests() override { BusDisconnect(); }
            ScriptCanvas::ScriptCanvasId GetScriptCanvasId(const GraphCanvas::GraphId&) const override { return m_id; }
            AZ::Outcome<int, AZStd::string> OpenScriptCanvasAsset(
                ScriptCanvas::SourceHandle, ScriptCanvasEditor::Tracker::ScriptCanvasFileState, int) override { return AZ::Success(0); }
            AZ::Outcome<int, AZStd::string> OpenScriptCanvasAssetId(
                const ScriptCanvas::SourceHandle&, ScriptCanvasEditor::Tracker::ScriptCanvasFileState) override { return AZ::Success(0); }
            int CloseScriptCanvasAsset(const ScriptCanvas::SourceHandle&) override { return 0; }
            bool CreateScriptCanvasAssetFor(const ScriptCanvasEditor::TypeDefs::EntityComponentId&) override { return false; }
            bool IsScriptCanvasAssetOpen(const ScriptCanvas::SourceHandle&) const override { return false; }
            void CreateNewRuntimeAsset() override {}
            bool IsInUndoRedo(const AZ::EntityId&) const override { return false; }
            bool IsScriptCanvasInUndoRedo(const ScriptCanvas::ScriptCanvasId&) const override { return false; }
            bool IsActiveGraphInUndoRedo() const override { return false; }
            void PostUndoPoint(ScriptCanvas::ScriptCanvasId id) override { EXPECT_EQ(id, m_id); ++m_undoPoints; }
            void SignalSceneDirty(ScriptCanvas::SourceHandle) override {}
            void PushPreventUndoStateUpdate() override {}
            void PopPreventUndoStateUpdate() override {}
            void ClearPreventUndoStateUpdate() override {}
            void TriggerUndo() override {}
            void TriggerRedo() override {}
            const ScriptCanvasEditor::CategoryInformation* FindNodePaletteCategoryInformation(AZStd::string_view) const override
            { return nullptr; }
            const ScriptCanvasEditor::NodePaletteModelInformation* FindNodePaletteModelInformation(
                const ScriptCanvas::NodeTypeIdentifier&) const override { return nullptr; }
            int m_undoPoints = 0;
        private:
            ScriptCanvas::ScriptCanvasId m_id;
        };

        class AssetIdRuntimeObserver
        {
        public:
            AZ_TYPE_INFO(AssetIdRuntimeObserver, "{AB93C349-A135-492E-A1EF-478C7E819437}");
            static inline AZStd::vector<AZ::Data::AssetId>* s_values = nullptr;
            static void Observe(const AZ::Data::AssetId& value)
            {
                ASSERT_NE(s_values, nullptr);
                s_values->push_back(value);
                if (s_values->size() == 2)
                {
                    ScriptCanvas::UnitTesting::Bus::Broadcast(
                        &ScriptCanvas::UnitTesting::BusTraits::MarkComplete, "Read AssetId before and after assignment");
                }
            }
            static void VerifyMethods(const AZ::Data::AssetId& id, const AZStd::string& text, bool valid, bool equal)
            {
                const AZ::Data::AssetId expected(AZ::Uuid("{F67CC648-EA51-464C-9F5D-4A9CE41A7F86}"), 0xfedcba98u);
                EXPECT_EQ(id, expected);
                EXPECT_EQ(text, expected.ToString<AZStd::string>());
                EXPECT_TRUE(valid);
                EXPECT_TRUE(equal);
                ScriptCanvas::UnitTesting::Bus::Broadcast(
                    &ScriptCanvas::UnitTesting::BusTraits::MarkComplete, "AssetId methods executed");
            }
            static void Reflect(AZ::BehaviorContext* context)
            {
                context->Class<AssetIdRuntimeObserver>("AssetIdRuntimeObserver")
                    ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                    ->Method("VerifyMethods", &VerifyMethods)
                    ->Method("Observe", &Observe, { AZ::BehaviorParameterOverrides("Value", "AssetId read from the graph") });
            }
        };
    }

    class AssetIdIntegrationTest : public ScriptCanvasTestFixture
    {
    protected:
        static inline int s_argc = 1;
        static inline char s_name[] = "ScriptCanvasAssetIdTest";
        static inline char* s_argv[] = { s_name, nullptr };
        static inline AZStd::unique_ptr<QApplication> s_qtApplication;

        static void SetUpTestCase()
        {
            if (!QApplication::instance())
            {
                s_qtApplication = AZStd::make_unique<QApplication>(s_argc, s_argv);
            }
            ScriptCanvasTestFixture::SetUpTestCase();
        }
        static void TearDownTestCase()
        {
            // Populating the node palette lazily creates this environment registry.
            ScriptCanvas::NodeRegistry::ResetInstance();
            ScriptCanvasTestFixture::TearDownTestCase();
            s_qtApplication.reset();
        }
        void SetUp() override
        {
            ScriptCanvasTestFixture::SetUp();
            AssetIdRuntimeObserver::Reflect(m_behaviorContext);
            AssetIdRuntimeObserver::s_values = &m_observedValues;
        }
        void TearDown() override
        {
            AssetIdRuntimeObserver::s_values = nullptr;
            m_observedValues.clear();
            m_behaviorContext->EnableRemoveReflection();
            AssetIdRuntimeObserver::Reflect(m_behaviorContext);
            m_behaviorContext->DisableRemoveReflection();
            ScriptCanvasTestFixture::TearDown();
        }
        ScriptCanvas::DataPtr MakeSource()
        {
            ScriptCanvas::DataPtr source = aznew ScriptCanvas::ScriptCanvasData();
            source->m_scriptCanvasEntity = AZStd::make_unique<AZ::Entity>("AssetId integration graph");
            auto* graph = source->m_scriptCanvasEntity->CreateComponent<ScriptCanvasEditor::EditorGraph>();
            graph->MarkOwnership(*source);
            source->m_scriptCanvasEntity->CreateComponent<ScriptCanvas::GraphVariableManagerComponent>(graph->GetScriptCanvasId());
            source->m_scriptCanvasEntity->Init();
            source->m_scriptCanvasEntity->Activate();
            return source;
        }
        AZStd::vector<AZ::Data::AssetId> m_observedValues;
    };

    TEST_F(AssetIdIntegrationTest, VariablePaletteCreatesAnAssetIdGraphVariable)
    {
        const auto source = MakeSource();
        const auto graphId = source->GetGraph()->GetScriptCanvasId();
        AssetIdEditorRequests editorRequests(graphId);
        ScriptCanvasEditor::VariableDockWidget dock;
        dock.OnActiveGraphChanged(AZ::Entity::MakeId());
        dock.PopulateVariablePalette({});
        auto* palette = qobject_cast<ScriptCanvasEditor::VariablePaletteTableView*>(dock.GetVariablePaletteTableView());
        ASSERT_NE(palette, nullptr);
        const auto* model = palette->GetVariableTypePaletteModel();
        ASSERT_NE(model, nullptr);
        EXPECT_EQ(model->FindTypeIdForTypeName("AssetId"), azrtti_typeid<AZ::Data::AssetId>());
        EXPECT_TRUE(model->FindIndexForTypeId(azrtti_typeid<AZ::Data::AssetId>()).isValid());

        const auto* variables = source->GetGraph()->GetVariableDataConst();
        ASSERT_NE(variables, nullptr);
        ASSERT_TRUE(variables->GetVariables().empty());
        palette->TryCreateVariableByTypeName("AssetId");

        ASSERT_EQ(variables->GetVariables().size(), 1);
        const auto& variable = variables->GetVariables().begin()->second;
        EXPECT_EQ(variable.GetDatum()->GetType(), ScriptCanvas::Data::Type::AssetId());
        const auto* value = variable.GetDatum()->GetAs<AZ::Data::AssetId>();
        ASSERT_NE(value, nullptr);
        EXPECT_EQ(*value, AZ::Data::AssetId());
        EXPECT_EQ(editorRequests.m_undoPoints, 1);
    }

    TEST_F(AssetIdIntegrationTest, JsonGraphRoundTripPreservesVariableNodesAndExecutesGetSet)
    {
        using namespace ScriptCanvas;
        auto source = MakeSource();
        auto* graph = source->ModGraph();
        const auto graphId = graph->GetScriptCanvasId();
        const auto variableId = CreateVariable(graphId, AZ::Data::AssetId(), "Asset");
        ASSERT_TRUE(variableId.IsValid());
        AZ::EntityId startId, getBeforeId, setId, getAfterId;
        ASSERT_NE(CreateTestNode<Nodes::Core::Start>(graphId, startId), nullptr);
        auto* before = CreateTestNode<Nodes::Core::GetVariableNode>(graphId, getBeforeId);
        auto* setter = CreateTestNode<Nodes::Core::SetVariableNode>(graphId, setId);
        auto* after = CreateTestNode<Nodes::Core::GetVariableNode>(graphId, getAfterId);
        ASSERT_NE(before, nullptr);
        ASSERT_NE(setter, nullptr);
        ASSERT_NE(after, nullptr);
        before->SetId(variableId);
        setter->SetId(variableId);
        after->SetId(variableId);
        const AZ::Data::AssetId expected(AZ::Uuid("{F67CC648-EA51-464C-9F5D-4A9CE41A7F86}"), 0xfedcba98u);
        ModifiableDatumView input;
        ASSERT_TRUE(setter->FindModifiableDatumView(setter->GetDataInSlotId(), input));
        input.SetAs(expected);
        const auto observeBefore = CreateClassFunctionNode(graphId, "AssetIdRuntimeObserver", "Observe");
        const auto observeAfter = CreateClassFunctionNode(graphId, "AssetIdRuntimeObserver", "Observe");
        ASSERT_TRUE(Connect(*graph, startId, "Out", getBeforeId, "In"));
        ASSERT_TRUE(Connect(*graph, getBeforeId, "Out", observeBefore, "In"));
        ASSERT_TRUE(Connect(*graph, observeBefore, "Out", setId, "In"));
        ASSERT_TRUE(Connect(*graph, setId, "Out", getAfterId, "In"));
        ASSERT_TRUE(Connect(*graph, getAfterId, "Out", observeAfter, "In"));
        auto* observeBeforeNode = GetTestNode<Nodes::Core::Method>(graphId, observeBefore);
        auto* observeAfterNode = GetTestNode<Nodes::Core::Method>(graphId, observeAfter);
        ASSERT_NE(observeBeforeNode, nullptr);
        ASSERT_NE(observeAfterNode, nullptr);
        ASSERT_TRUE(graph->ConnectByEndpoint(
            { getBeforeId, before->GetDataOutSlotId() }, { observeBefore, observeBeforeNode->GetSlotId("Value") }));
        ASSERT_TRUE(graph->ConnectByEndpoint(
            { getAfterId, after->GetDataOutSlotId() }, { observeAfter, observeAfterNode->GetSlotId("Value") }));

        AZStd::string json;
        AZ::IO::ByteContainerStream<AZStd::string> stream(&json);
        const auto saved = Serialize(*source, stream);
        ASSERT_TRUE(saved) << saved.m_errors.c_str();
        source.reset();
        auto loaded = Deserialize(json, MakeInternalGraphEntitiesUnique::No, LoadReferencedAssets::Yes);
        ASSERT_TRUE(loaded) << loaded.m_errors.c_str();
        EXPECT_FALSE(loaded.m_fromObjectStreamXML);
        auto* restoredGraph = loaded.m_graphDataPtr->ModGraph();
        const auto* restoredVariable = restoredGraph->GetVariableDataConst()->FindVariable(variableId);
        ASSERT_NE(restoredVariable, nullptr);
        ASSERT_NE(restoredVariable->GetDatum()->GetAs<AZ::Data::AssetId>(), nullptr);
        EXPECT_EQ(*restoredVariable->GetDatum()->GetAs<AZ::Data::AssetId>(), AZ::Data::AssetId());
        auto* restoredBefore = GetTestNode<Nodes::Core::GetVariableNode>(restoredGraph->GetScriptCanvasId(), getBeforeId);
        auto* restoredAfter = GetTestNode<Nodes::Core::GetVariableNode>(restoredGraph->GetScriptCanvasId(), getAfterId);
        auto* restoredSetter = GetTestNode<Nodes::Core::SetVariableNode>(restoredGraph->GetScriptCanvasId(), setId);
        ASSERT_NE(restoredBefore, nullptr);
        ASSERT_NE(restoredAfter, nullptr);
        ASSERT_NE(restoredSetter, nullptr);
        EXPECT_EQ(restoredBefore->GetId(), variableId);
        EXPECT_EQ(restoredAfter->GetId(), variableId);
        EXPECT_EQ(restoredSetter->GetId(), variableId);
        const auto* restoredInput = restoredSetter->FindDatum(restoredSetter->GetDataInSlotId());
        ASSERT_NE(restoredInput, nullptr);
        ASSERT_NE(restoredInput->GetAs<AZ::Data::AssetId>(), nullptr);
        EXPECT_EQ(*restoredInput->GetAs<AZ::Data::AssetId>(), expected);

        ScriptCanvasEditor::LoadTestGraphResult executable;
        executable.m_editorAsset = SourceHandle::FromRelativePath(
            loaded.m_graphDataPtr, AZ::Uuid::CreateRandom(), "AssetIdIntegration.scriptcanvas");
        AZ::Outcome<AZ::Data::Asset<RuntimeAsset>, AZStd::string> runtime = AZ::Failure(AZStd::string("No asset conversion handler"));
        ScriptCanvasEditor::EditorAssetConversionBus::BroadcastResult(runtime,
            &ScriptCanvasEditor::EditorAssetConversionBusTraits::CreateRuntimeAsset, executable.m_editorAsset);
        ASSERT_TRUE(runtime.IsSuccess()) << runtime.GetError().c_str();
        executable.m_runtimeAsset = runtime.GetValue();
        executable.m_entity = AZStd::make_unique<AZ::Entity>("AssetId graph execution");
        ScriptCanvasEditor::RunGraphSpec spec;
        spec.graphPath = "AssetIdIntegration.scriptcanvas";
        spec.runSpec.duration = ScriptCanvasEditor::DurationSpec::Ticks(1);
        ScriptCanvasEditor::Reporter reporter;
        reporter.SetExecutionMode(ExecutionMode::Interpreted);
        reporter.SetExecutionConfiguration(ExecutionConfiguration::Debug);
        ScriptCanvasEditor::RunGraphImplementation(spec, executable, reporter);
        EXPECT_TRUE(reporter.IsCompiled());
        EXPECT_TRUE(reporter.IsActivated());
        EXPECT_TRUE(reporter.IsComplete());
        EXPECT_TRUE(reporter.IsErrorFree());
        EXPECT_THAT(m_observedValues, ::testing::ElementsAre(AZ::Data::AssetId(), expected));
    }

    TEST_F(AssetIdIntegrationTest, PaletteHidesReprAndKeepsPublicAssetIdMethods)
    {
        const auto reflected = m_behaviorContext->m_classes.find("AssetId");
        ASSERT_NE(reflected, m_behaviorContext->m_classes.end());
        EXPECT_TRUE(reflected->second->m_methods.contains("__repr__"));
        ScriptCanvasEditor::NodePaletteModel palette;
        palette.RepopulateModel();
        AZStd::unordered_set<AZStd::string> methods;
        for (const auto& entry : palette.GetNodeRegistry())
        {
            const auto* method = azrtti_cast<const ScriptCanvasEditor::MethodNodeModelInformation*>(entry.second);
            if (method && method->m_classMethod == "AssetId")
            {
                methods.insert(method->m_methodName);
            }
        }
        EXPECT_FALSE(methods.contains("__repr__"));
        EXPECT_FALSE(methods.contains("__tostring"));
        EXPECT_FALSE(methods.contains("__eq"));
        for (const char* name : {"CreateString", "ToString", "IsValid", "IsEqual"})
        {
            EXPECT_TRUE(methods.contains(name)) << name;
        }
    }

    TEST_F(AssetIdIntegrationTest, AssetIdNamedMethodsPreserveLuaOperators)
    {
        AZ::ScriptContext script;
        script.BindTo(m_behaviorContext);
        ASSERT_TRUE(script.Execute(R"(
            local a = AssetId.CreateString("{F67CC648-EA51-464C-9F5D-4A9CE41A7F86}:fedcba98")
            local b = AssetId.CreateString(AssetId.ToString(a))
            local other = AssetId.CreateString("{F67CC648-EA51-464C-9F5D-4A9CE41A7F86}:fedcba99")
            local empty = AssetId.CreateString("invalid")
            assert(AssetId.IsValid(a))
            assert(not AssetId.IsValid(empty))
            assert(AssetId.IsEqual(a, b) and a == b)
            assert(not AssetId.IsEqual(a, other) and a ~= other)
            assert(AssetId.IsEqual(empty, AssetId()))
            assert(AssetId.ToString(a) == tostring(a))
            assert(AssetId.__repr__(a) == tostring(a))
        )"));
    }

    TEST_F(AssetIdIntegrationTest, AssetIdMethodsExecuteInCompiledGraph)
    {
        using namespace ScriptCanvas;
        auto source = MakeSource();
        auto* graph = source->ModGraph();
        const auto graphId = graph->GetScriptCanvasId();
        AZ::EntityId startId;
        ASSERT_NE(CreateTestNode<Nodes::Core::Start>(graphId, startId), nullptr);
        const auto create = CreateClassFunctionNode(graphId, "AssetId", "CreateString");
        const auto stringify = CreateClassFunctionNode(graphId, "AssetId", "ToString");
        const auto valid = CreateClassFunctionNode(graphId, "AssetId", "IsValid");
        const auto equal = CreateClassFunctionNode(graphId, "AssetId", "IsEqual");
        const auto verify = CreateClassFunctionNode(graphId, "AssetIdRuntimeObserver", "VerifyMethods");
        for (auto id : {create, stringify, valid, equal, verify})
        {
            ASSERT_NE(GetTestNode<Nodes::Core::Method>(graphId, id), nullptr);
        }
        for (auto id : {create, stringify, valid, equal})
        {
            ASSERT_EQ(GetTestNode<Nodes::Core::Method>(graphId, id)->GetSlotsByType(CombinedSlotType::DataOut).size(), 1);
        }
        auto inputs = [&](AZ::EntityId id)
        {
            return GetTestNode<Nodes::Core::Method>(graphId, id)->GetSlotsByType(CombinedSlotType::DataIn);
        };
        auto output = [&](AZ::EntityId id)
        {
            return GetTestNode<Nodes::Core::Method>(graphId, id)->GetSlotsByType(CombinedSlotType::DataOut).front()->GetId();
        };
        ASSERT_EQ(inputs(create).size(), 1);
        ASSERT_EQ(inputs(stringify).size(), 1);
        ASSERT_EQ(inputs(valid).size(), 1);
        ASSERT_EQ(inputs(equal).size(), 2);
        ASSERT_EQ(inputs(verify).size(), 4);
        const AZ::Data::AssetId expected(AZ::Uuid("{F67CC648-EA51-464C-9F5D-4A9CE41A7F86}"), 0xfedcba98u);
        ModifiableDatumView input;
        ASSERT_TRUE(GetTestNode<Nodes::Core::Method>(graphId, create)->FindModifiableDatumView(inputs(create)[0]->GetId(), input));
        input.SetAs(expected.ToString<AZStd::string>());
        ASSERT_TRUE(GetTestNode<Nodes::Core::Method>(graphId, equal)->FindModifiableDatumView(inputs(equal)[1]->GetId(), input));
        input.SetAs(expected);
        AZ::EntityId previous = startId;
        for (auto id : {create, stringify, valid, equal, verify})
        {
            ASSERT_TRUE(Connect(*graph, previous, "Out", id, "In"));
            previous = id;
        }
        for (auto id : {stringify, valid, equal, verify})
        {
            ASSERT_TRUE(graph->ConnectByEndpoint({create, output(create)}, {id, inputs(id)[0]->GetId()}));
        }
        ASSERT_TRUE(graph->ConnectByEndpoint({stringify, output(stringify)}, {verify, inputs(verify)[1]->GetId()}));
        ASSERT_TRUE(graph->ConnectByEndpoint({valid, output(valid)}, {verify, inputs(verify)[2]->GetId()}));
        ASSERT_TRUE(graph->ConnectByEndpoint({equal, output(equal)}, {verify, inputs(verify)[3]->GetId()}));
        ScriptCanvasEditor::LoadTestGraphResult executable;
        executable.m_editorAsset = SourceHandle::FromRelativePath(
            source, AZ::Uuid::CreateRandom(), "AssetIdIntegration.scriptcanvas");
        AZ::Outcome<AZ::Data::Asset<RuntimeAsset>, AZStd::string> runtime = AZ::Failure(AZStd::string("No asset conversion handler"));
        ScriptCanvasEditor::EditorAssetConversionBus::BroadcastResult(runtime,
            &ScriptCanvasEditor::EditorAssetConversionBusTraits::CreateRuntimeAsset, executable.m_editorAsset);
        ASSERT_TRUE(runtime.IsSuccess()) << runtime.GetError().c_str();
        executable.m_runtimeAsset = runtime.GetValue();
        executable.m_entity = AZStd::make_unique<AZ::Entity>("AssetId graph execution");
        ScriptCanvasEditor::RunGraphSpec spec;
        spec.graphPath = "AssetIdIntegration.scriptcanvas";
        spec.runSpec.duration = ScriptCanvasEditor::DurationSpec::Ticks(1);
        ScriptCanvasEditor::Reporter reporter;
        reporter.SetExecutionMode(ExecutionMode::Interpreted);
        reporter.SetExecutionConfiguration(ExecutionConfiguration::Debug);
        ScriptCanvasEditor::RunGraphImplementation(spec, executable, reporter);
        EXPECT_TRUE(reporter.IsCompiled());
        EXPECT_TRUE(reporter.IsActivated());
        EXPECT_TRUE(reporter.IsComplete());
        EXPECT_TRUE(reporter.IsErrorFree());
    }
}
