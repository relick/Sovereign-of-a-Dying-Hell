Objective: build a small adventure game.
Thoughts:
How does movement happen? It could be linear VN-type, it could be via explicit movement (+ map?), it could be a way to integrate a dungeon-type exploration. 

Requirements:
- Rooms with background, and interactibles able to be placed in them
- Dialogue system with full-screen character portraits
- Rooms have some kind of programmatic connection
- Flags and inventory
- Item usage
- Music and sound effects
- Map and menus

Thinking: Even that is maybe a bit much at once? I'm better off just trying to make a very simple dialogue, character and background system with music. Then it'll be a lot easier to build more into it and hopefully with more ideas.

Requirements:
- Rooms, have background + palette.
- Characters, can be half or full screen. Need to be able to change expression.
- Dialogue system, needs nice font, text box, and ability to change characters/expressions/rooms/music through the dialogue.
- Music and sound effects.
- Intro and title screen.

After all that's done, write a little script for it!

// NOTES:
Priority ordering in MD:
1. BG Colour
2. Plane B
3. Plane A
4. Sprites
5. Window Plane
6. Plane B (priority)
7. Plane A (priority)
8. Sprites (priority)
9. Window Plane (priority)

So for general VN stuff, I think it makes sense to standardise around -
Visual area:
BG = Plane B, PAL0
BG sprites = Sprites, any palette
Characters = Plane A (priority). Char1 = PAL1, Char2 = PAL2
Effects/fade = Sprites (priority), PAL3. Balance with BG sprites (e.g. make them disappear during fades)

Text area:
Frame + text = Window Plane, PAL3
Effects/fade = Sprites (priority), PAL3