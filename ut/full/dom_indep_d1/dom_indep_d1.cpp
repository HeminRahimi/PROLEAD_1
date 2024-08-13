#include <catch2/catch.hpp>

#include "Hardware/Adversaries.hpp"
#include "Hardware/Prepare.hpp"

class Test {
 public:
  Test(double maximum_leakage, uint64_t number_of_propagations,
       uint64_t number_of_standard_probes, uint64_t number_of_probe_extensions,
       uint64_t number_of_probing_sets)
      : maximum_leakage_(maximum_leakage),
        number_of_propagations_(number_of_propagations),
        number_of_standard_probes_(number_of_standard_probes),
        number_of_probe_extensions_(number_of_probe_extensions),
        number_of_probing_sets_(number_of_probing_sets) {}

  bool Require(bool leakage_expected, uint64_t number_of_propagations,
               uint64_t number_of_standard_probes,
               uint64_t number_of_probe_extensions,
               uint64_t number_of_probing_sets) {
    if (leakage_expected && maximum_leakage_ < 5.0) {
      return false;
    } else if (!leakage_expected && maximum_leakage_ >= 5.0) {
      return false;
    } else if (number_of_propagations != number_of_propagations_) {
      return false;
    } else if (number_of_standard_probes != number_of_standard_probes_) {
      return false;
    } else if (number_of_probe_extensions != number_of_probe_extensions_) {
      return false;
    } else if (number_of_probing_sets != number_of_probing_sets_) {
      return false;
    } else {
      return true;
    }
  }

 private:
  double maximum_leakage_;
  uint64_t number_of_propagations_;
  uint64_t number_of_standard_probes_;
  uint64_t number_of_probe_extensions_;
  uint64_t number_of_probing_sets_;
};

Test TestDomIndepd1(Settings& settings) {
  const std::string library_file_name = "library.json";
  const std::string library_name = "NANG45";
  const std::string design_file_name = "ut/full/dom_indep_d1/dom_indep_d1.v";
  const std::string topmodule_name = "dom_indep_d1";
  const std::string result_folder_name = "ut/full/dom_indep_d1";

  Hardware::CircuitStruct circuit;
  Hardware::Library library(library_file_name, library_name,
                            settings.IsRelaxedModel());
  Hardware::Read::DesignFile(design_file_name, topmodule_name, settings,
                             library, &circuit, 0, 0, 0, NULL);
  circuit.SetIsProbeAllowed(library, settings);
  circuit.SetIsExtensionAllowed(library, settings);
  circuit.SetIsAnalysisAllowed(settings);
  circuit.SetIsFaultAllowed(settings);
  std::cout << "done" << std::endl;

  Hardware::Prepare::MakeCircuitDepth(library, &circuit);

  std::vector<SharedData> shared_data(settings.GetNumberOfThreads(),
                                      SharedData(circuit, settings));
  Simulation simulation(circuit, settings);
  simulation.topmodule_name_ = topmodule_name;
  simulation.result_folder_name_ = result_folder_name;

  if (settings.IsRelaxedModel()) {
    Hardware::Adversaries<Hardware::RelaxedProbe> adversary(
        library, circuit, settings, simulation);
    double maximum_leakage =
        adversary.EvaluateRobustProbingSecurity(shared_data);
    return Test(maximum_leakage, adversary.GetNumberOfPropagations(),
                adversary.GetNumberOfStandardProbes(),
                adversary.GetNumberOfExtendedProbes(),
                adversary.GetNumberOfProbingSets());
  } else {
    Hardware::Adversaries<Hardware::RobustProbe> adversary(
        library, circuit, settings, simulation);
    double maximum_leakage =
        adversary.EvaluateRobustProbingSecurity(shared_data);
    return Test(maximum_leakage, adversary.GetNumberOfPropagations(),
                adversary.GetNumberOfStandardProbes(),
                adversary.GetNumberOfExtendedProbes(),
                adversary.GetNumberOfProbingSets());
  }
}

TEST_CASE("Test full verification (dom_indep_d1)", "[dom_indep_d1]") {
  const std::string config_file_name = "ut/full/dom_indep_d1/dom_indep_d1.json";

  Settings settings(config_file_name, true);

  SECTION("Robust Probing Model with no minimization in normal mode") {
    settings.SetMinimization(Minimization::none);
    settings.SetRelaxedModel(false);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 8, 16, 14, 16));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 8, 16, 14, 56));
    }
  }

  SECTION("Robust Probing Model with trivial minimization in normal mode") {
    settings.SetMinimization(Minimization::trivial);
    settings.SetRelaxedModel(false);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 4, 8, 14, 8));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 4, 8, 14, 12));
    }
  }

  SECTION("Robust Probing Model with aggressive minimization in normal mode") {
    settings.SetMinimization(Minimization::aggressive);
    settings.SetRelaxedModel(false);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 4, 8, 14, 4));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 4, 8, 14, 6));
    }
  }

  SECTION(
      "Robust but Relaxed Probing Model with no minimization in normal mode") {
    settings.SetMinimization(Minimization::none);
    settings.SetRelaxedModel(true);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 30, 16, 30, 16));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 30, 16, 30, 56));
    }
  }

  SECTION(
      "Robust but Relaxed Probing Model with trivial minimization in normal "
      "mode") {
    settings.SetMinimization(Minimization::trivial);
    settings.SetRelaxedModel(true);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 30, 8, 30, 8));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 30, 8, 30, 12));
    }
  }

  SECTION(
      "Robust but Relaxed Probing Model with aggressive minimization in normal "
      "mode") {
    settings.SetMinimization(Minimization::aggressive);
    settings.SetRelaxedModel(true);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 30, 8, 30, 8));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 30, 8, 30, 12));
    }
  }

  SECTION("Robust Probing Model with no minimization in compact mode") {
    settings.SetMinimization(Minimization::none);
    settings.SetCompactDistribution(true);
    settings.SetRelaxedModel(false);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 8, 16, 14, 16));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 8, 16, 14, 56));
    }
  }

  SECTION("Robust Probing Model with trivial minimization in compact mode") {
    settings.SetMinimization(Minimization::trivial);
    settings.SetCompactDistribution(true);
    settings.SetRelaxedModel(false);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 4, 8, 14, 8));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 4, 8, 14, 12));
    }
  }

  SECTION("Robust Probing Model with aggressive minimization in compact mode") {
    settings.SetMinimization(Minimization::aggressive);
    settings.SetCompactDistribution(true);
    settings.SetRelaxedModel(false);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 4, 8, 14, 4));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 4, 8, 14, 6));
    }
  }

  SECTION(
      "Robust but Relaxed Probing Model with no minimization in compact mode") {
    settings.SetMinimization(Minimization::none);
    settings.SetCompactDistribution(true);
    settings.SetRelaxedModel(true);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 30, 16, 30, 16));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 30, 16, 30, 56));
    }
  }

  SECTION(
      "Robust but Relaxed Probing Model with trivial minimization in compact "
      "mode") {
    settings.SetMinimization(Minimization::trivial);
    settings.SetCompactDistribution(true);
    settings.SetRelaxedModel(true);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 30, 8, 30, 8));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 30, 8, 30, 12));
    }
  }

  SECTION(
      "Robust but Relaxed Probing Model with aggressive minimization in "
      "compact mode") {
    settings.SetMinimization(Minimization::aggressive);
    settings.SetCompactDistribution(true);
    settings.SetRelaxedModel(true);
    settings.SetTestOrder(GENERATE(1, 2));

    Test test = TestDomIndepd1(settings);

    if (settings.GetTestOrder() == 1) {
      // We expect no leakage in the first order.
      REQUIRE(test.Require(false, 30, 8, 30, 8));
    } else {
      // But we expect leakage in the second order.
      REQUIRE(test.Require(true, 30, 8, 30, 12));
    }
  }
}
