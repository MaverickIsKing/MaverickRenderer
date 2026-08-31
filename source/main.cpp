#include "Renderer.h"

int main() {

    Renderer renderer(1920, 1080, 1, "../scenes/scene01.xml");
    renderer.Run();
    return 0;
}