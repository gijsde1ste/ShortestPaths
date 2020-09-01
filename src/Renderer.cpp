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


    tpie::stack<Point_2> *s = f.getStack();

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    Point_2 p1 = s->pop();
    Point_2 p2;
    while (!s->empty()){

        p2 = s->pop();
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

void Renderer::draw(std::vector<Triangle> t, SptInternal * spt){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (auto i = t.begin(); i != t.end(); i++){
        SDL_RenderDrawLine(renderer, (*i).a.x * 300, (*i).a.y * 300, (*i).b.x * 300, (*i).b.y * 300);
        SDL_RenderDrawLine(renderer, (*i).b.x * 300, (*i).b.y * 300, (*i).c.x * 300, (*i).c.y * 300);
        SDL_RenderDrawLine(renderer, (*i).c.x * 300, (*i).c.y * 300, (*i).a.x * 300, (*i).a.y * 300);
    }

    int count = 0;
    while (count < spt->targetCount()){

        std::vector<Point_2> path = spt->queryTarget(count);

        Point_2 p1 = path.back();
        path.pop_back();

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        while (!path.empty()){

            Point_2 p2 = path.back();
            path.pop_back();

            SDL_RenderDrawLine(renderer, p1.x * 300, p1.y * 300, p2.x * 300, p2.y * 300);
            p1 = p2;
        }

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect rect = {p1.x * 300 - 5, p1.y * 300 - 5, 10, 10};
        SDL_RenderFillRect(renderer, &rect);

        count++;
    }
}

void Renderer::draw(SparseShortestPathTree * sspt, std::vector<Triangle> t, std::vector<Point_2> targets, std::vector<DegreeThreeNode> splitVertices){

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (auto i = t.begin(); i != t.end(); i++){
        SDL_RenderDrawLine(renderer, (*i).a.x * 300, (*i).a.y * 300, (*i).b.x * 300, (*i).b.y * 300);
        SDL_RenderDrawLine(renderer, (*i).b.x * 300, (*i).b.y * 300, (*i).c.x * 300, (*i).c.y * 300);
        SDL_RenderDrawLine(renderer, (*i).c.x * 300, (*i).c.y * 300, (*i).a.x * 300, (*i).a.y * 300);
    }


    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect rect;
    for (auto i = targets.begin(); i != targets.end(); i++){
        rect = {i->x * 300 - 5, i->y * 300 - 5, 10, 10};
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    for (auto i = splitVertices.begin(); i != splitVertices.end(); i++){
        rect = {i->vertex.x * 300 - 5, i->vertex.y * 300 - 5, 10, 10};
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    tpie::uncompressed_stream<ssptResultPoint> in;
    in.open("result.tpie");

    while(in.can_read()){
        ssptResultPoint srp = in.read();
        std::cout << srp.point.x << " " << srp.point.y << " " << srp.parent << std::endl;
        if (srp.parent != -1){
            Point_2 p1 = srp.point;
            tpie::stream_size_type offset = in.offset();
            in.seek(srp.parent);
            ssptResultPoint srp2 = in.read();
            Point_2 p2 = srp2.point;
            in.seek(offset);

            SDL_RenderDrawLine(renderer, p1.x * 300, p1.y * 300, p2.x * 300, p2.y * 300);
        }
    }

    /*if (sspt->geodesic->size() > 0){
        Point_2 p1 = sspt->geodesic->pop();
        Point_2 p2;

        while(!sspt->geodesic->empty()){
            p2 = sspt->geodesic->pop();
            SDL_RenderDrawLine(renderer, p1.x * 300, p1.y * 300, p2.x * 300, p2.y * 300);
            p1 = p2;
        }
    }*/

    /*SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    std::cout << "stacks: " << sspt->stacks.size() << " Deques: " << sspt->cusps.size() << std::endl;
    for (auto i = sspt->stacks.begin(); i != sspt->stacks.end(); i++){
        std::cout << (*i)->size() << std::endl;
        if ((*i)->size() > 1){
            Point_2 p1 = (*i)->pop();
            Point_2 p2;
            while(!(*i)->empty()){
                p2 = (*i)->pop();
                SDL_RenderDrawLine(renderer, p1.x * 300, p1.y * 300, p2.x * 300, p2.y * 300);
                p1 = p2;
            }
        }
    }
    std::cout << " ---- " << std::endl;
    for (auto i = sspt->cusps.begin(); i != sspt->cusps.end(); i++){
        std::cout << (*i)->size() << std::endl;
    }*/
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
        SDL_Delay(1);
        SDL_PollEvent(&event);

        if (event.type == SDL_QUIT){
            break;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
