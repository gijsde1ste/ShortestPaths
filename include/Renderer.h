#ifndef RENDERER_H
#define RENDERER_H

#include <definition.h>
#include <tpie/stack.h>
#include <deque.h>
#include <funnel.h>
#include <SDL2/SDL.h>
#include <Triangulation.h>

class Renderer
{
    public:
        Renderer();

        void drawStart();
        void clear();
        void drawPause();
        void drawEnd();

        void draw(funnel f, std::vector<Triangle> t);
        void drawFunnel(funnel f);
        void drawTriangulation(std::vector<Triangle> t);


    protected:

    private:
        SDL_Event event;
        SDL_Window * window;
        SDL_Renderer * renderer;
};

#endif // RENDERER_H
