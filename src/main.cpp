#include <definition.h>

#include <deque.h>
#include <Triangulation.h>
#include <funnel.h>
#include <internal_deque.h>
#include <tpie/internal_stack.h>

#include <Renderer.h>

void testDeque(){
    tpie::deque<double> d = tpie::deque<double>();

    for (int i = 0; i < 5242880; i++){
        d.pushFront(i);
    }

    for (int i = 0; i < 5242880; i++){
        d.popBack();
    }

    std::cout << d.size() << std::endl;
}

void testStack(){
    tpie::internal_stack<double> s = tpie::internal_stack<double>(5242880);

    for (int i = 0; i < 5242880; i++){
        s.push(i);
    }

    for (int i = 0; i < 5242880; i++){
        s.pop();
    }

    std::cout << s.size() << std::endl;
}

int main() {
	tpie::tpie_init();

	size_t available_memory_mb = 20;
	tpie::get_memory_manager().set_limit(available_memory_mb*1024*1024);

    testStack();

    /*Triangulation t;
    t.open("128.tpie");

    Point_2 target = t.getRandomPoint();
    std::cout << "Target: " << target.x << " " << target.y  << std::endl;
    t.createPath(target);

    funnel f = funnel(t.getRoot(), t.getNextEdge());
    while (!t.finished()){
        f.extend(t.getNextEdge());
    }
    f.extendFinalStep(target);

    std::vector<Triangle> triangles = t.copyPolygon();

    t.close();

    std::cout << "Path is " << f.getStack().size() << " points long" << std::endl;
    //Renderer renderer;
    //renderer.draw(f, triangles);
    */
    tpie::tpie_finish();

    std::cout << "Finished" << std::endl;

	return 0;
}

