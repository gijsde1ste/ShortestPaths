#ifndef RENDERER_H
#define RENDERER_H

#include <definition.h>
#include <tpie/stack.h>
#include <deque.h>
#include <funnel.h>
#include <internal_deque.h>
#include <SDL2/SDL.h>
#include <Triangulation.h>
#include <SparseShortestPathTree.h>
#include <SptInternal.h>

class Renderer
{
    public:
        Renderer();

        void drawStart();
        void clear();
        void drawPause();
        void drawEnd();

        void draw(funnel f, std::vector<Triangle> t);
        void draw(SparseShortestPathTree * sspt, std::vector<Triangle> t, std::vector<Point_2> targets, std::vector<DegreeThreeNode> splitVertices);
        void draw(std::vector<Triangle> t, SptInternal * spt);

    protected:

    private:
        SDL_Event event;
        SDL_Window * window;
        SDL_Renderer * renderer;
};

#endif // RENDERER_H
