#ifndef SPTEXPERIMENT_H
#define SPTEXPERIMENT_H

#include <SimpleShortestPathExperiment.h>
#include <SptInternalTriangulation.h>
#include <SptInternal.h>


class SptExperiment: public SimpleShortestPathExperiment
{
    public:
        void run(int argc, char** argv);
        void testIOFunnel(std::string file);
        void testInternalFunnel(std::string file);

    protected:

    private:
};

#endif // SPTEXPERIMENT_H
