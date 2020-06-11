#include "Renderer.h"

Renderer::Renderer()
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 960, 0);
    renderer = SDL_CreateRenderer(window, -1 , 0);
}

void Renderer::draw(funnel f, std::vector<Triangle> t){
    drawStart();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (auto i = t.begin(); i != t.end(); i++){
        SDL_RenderDrawLine(renderer, (*i).a.x * 300, (*i).a.y * 300, (*i).b.x * 300, (*i).b.y * 300);
        SDL_RenderDrawLine(renderer, (*i).b.x * 300, (*i).b.y * 300, (*i).c.x * 300, (*i).c.y * 300);
        SDL_RenderDrawLine(renderer, (*i).c.x * 300, (*i).c.y * 300, (*i).a.x * 300, (*i).a.y * 300);
    }


    tpie::stack<Point_2> s = f.getStack();

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    Point_2 p1 = s.pop();
    Point_2 p2;
    while (!s.empty()){

        p2 = s.pop();
        SDL_RenderDrawLine(renderer, p1.x * 300, p1.y * 300, p2.x * 300, p2.y * 300);
        p1 = p2;
    }

    /*SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    tpie::deque<Point_2> d = f.getCusp();
    p1 = d.popFront();
    while (!d.empty()){
        p2 = d.popFront();
        SDL_RenderDrawLine(renderer, p1.x * 300, p1.y * 300, p2.x * 300, p2.y * 300);
        p1 = p2;
    }*/



    //drawFunnel(f);
    //drawTriangulation(t);

    drawEnd();
}

void Renderer::drawStart(){
    clear();

    // Draw with black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void Renderer::clear(){
    // Clear screen white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void Renderer::drawPause(){
    SDL_RenderPresent(renderer);

    while( true ){
        SDL_Delay(10);
        SDL_PollEvent(&event);
        if (event.type == SDL_KEYDOWN) {
            break;
        }

        if (event.type == SDL_QUIT){
            drawEnd();
            break;
        }
    }
}

void Renderer::drawEnd(){
    SDL_RenderPresent(renderer);

    while( true ){
        SDL_Delay(10);
        SDL_PollEvent(&event);

        if (event.type == SDL_QUIT){
            break;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
