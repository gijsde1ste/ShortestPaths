#include <stdio.h>
#include <definition.h>
#include <SimpleShortestPathExperiment.h>
#include <SptExperiment.h>
#include <SparseShortestPathTree.h>
#include <SptInternal.h>
#include <Renderer.h>

#include <Triangulation.h>
#include <SsptTriangulation.h>
#include <SptInternalTriangulation.h>


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
    } else if (type == "internal"){
        SptExperiment experiment;
        experiment.run(argc, argv);
    } else if (type == "sspt") {

        // Temporary development default thingy
        std::vector<Point_2> targets = getTargets();

        // Remove temp files from possibly crashed last run
        remove("leafToRoot.tpie");
        SsptTriangulation t;
        t.open("3x3.tpie");
        t.createPath(targets);
        t.close();

        t.open("leafToRoot.tpie");
        Node n = t.getRoot();
        t.setPathProgress(n.id);
        SparseShortestPathTree sspt(t.SplitVertices);

        sspt.extendStart(n, t.getNextEdge(&sspt));
        int count = 0;
        while(!t.finished()){
            Edge e = t.getNextEdge(&sspt);
            //if (count == 77) break;
            sspt.extend(e);
            count++;
        }
        //sspt.printFunnel();
        sspt.out.close();

        Renderer r;
        r.drawStart();
        r.draw(&sspt, t.copyPolygon(), targets, t.SplitVertices);
        r.drawEnd();

        //std::cout << sspt.geodesic->size() << std::endl;

        t.close();
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
