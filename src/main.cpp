#include <definition.h>
#include <SimpleShortestPathExperiment.h>
#include <Renderer.h>


int main(int argc, char** argv) {

    std::string type = "shortestPath";
    if (argc > 1) type = argv[1];

    tpie::tpie_init();

	size_t available_memory_mb = 128;
	tpie::get_memory_manager().set_limit(available_memory_mb*1024*1024);

    if (type == "shortestPath"){
        SimpleShortestPathExperiment experiment;
        experiment.run(argc, argv);
    }

    tpie::tpie_finish();

    std::cout << "Finished" << std::endl;

	return 0;
}

