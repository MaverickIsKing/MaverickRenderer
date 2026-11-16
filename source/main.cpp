#include "Renderer.h"

int main() {

    Renderer renderer(1920, 1080, 3, 8, 2, "../scenes/scene19_2.xml");
    renderer.Run();
    return 0;
}