#include <definition.h>
#include <SimpleShortestPathExperiment.h>
#include <Renderer.h>

#include <Triangulation.h>


std::vector<Point_2> getTargets(){
    std::vector<Point_2> targets;
	tpie::uncompressed_stream<Point_2> in;
	in.open("targets.tpie");
	while (in.can_read()){
        targets.push_back(in.read());
	}
	in.close();

	return targets;
}

void run(int argc, char** argv){

    std::string type = "shortestPath";
    if (argc > 1) type = argv[1];

    if (type == "shortestPath"){
        SimpleShortestPathExperiment experiment;
        experiment.run(argc, argv);
    } else if (type == "debug") {
        std::vector<Point_2> targets = getTargets();
        for (auto i = targets.begin(); i != targets.end(); ++i){
            std::cout << (*i).x << " " << (*i).y << std::endl;
        }


        tpie::uncompressed_stream<Node> in;
        in.open("leafToRoot.tpie");

        while(in.can_read()){
            Node n = in.read();
            std::cout << n.id << " " << n.postOrder << " " << n.leftChild << " " << n.rightChild << std::endl;
        }
        in.close();
    } else {
        // Temporary development default thingy
        std::vector<Point_2> targets = getTargets();

        Triangulation t;
        t.open("128.tpie");

        t.createPath(targets);
    }
}

int main(int argc, char** argv) {

    tpie::tpie_init();

	size_t available_memory_mb = 128;
	tpie::get_memory_manager().set_limit(available_memory_mb*1024*1024);

	// Using things in the same scope as tpie_finish gives errors so move it to different method
    run(argc, argv);

    tpie::tpie_finish();

    std::cout << "Finished" << std::endl;

	return 0;
}
