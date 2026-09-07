#include "Renderer.h"

int main() {

    Renderer renderer(1920, 1080, 3, 10, 1000, "../scenes/scene05.xml");
    renderer.Run();
    return 0;
}