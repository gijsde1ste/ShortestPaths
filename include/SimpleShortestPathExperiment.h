#ifndef SIMPLESHORTESTPATHEXPERIMENT_H
#define SIMPLESHORTESTPATHEXPERIMENT_H

#include <deque.h>
#include <Triangulation.h>
#include <internal_triangulation.h>
#include <funnel.h>
#include <internal_funnel.h>
#include <chrono>
#include <tpie/file_base_crtp.h>


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

    private:
};

#endif // SIMPLESHORTESTPATHEXPERIMENT_H
