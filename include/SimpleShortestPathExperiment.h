#ifndef SIMPLESHORTESTPATHEXPERIMENT_H
#define SIMPLESHORTESTPATHEXPERIMENT_H

#include <deque.h>
#include <Triangulation.h>
#include <internal_triangulation.h>
#include <funnel.h>
#include <internal_funnel.h>
#include <chrono>
#include <tpie/file_base_crtp.h>
#include <iostream>
#include <fstream>

struct Timing{
    std::chrono::microseconds duration;
    std::string title;

    Timing(std::chrono::time_point<std::chrono::high_resolution_clock> a, std::chrono::time_point<std::chrono::high_resolution_clock> b, std::string t){
        duration = std::chrono::duration_cast<std::chrono::microseconds>(b - a);
        title = t;
    }
};

class SimpleShortestPathExperiment
{
    public:
        SimpleShortestPathExperiment();
        void run(int argc, char** argv);
        void generateTargets(std::string file);
        void testIOFunnel(std::string file, std::vector<Point_2> targets);
        void testInternalFunnel(std::string file, std::vector<Point_2> targets);
        std::vector<Point_2> getTargets();

    protected:
        void writeResults();

    private:
        std::vector<Timing> timings;
        std::string experimentName;
};

#endif // SIMPLESHORTESTPATHEXPERIMENT_H
