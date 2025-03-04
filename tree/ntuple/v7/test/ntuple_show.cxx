#include "ntuple_test.hxx"
#include "SimpleCollectionProxy.hxx"

TEST(RNTupleShow, Empty)
{
   std::string rootFileName{"test_ntuple_show_empty.root"};
   std::string ntupleName{"EmptyNTuple"};
   FileRaii fileGuard(rootFileName);
   {
      auto model = RNTupleModel::Create();
      auto ntuple = RNTupleWriter::Recreate(std::move(model), ntupleName, rootFileName);
      ntuple->Fill();
      ntuple->Fill();
   }
   auto model2 = RNTupleModel::Create();
   auto ntuple2 = RNTupleReader::Open(std::move(model2), ntupleName, rootFileName);

   std::ostringstream os;
   ntuple2->Show(0, ROOT::Experimental::ENTupleShowFormat::kCurrentModelJSON, os);
   std::string fString{"{}\n"};
   EXPECT_EQ(fString, os.str());

   std::ostringstream os1;
   ntuple2->Show(1, ROOT::Experimental::ENTupleShowFormat::kCurrentModelJSON, os1);
   std::string fString1{"{}\n"};
   EXPECT_EQ(fString1, os1.str());
}

TEST(RNTupleShow, BasicTypes)
{
   std::string rootFileName{"test_ntuple_show_basictypes.root"};
   std::string ntupleName{"SimpleTypesNtuple"};
   FileRaii fileGuard(rootFileName);
   {
      auto model = RNTupleModel::Create();
      auto fieldPt = model->MakeField<float>("pt");
      auto fielddb = model->MakeField<double>("db");
      auto fieldint = model->MakeField<int>("int");
      auto fielduint = model->MakeField<unsigned>("uint");
      auto field64uint = model->MakeField<std::uint64_t>("uint64");
      auto fieldstring = model->MakeField<std::string>("string");
      auto fieldbool = model->MakeField<bool>("boolean");
      auto fieldchar = model->MakeField<uint8_t>("uint8");
      auto fieldbitset = model->MakeField<std::bitset<65>>("bitset");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), ntupleName, rootFileName);

      *fieldPt = 5.0f;
      *fielddb = 9.99;
      *fieldint = -4;
      *fielduint = 3;
      *field64uint = 44444444444ull;
      *fieldstring = "TestString";
      *fieldbool = true;
      *fieldchar = 97;
      *fieldbitset = std::bitset<65>("10000000000000000000000000000010000000000000000000000000000010010");
      ntuple->Fill();

      *fieldPt = 8.5f;
      *fielddb = 9.998;
      *fieldint = -94;
      *fielduint = -30;
      *field64uint = 2299994967294ull;
      *fieldstring = "TestString2";
      *fieldbool = false;
      *fieldchar = 98;
      fieldbitset->flip();
      ntuple->Fill();
   }

   auto ntuple2 = RNTupleReader::Open(ntupleName, rootFileName);

   std::ostringstream os;
   ntuple2->Show(0, ROOT::Experimental::ENTupleShowFormat::kCompleteJSON, os);
   // clang-format off
   std::string fString{ std::string("")
      + "{\n"
      + "  \"pt\": 5,\n"
      + "  \"db\": 9.99,\n"
      + "  \"int\": -4,\n"
      + "  \"uint\": 3,\n"
      + "  \"uint64\": 44444444444,\n"
      + "  \"string\": \"TestString\",\n"
      + "  \"boolean\": true,\n"
      + "  \"uint8\": 97,\n"
      + "  \"bitset\": \"10000000000000000000000000000010000000000000000000000000000010010\"\n"
      + "}\n" };
   // clang-format on
   EXPECT_EQ(fString, os.str());

   std::ostringstream os1;
   ntuple2->Show(1, ROOT::Experimental::ENTupleShowFormat::kCompleteJSON, os1);
   // clang-format off
   std::string fString1{ std::string("")
      + "{\n"
      + "  \"pt\": 8.5,\n"
      + "  \"db\": 9.998,\n"
      + "  \"int\": -94,\n"
      + "  \"uint\": 4294967266,\n"
      + "  \"uint64\": 2299994967294,\n"
      + "  \"string\": \"TestString2\",\n"
      + "  \"boolean\": false,\n"
      + "  \"uint8\": 98,\n"
      + "  \"bitset\": \"01111111111111111111111111111101111111111111111111111111111101101\"\n"
      + "}\n" };
   // clang-format on
   EXPECT_EQ(fString1, os1.str());

   // TODO(jblomer): this should fail to an exception instead
   EXPECT_DEATH(ntuple2->Show(2, ROOT::Experimental::ENTupleShowFormat::kCompleteJSON), ".*");
}

TEST(RNTupleShow, Vectors)
{
   std::string rootFileName{"test_ntuple_show_vector.root"};
   std::string ntupleName{"VecNTuple"};
   FileRaii fileGuard(rootFileName);
   {
      auto model = RNTupleModel::Create();
      auto fieldIntVec = model->MakeField<std::vector<int>>("intVec");
      auto fieldFloatVecVec = model->MakeField<std::vector<std::vector<float>>>("floatVecVec");
      auto fieldBoolVecVec = model->MakeField<std::vector<std::vector<bool>>>("booleanVecVec");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), ntupleName, rootFileName);

      *fieldIntVec = std::vector<int>{4, 5, 6};
      *fieldFloatVecVec = std::vector<std::vector<float>>{std::vector<float>{0.1, 0.2}, std::vector<float>{1.1, 1.2}};
      *fieldBoolVecVec = std::vector<std::vector<bool>>{
         std::vector<bool>{false, true, false}, std::vector<bool>{false, true}, std::vector<bool>{true, false, false}};
      ntuple->Fill();

      fieldIntVec->emplace_back(7);
      fieldFloatVecVec->emplace_back(std::vector<float>{2.2, 2.3});
      fieldBoolVecVec->emplace_back(std::vector<bool>{false, true});
      ntuple->Fill();
   }
   auto model2 = RNTupleModel::Create();
   auto fieldIntVec = model2->MakeField<std::vector<int>>("intVec");
   auto fieldFloatVecVec = model2->MakeField<std::vector<std::vector<float>>>("floatVecVec");
   auto fieldBoolVecVec = model2->MakeField<std::vector<std::vector<bool>>>("booleanVecVec");
   auto ntuple2 = RNTupleReader::Open(std::move(model2), ntupleName, rootFileName);

   std::ostringstream os;
   ntuple2->Show(0, ROOT::Experimental::ENTupleShowFormat::kCurrentModelJSON, os);
   // clang-format off
   std::string fString{ std::string("")
      + "{\n"
      + "  \"intVec\": [4, 5, 6],\n"
      + "  \"floatVecVec\": [[0.1, 0.2], [1.1, 1.2]],\n"
      + "  \"booleanVecVec\": [[false, true, false], [false, true], [true, false, false]]\n"
      + "}\n" };
   // clang-format on
   EXPECT_EQ(fString, os.str());

   std::ostringstream os1;
   ntuple2->Show(1, ROOT::Experimental::ENTupleShowFormat::kCurrentModelJSON, os1);
   // clang-format off
   std::string fString1{ std::string("")
      + "{\n"
      + "  \"intVec\": [4, 5, 6, 7],\n"
      + "  \"floatVecVec\": [[0.1, 0.2], [1.1, 1.2], [2.2, 2.3]],\n"
      + "  \"booleanVecVec\": [[false, true, false], [false, true], [true, false, false], [false, true]]\n"
      + "}\n" };
   // clang-format on
   EXPECT_EQ(fString1, os1.str());
}

TEST(RNTupleShow, Arrays)
{
   std::string rootFileName{"test_ntuple_show_array.root"};
   std::string ntupleName{"Arrays"};
   FileRaii fileGuard(rootFileName);
   {
      auto model = RNTupleModel::Create();
      auto Intarrayfield = model->MakeField<std::array<int, 2>>("IntArray");
      auto Floatarrayfield = model->MakeField<std::array<float, 3>>("FloatArray");
      auto Vecarrayfield = model->MakeField<std::array<std::vector<double>, 4>>("ArrayOfVec");
      auto StringArray = model->MakeField<std::array<std::string, 2>>("stringArray");
      auto arrayOfArray = model->MakeField<std::array<std::array<bool, 2>, 3>>("ArrayOfArray");
      auto arrayVecfield = model->MakeField<std::vector<std::array<float, 2>>>("VecOfArray");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), ntupleName, rootFileName);

      *Intarrayfield = {1, 3};
      *Floatarrayfield = {3.5f, 4.6f, 5.7f};
      *Vecarrayfield = {std::vector<double>{1, 2}, std::vector<double>{4, 5}, std::vector<double>{7, 8, 9},
                        std::vector<double>{11}};
      *StringArray = {"First", "Second"};
      *arrayOfArray = {std::array<bool, 2>{true, false}, std::array<bool, 2>{false, true},
                       std::array<bool, 2>{false, false}};
      *arrayVecfield = {std::array<float, 2>{0, 1}, std::array<float, 2>{2, 3}, std::array<float, 2>{4, 5}};
      ntuple->Fill();

      *Intarrayfield = {2, 5};
      *Floatarrayfield = {2.3f, 5.7f, 11.13f};
      *Vecarrayfield = {std::vector<double>{17, 19}, std::vector<double>{23, 29}, std::vector<double>{31, 37, 41},
                        std::vector<double>{43}};
      *StringArray = {"Third", "Fourth"};
      *arrayOfArray = {std::array<bool, 2>{true, true}, std::array<bool, 2>{false, true},
                       std::array<bool, 2>{true, true}};
      *arrayVecfield = {std::array<float, 2>{6, 7}, std::array<float, 2>{8, 9}};
      ntuple->Fill();
   }
   auto model2 = RNTupleModel::Create();
   auto Intarrayfield = model2->MakeField<std::array<int, 2>>("IntArray");
   auto Floatarrayfield = model2->MakeField<std::array<float, 3>>("FloatArray");
   auto Vecarrayfield = model2->MakeField<std::array<std::vector<double>, 4>>("ArrayOfVec");
   auto StringArray = model2->MakeField<std::array<std::string, 2>>("stringArray");
   auto arrayOfArray = model2->MakeField<std::array<std::array<bool, 2>, 3>>("ArrayOfArray");
   auto arrayVecfield = model2->MakeField<std::vector<std::array<float, 2>>>("VecOfArray");
   auto ntuple2 = RNTupleReader::Open(std::move(model2), ntupleName, rootFileName);

   std::ostringstream os;
   ntuple2->Show(0, ROOT::Experimental::ENTupleShowFormat::kCurrentModelJSON, os);
   // clang-format off
   std::string fString{ std::string("")
      + "{\n"
      + "  \"IntArray\": [1, 3],\n"
      + "  \"FloatArray\": [3.5, 4.6, 5.7],\n"
      + "  \"ArrayOfVec\": [[1, 2], [4, 5], [7, 8, 9], [11]],\n"
      + "  \"stringArray\": [\"First\", \"Second\"],\n"
      + "  \"ArrayOfArray\": [[true, false], [false, true], [false, false]],\n"
      + "  \"VecOfArray\": [[0, 1], [2, 3], [4, 5]]\n"
      + "}\n"};
   // clang-format on
   EXPECT_EQ(fString, os.str());

   std::ostringstream os1;
   ntuple2->Show(1, ROOT::Experimental::ENTupleShowFormat::kCurrentModelJSON, os1);
   // clang-format off
   std::string fString1{ std::string("")
      + "{\n"
      + "  \"IntArray\": [2, 5],\n"
      + "  \"FloatArray\": [2.3, 5.7, 11.13],\n"
      + "  \"ArrayOfVec\": [[17, 19], [23, 29], [31, 37, 41], [43]],\n"
      + "  \"stringArray\": [\"Third\", \"Fourth\"],\n"
      + "  \"ArrayOfArray\": [[true, true], [false, true], [true, true]],\n"
      + "  \"VecOfArray\": [[6, 7], [8, 9]]\n"
      + "}\n"};
   // clang-format on
   EXPECT_EQ(fString1, os1.str());
}

TEST(RNTupleShow, Objects)
{
   std::string rootFileName{"test_ntuple_show_object.root"};
   std::string ntupleName{"Objects"};
   FileRaii fileGuard(rootFileName);
   {
      auto model = RNTupleModel::Create();
      auto customStructfield = model->MakeField<CustomStruct>("CustomStruct");
      auto customStructVec = model->MakeField<std::vector<CustomStruct>>("CustomStructVec");
      auto customStructArray = model->MakeField<std::array<CustomStruct, 2>>("CustomStructArray");
      auto derivedAfield = model->MakeField<DerivedA>("DerivedA");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), ntupleName, rootFileName);

      *customStructfield =
         CustomStruct{4.1f, std::vector<float>{0.1f, 0.2f, 0.3f},
                      std::vector<std::vector<float>>{{1.1f, 1.2f, 1.3f}, {2.1f, 2.2f, 2.3f}}, "Example1String"};
      *customStructVec = {
         CustomStruct{4.2f, std::vector<float>{0.1f, 0.2f, 0.3f},
                      std::vector<std::vector<float>>{{1.1f, 1.3f}, {2.1f, 2.2f, 2.3f}}, "Example2String"},
         CustomStruct{4.3f, std::vector<float>{0.1f, 0.2f, 0.3f},
                      std::vector<std::vector<float>>{{1.1f, 1.2f, 1.3f}, {2.1f, 2.3f}}, "Example3String"},
         CustomStruct{4.4f, std::vector<float>{0.1f, 0.3f},
                      std::vector<std::vector<float>>{{1.1f, 1.2f, 1.3f}, {2.1f, 2.2f, 2.3f}}, "Example4String"}};
      *customStructArray = {
         CustomStruct{4.5f, std::vector<float>{0.1f, 0.2f, 0.3f},
                      std::vector<std::vector<float>>{{1.1f, 1.3f}, {2.1f, 2.2f, 2.3f}}, "AnotherString1"},
         CustomStruct{4.6f, std::vector<float>{0.1f, 0.2f, 0.3f},
                      std::vector<std::vector<float>>{{1.1f, 1.2f, 1.3f}, {2.1f, 2.3f}}, "AnotherString2"}};
      *derivedAfield = {};
      ntuple->Fill();
   }
   auto model2 = RNTupleModel::Create();
   auto customStructfield = model2->MakeField<CustomStruct>("CustomStruct");
   auto customStructVec = model2->MakeField<std::vector<CustomStruct>>("CustomStructVec");
   auto customStructArray = model2->MakeField<std::array<CustomStruct, 2>>("CustomStructArray");
   auto derivedAfield = model2->MakeField<DerivedA>("DerivedA");
   auto ntuple2 = RNTupleReader::Open(std::move(model2), ntupleName, rootFileName);

   std::ostringstream os;
   ntuple2->Show(0, ROOT::Experimental::ENTupleShowFormat::kCurrentModelJSON, os);
   // clang-format off
   std::string fString{ std::string("")
      + "{\n"
      + "  \"CustomStruct\": {\n"
      + "    \"a\": 4.1,\n"
      + "    \"v1\": [0.1, 0.2, 0.3],\n"
      + "    \"v2\": [[1.1, 1.2, 1.3], [2.1, 2.2, 2.3]],\n"
      + "    \"s\": \"Example1String\"\n"
      + "  },\n"
      + "  \"CustomStructVec\": [{\"a\": 4.2, \"v1\": [0.1, 0.2, 0.3], \"v2\": [[1.1, 1.3], [2.1, 2.2, 2.3]], "
      +      "\"s\": \"Example2String\"}, {\"a\": 4.3, \"v1\": [0.1, 0.2, 0.3], "
      +      "\"v2\": [[1.1, 1.2, 1.3], [2.1, 2.3]], \"s\": \"Example3String\"}, "
      +      "{\"a\": 4.4, \"v1\": [0.1, 0.3], \"v2\": [[1.1, 1.2, 1.3], [2.1, 2.2, 2.3]], "
      +      "\"s\": \"Example4String\"}],\n"
      + "  \"CustomStructArray\": [{\"a\": 4.5, \"v1\": [0.1, 0.2, 0.3], \"v2\": [[1.1, 1.3], [2.1, 2.2, 2.3]], "
      +      "\"s\": \"AnotherString1\"}, {\"a\": 4.6, \"v1\": [0.1, 0.2, 0.3], "
      +      "\"v2\": [[1.1, 1.2, 1.3], [2.1, 2.3]], \"s\": \"AnotherString2\"}],\n"
      + "  \"DerivedA\": {\n"
      + "    \":_0\": {\n"
      + "      \"a\": 0,\n"
      + "      \"v1\": [],\n"
      + "      \"v2\": [],\n"
      + "      \"s\": \"\"\n"
      + "    },\n"
      + "    \"a_v\": [],\n"
      + "    \"a_s\": \"\"\n"
      + "  }\n"
      + "}\n" };
   // clang-format on
   EXPECT_EQ(fString, os.str());
}

TEST(RNTupleShow, Collections)
{
   std::string rootFileName{"test_ntuple_show_collection.root"};
   std::string ntupleName{"Collections"};
   FileRaii fileGuard(rootFileName);
   {
      auto model = RNTupleModel::Create();
      auto collection_model = RNTupleModel::Create();
      auto int_field = collection_model->MakeField<int>("myInt");
      auto float_field = collection_model->MakeField<float>("myFloat");
      auto collection = model->MakeCollection("collection", std::move(collection_model));
      auto ntuple = RNTupleWriter::Recreate(std::move(model), ntupleName, rootFileName);
      *int_field = 0;
      *float_field = 10.0;
      collection->Fill();
      *int_field = 1;
      *float_field = 20.0;
      collection->Fill();
      ntuple->Fill();
   }

   auto ntuple = RNTupleReader::Open(ntupleName, rootFileName);
   std::ostringstream osData;
   ntuple->Show(0, ROOT::Experimental::ENTupleShowFormat::kCompleteJSON, osData);
   // clang-format off
   std::string outputData{ std::string("")
      + "{\n"
      + "  \"collection\": [{\"myInt\": 0, \"myFloat\": 10}, {\"myInt\": 1, \"myFloat\": 20}]\n"
      + "}\n" };
   // clang-format on
   EXPECT_EQ(outputData, osData.str());

   std::ostringstream osFields;
   ntuple->PrintInfo(ROOT::Experimental::ENTupleInfo::kSummary, osFields);
   // clang-format off
   std::string outputFields{ std::string("")
      + "************************************ NTUPLE ************************************\n"
      + "* N-Tuple : Collections                                                        *\n"
      + "* Entries : 1                                                                  *\n"
      + "********************************************************************************\n"
      + "* Field 1           : collection (std::vector<>)                               *\n"
      + "*   Field 1.1       : _0                                                       *\n"
      + "*     Field 1.1.1   : myInt (std::int32_t)                                     *\n"
      + "*     Field 1.1.2   : myFloat (float)                                          *\n"
      + "********************************************************************************\n" };
   // clang-format on
   EXPECT_EQ(outputFields, osFields.str());
}

TEST(RNTupleShow, RVec)
{
   std::string rootFileName{"test_ntuple_show_rvec.root"};
   std::string ntupleName{"r"};
   FileRaii fileGuard(rootFileName);

   auto modelWrite = RNTupleModel::Create();
   auto fieldIntVec = modelWrite->MakeField<ROOT::RVec<int>>("intVec");
   auto fieldFloatVecVec = modelWrite->MakeField<ROOT::RVec<ROOT::RVec<float>>>("floatVecVec");
   auto fieldCustomStructVec = modelWrite->MakeField<ROOT::RVec<CustomStruct>>("customStructVec");

   auto modelRead = modelWrite->Clone();
   {
      auto ntuple = RNTupleWriter::Recreate(std::move(modelWrite), ntupleName, rootFileName);

      *fieldIntVec = ROOT::RVec<int>{1, 2, 3};
      *fieldFloatVecVec = ROOT::RVec<ROOT::RVec<float>>{ROOT::RVec<float>{0.1, 0.2}, ROOT::RVec<float>{1.1, 1.2}};
      *fieldCustomStructVec = ROOT::RVec<CustomStruct>{{}, {1.f, {2.f, 3.f}, {{4.f}, {5.f}}, "foo"}};
      ntuple->Fill();

      fieldIntVec->emplace_back(4);
      fieldFloatVecVec->emplace_back(ROOT::RVec<float>{2.1, 2.2});
      fieldCustomStructVec->emplace_back(CustomStruct{6.f, {7.f, 8.f}, {{9.f}, {10.f}}, "bar"});
      ntuple->Fill();
   }

   auto r = RNTupleReader::Open(std::move(modelRead), ntupleName, rootFileName);

   std::ostringstream os;
   r->Show(0, ROOT::Experimental::ENTupleShowFormat::kCurrentModelJSON, os);
   // clang-format off
   std::string expected1{std::string("")
      + "{\n"
      + "  \"intVec\": [1, 2, 3],\n"
      + "  \"floatVecVec\": [[0.1, 0.2], [1.1, 1.2]],\n"
      + "  \"customStructVec\": [{\"a\": 0, \"v1\": [], \"v2\": [], \"s\": \"\"},"
      + " {\"a\": 1, \"v1\": [2, 3], \"v2\": [[4], [5]], \"s\": \"foo\"}]\n"
      + "}\n"};
   // clang-format on
   EXPECT_EQ(os.str(), expected1);

   std::ostringstream os2;
   r->Show(1, ROOT::Experimental::ENTupleShowFormat::kCurrentModelJSON, os2);
   // clang-format off
   std::string expected2{std::string("")
      + "{\n"
      + "  \"intVec\": [1, 2, 3, 4],\n"
      + "  \"floatVecVec\": [[0.1, 0.2], [1.1, 1.2], [2.1, 2.2]],\n"
      + "  \"customStructVec\": [{\"a\": 0, \"v1\": [], \"v2\": [], \"s\": \"\"},"
      + " {\"a\": 1, \"v1\": [2, 3], \"v2\": [[4], [5]], \"s\": \"foo\"},"
      + " {\"a\": 6, \"v1\": [7, 8], \"v2\": [[9], [10]], \"s\": \"bar\"}]\n"
      + "}\n"};
   // clang-format on
   EXPECT_EQ(os2.str(), expected2);
}

TEST(RNTupleShow, RVecTypeErased)
{
   std::string rootFileName{"test_ntuple_show_rvec_typeerased.root"};
   std::string ntupleName{"r"};
   FileRaii fileGuard(rootFileName);
   {
      auto m = RNTupleModel::Create();

      auto intVecField = RFieldBase::Create("intVec", "ROOT::VecOps::RVec<int>").Unwrap();
      m->AddField(std::move(intVecField));

      auto floatVecVecField =
         RFieldBase::Create("floatVecVec", "ROOT::VecOps::RVec<ROOT::VecOps::RVec<float>>").Unwrap();
      m->AddField(std::move(floatVecVecField));

      auto customStructField = RFieldBase::Create("customStructVec", "ROOT::VecOps::RVec<CustomStruct>").Unwrap();
      m->AddField(std::move(customStructField));

      ROOT::RVec<int> intVec = {1, 2, 3};
      ROOT::RVec<ROOT::RVec<float>> floatVecVec{ROOT::RVec<float>{0.1, 0.2}, ROOT::RVec<float>{1.1, 1.2}};
      ROOT::RVec<CustomStruct> customStructVec{{}, {1.f, {2.f, 3.f}, {{4.f}, {5.f}}, "foo"}};

      m->Freeze();
      m->GetDefaultEntry()->CaptureValueUnsafe("intVec", &intVec);
      m->GetDefaultEntry()->CaptureValueUnsafe("floatVecVec", &floatVecVec);
      m->GetDefaultEntry()->CaptureValueUnsafe("customStructVec", &customStructVec);

      auto ntuple = RNTupleWriter::Recreate(std::move(m), ntupleName, rootFileName);

      ntuple->Fill();

      intVec.emplace_back(4);
      floatVecVec.emplace_back(ROOT::RVec<float>{2.1, 2.2});
      customStructVec.emplace_back(CustomStruct{6.f, {7.f, 8.f}, {{9.f}, {10.f}}, "bar"});
      ntuple->Fill();
   }

   auto ntuple = RNTupleReader::Open(ntupleName, rootFileName);

   std::ostringstream os;
   ntuple->Show(0, ROOT::Experimental::ENTupleShowFormat::kCompleteJSON, os);
   // clang-format off
   std::string fString{std::string("")
      + "{\n"
      + "  \"intVec\": [1, 2, 3],\n"
      + "  \"floatVecVec\": [[0.1, 0.2], [1.1, 1.2]],\n"
      + "  \"customStructVec\": [{\"a\": 0, \"v1\": [], \"v2\": [], \"s\": \"\"},"
      + " {\"a\": 1, \"v1\": [2, 3], \"v2\": [[4], [5]], \"s\": \"foo\"}]\n"
      + "}\n"};
   // clang-format on
   EXPECT_EQ(fString, os.str());

   std::ostringstream os1;
   ntuple->Show(1, ROOT::Experimental::ENTupleShowFormat::kCompleteJSON, os1);
   // clang-format off
   std::string fString1{std::string("")
      + "{\n"
      + "  \"intVec\": [1, 2, 3, 4],\n"
      + "  \"floatVecVec\": [[0.1, 0.2], [1.1, 1.2], [2.1, 2.2]],\n"
      + "  \"customStructVec\": [{\"a\": 0, \"v1\": [], \"v2\": [], \"s\": \"\"},"
      + " {\"a\": 1, \"v1\": [2, 3], \"v2\": [[4], [5]], \"s\": \"foo\"},"
      + " {\"a\": 6, \"v1\": [7, 8], \"v2\": [[9], [10]], \"s\": \"bar\"}]\n"
      + "}\n"};
   // clang-format off
   EXPECT_EQ(fString1, os1.str());
}

TEST(RNTupleShow, CollectionProxy)
{
   SimpleCollectionProxy<StructUsingCollectionProxy<float>> proxy;
   auto klass = TClass::GetClass("StructUsingCollectionProxy<float>");
   klass->CopyCollectionProxy(proxy);

   FileRaii fileGuard("test_ntuple_show_collectionproxy.ntuple");
   {
      auto model = RNTupleModel::Create();
      auto proxyF = model->MakeField<StructUsingCollectionProxy<float>>("proxyF");
      auto vecProxyF = model->MakeField<std::vector<StructUsingCollectionProxy<float>>>("vecProxyF");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "f", fileGuard.GetPath());
      proxyF->v.push_back(42.0f);
      proxyF->v.push_back(24.0f);
      StructUsingCollectionProxy<float> floats;
      floats.v.push_back(1.0f);
      floats.v.push_back(2.0f);
      vecProxyF->push_back(floats);
      vecProxyF->push_back(floats);
      ntuple->Fill();
   }

   {
      auto ntuple = RNTupleReader::Open("f", fileGuard.GetPath());
      EXPECT_EQ(1U, ntuple->GetNEntries());

      std::ostringstream os;
      ntuple->Show(0, ROOT::Experimental::ENTupleShowFormat::kCompleteJSON, os);
      // clang-format off
      std::string expected{std::string("")
         + "{\n"
         + "  \"proxyF\": [42, 24],\n"
         + "  \"vecProxyF\": [[1, 2], [1, 2]]\n"
         + "}\n"};
      // clang-format on
      EXPECT_EQ(os.str(), expected);
   }
}

TEST(RNTupleShow, CompleteOrCurrentModel)
{
   FileRaii fileGuard("test_ntuple_show_completeorcurrentmodel.root");
   {
      auto model = RNTupleModel::Create();
      auto fldInt = model->MakeField<int>("int");
      auto fldFloat = model->MakeField<float>("float");
      auto ntuple = RNTupleWriter::Recreate(std::move(model), "f", fileGuard.GetPath());

      *fldInt = 42;
      *fldFloat = 3.14;
      *fldFloat = ntuple->Fill();
   }

   auto ntuple1 = RNTupleReader::Open("f", fileGuard.GetPath());

   auto model = RNTupleModel::Create();
   auto fldInt = model->MakeField<int>("int");
   auto ntuple2 = RNTupleReader::Open(std::move(model), "f", fileGuard.GetPath());

   std::ostringstream os1;
   ntuple1->Show(0, ROOT::Experimental::ENTupleShowFormat::kCurrentModelJSON, os1);
   // clang-format off
   std::string expected1{std::string("")
      + "{\n"
      + "  \"int\": 42,\n"
      + "  \"float\": 3.14\n"
      + "}\n"};
   // clang-format on
   EXPECT_EQ(expected1, os1.str());

   std::ostringstream os2;
   ntuple2->Show(0, ROOT::Experimental::ENTupleShowFormat::kCurrentModelJSON, os2);
   // clang-format off
   std::string expected2{std::string("")
      + "{\n"
      + "  \"int\": 42\n"
      + "}\n"};
   // clang-format on
   EXPECT_EQ(expected2, os2.str());

   std::ostringstream os3;
   ntuple2->Show(0, ROOT::Experimental::ENTupleShowFormat::kCompleteJSON, os3);
   // clang-format off
   std::string expected3{std::string("")
      + "{\n"
      + "  \"int\": 42,\n"
      + "  \"float\": 3.14\n"
      + "}\n"};
   // clang-format on
   EXPECT_EQ(expected3, os3.str());
}
