#include "Software/Execute.hpp"

void Software::Execute(const po::variables_map& vm){

    Software::ConfigProbesStruct probes;
    Software::SettingsStruct settings;
    Software::HelperStruct GlobalHelper;
    std::vector<Software::SharedDataStruct> SharedData;
    std::vector<Software::ThreadSimulationStruct> GlobalThreadSimulations;

    std::string config_file_name = vm["configfile"].as<std::string>();
    Settings settings2(config_file_name, false);

    std::cout << "Start Software Leakage Evaluation" << std::endl << std::endl;
    Software::Prepare::All(vm, probes, settings, settings2, SharedData, GlobalHelper, GlobalThreadSimulations);
    double maximum_leakage = Software::Analyze::All(settings, SharedData, GlobalHelper, GlobalThreadSimulations);
    std::cout << "done!" << std::endl;
}
