#include "application.h"

int main(int argc, char *argv[])
{
    Application app(argc, argv, QStringLiteral("Tile Editor"));
    return app.run();
}
