#include "Renderer.h"

int main() {

    Renderer renderer(1920, 1080, 3, 10, 500, "../scenes/scene14_1.xml");
    renderer.Run();
    return 0;
}