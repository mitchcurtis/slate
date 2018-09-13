#ifndef COMMANDS_H
#define COMMANDS_H

// These have to be unique, so it's better to keep them in one place.
enum {
    ApplyPixelEraserCommandId = 1,
    ApplyPixelFillCommandId,
    ApplyPixelLineCommandId,
    ApplyPixelPenCommandId,
    ApplyTileEraserCommandId,
    ApplyTileFillCommandId,
    ApplyTilePenCommandId
};

#endif // COMMANDS_H
