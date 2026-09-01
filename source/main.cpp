#include "Renderer.h"

int main() {

    Renderer renderer(1920, 1080, 8, "../scenes/scene02.xml");
    renderer.Run();
    return 0;
}