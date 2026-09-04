#include "Renderer.h"

int main() {

    Renderer renderer(1920, 1080, 16, "../scenes/scene03.xml");
    renderer.Run();
    return 0;
}