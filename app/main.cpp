#include "application.h"

int main(int argc, char *argv[])
{
    Application app(argc, argv, QStringLiteral("Slate"));
    return app.run();
}
