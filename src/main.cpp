#include <iostream>
#include <exception>

#include "Application.h"

int main() {
    vge::Application app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}