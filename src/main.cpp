#include <definition.h>

#include <deque.h>
#include <Triangulation.h>
#include <funnel.h>

#include <Renderer.h>

int main() {
	tpie::tpie_init();

	size_t available_memory_mb = 128;
	tpie::get_memory_manager().set_limit(available_memory_mb*1024*1024);

    Triangulation t;
    t.open("128.tpie");

    Point_2 target = t.getRandomPoint();
    std::cout << "Target: " << target.x << " " << target.y  << std::endl;
    t.createPath(target);

    funnel f = funnel(t.getNextEdge());
    while (!t.finished()){
        f.extend(t.getNextEdge());
    }

    //std::vector<Triangle> triangles = t.copyPolygon();

    t.close();

    //Renderer renderer;
    //renderer.draw(f, triangles);

    tpie::tpie_finish();

    std::cout << "Finished" << std::endl;

	return 0;
}

