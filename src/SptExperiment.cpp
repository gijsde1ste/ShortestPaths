#include "SptExperiment.h"


void SptExperiment::run(int argc, char** argv){
    // handle some arguments
    experimentName = "test";
    if (argc > 2) experimentName = argv[2];

    std::string file = experimentName + ".tpie";
    experimentName = "Spt"+experimentName;

    // mode 0 is I/O, 1 is internal
    int mode = 0;
    if (argc > 3) mode = std::stoi(argv[3]);

	switch (mode){
        case 0:
            experimentName += "IO";
            testIOFunnel(file);
            writeResults();
            break;
        case 1:
            experimentName += "Internal";
            testInternalFunnel(file);
            writeResults();
            break;
        default:
            experimentName += "Internal";
            testInternalFunnel(file);
            writeResults();
            break;
    }
}


void SptExperiment::testIOFunnel(std::string file){

}

void SptExperiment::testInternalFunnel(std::string file){

    std::chrono::microseconds duration;
    auto startTotal = std::chrono::high_resolution_clock::now();

    auto startOpeningFile = std::chrono::high_resolution_clock::now();
    SptInternalTriangulation t;
    t.open(file);
    auto stopOpeningFile = std::chrono::high_resolution_clock::now();

    auto startFunnel = std::chrono::high_resolution_clock::now();
    Node n = t.getRoot();
    t.setPathProgress(n.postOrder);

    SptInternal spt(t.size());
    spt.extendStart(n, t.getNextEdge());
    while(!t.finished()){
        spt.extend(t.getNextEdge());
    }
    auto stopFunnel = std::chrono::high_resolution_clock::now();

    auto startQuery = std::chrono::high_resolution_clock::now();
    spt.generateRandomTargets(1000);
    auto stopQuery = std::chrono::high_resolution_clock::now();

    /* Visualise backup

    Renderer r;
    r.drawStart();
    r.draw(t.copyPolygon(), &spt);
    r.drawEnd();

    */

    auto stopTotal = std::chrono::high_resolution_clock::now();

    timings.push_back(Timing(startOpeningFile, stopOpeningFile, "OpeningFile"));
    timings.push_back(Timing(startFunnel, stopFunnel, "Funnel"));
    timings.push_back(Timing(startQuery, stopQuery, "Query"));
    timings.push_back(Timing(startTotal, stopTotal, "Total"));
}
