#include "SimpleShortestPathExperiment.h"

SimpleShortestPathExperiment::SimpleShortestPathExperiment()
{
    //ctor
}

void SimpleShortestPathExperiment::run(int argc, char** argv){
    // handle some arguments
    experimentName = "test";
    if (argc > 2) experimentName = argv[2];

    std::string file = experimentName + ".tpie";

    // mode 0 is I/O, 1 is internal, 2 is run to generate random source/target points and write them to file
    // next runs will use these points so we can time them properly
    int mode = 0;
    if (argc > 3) mode = std::stoi(argv[3]);

    std::vector<Point_2> targets;
    if (mode != 2){
        targets = getTargets();
    }

	switch (mode){
        case 0:
            experimentName += "IO";
            testIOFunnel(file, targets);
            writeResults();
            break;
        case 1:
            experimentName += "Internal";
            testInternalFunnel(file, targets);
            writeResults();
            break;
        case 2:
            generateTargets(file);
            break;
        default:
            generateTargets(file);
            break;
    }
}

void SimpleShortestPathExperiment::generateTargets(std::string file){
    Triangulation t;
    t.open(file);

    std::vector<Point_2> targets;
    for (int i = 0; i < 10; i++){
        targets.push_back(t.getRandomPoint());
    }
    t.close();

    tpie::uncompressed_stream<Point_2> out;
    out.open("targets.tpie");
    out.truncate(0);
    out.write(targets.begin(), targets.end());
    out.close();
}

void SimpleShortestPathExperiment::testIOFunnel(std::string file, std::vector<Point_2> targets){
    std::chrono::microseconds duration;
    auto startTotal = std::chrono::high_resolution_clock::now();

    Triangulation t;
    t.open(file);

    //funnel f(tpie::file_base_crtp<Point_2>::calculate_block_factor(4096));
    funnel f;
    for (auto i = targets.begin(); i != targets.end(); i++){
        std::cout << "Target: " << (*i).x << " " << (*i).y  << std::endl;
        auto startPath = std::chrono::high_resolution_clock::now();
        t.createPath(*i);
        auto stopPath = std::chrono::high_resolution_clock::now();

        timings.push_back(Timing(startPath, stopPath, "Path"));

        auto startFunnel = std::chrono::high_resolution_clock::now();
        f.extendStart(t.getRoot(), t.getNextEdge());
        while (!t.finished()){
            f.extend(t.getNextEdge());
        }
        f.extendFinalStep(*i);
        auto stopFunnel = std::chrono::high_resolution_clock::now();

        timings.push_back(Timing(startFunnel, stopFunnel, "Funnel"));

        f.reset();
    }


    //std::vector<Triangle> triangles = t.copyPolygon();

    t.close();

    auto stopTotal = std::chrono::high_resolution_clock::now();

    timings.push_back(Timing(startTotal, stopTotal, "Total"));

    //Renderer renderer;
    //renderer.draw(f, triangles);
}

void SimpleShortestPathExperiment::testInternalFunnel(std::string file, std::vector<Point_2> targets){

    std::chrono::microseconds duration;
    auto startTotal = std::chrono::high_resolution_clock::now();

    internal_triangulation t;
    t.open(file);

    internal_funnel f = internal_funnel(t.size());
    for (auto i = targets.begin(); i != targets.end(); i++){
        std::cout << "Target: " << (*i).x << " " << (*i).y  << std::endl;

        auto startPath = std::chrono::high_resolution_clock::now();
        t.createPath(*i);
        auto stopPath = std::chrono::high_resolution_clock::now();

        timings.push_back(Timing(startPath, stopPath, "Path"));

        auto startFunnel = std::chrono::high_resolution_clock::now();
        f.extendStart(t.getRoot(), t.getNextEdge());
        while (!t.finished()){
            f.extend(t.getNextEdge());
        }
        f.extendFinalStep(*i);
        auto stopFunnel = std::chrono::high_resolution_clock::now();

        timings.push_back(Timing(startFunnel, stopFunnel, "Funnel"));

        f.reset(t.size());
    }

    auto stopTotal = std::chrono::high_resolution_clock::now();

    timings.push_back(Timing(startTotal, stopTotal, "Total"));

    //std::vector<Triangle> triangles = t.copyPolygon();


    //Renderer renderer;
    //renderer.draw(f, triangles);
}

std::vector<Point_2> SimpleShortestPathExperiment::getTargets(){
    std::vector<Point_2> targets;
	tpie::uncompressed_stream<Point_2> in;
	in.open("targets.tpie");
	while (in.can_read()){
        targets.push_back(in.read());
	}
	in.close();

	return targets;
}

void SimpleShortestPathExperiment::writeResults(){

    std::ofstream file;
    file.open("../results/"+experimentName + ".csv", std::ios::trunc);

    std::string line;
    for (auto i = timings.begin(); i != timings.end(); ++i){
        line = (*i).title + "," + std::to_string((*i).duration.count()) + "\n";
        file << line;
    }

    file.close();
}
