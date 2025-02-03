Main list of TODO:
- Software text blitting // done!
- Palette fading in RAM to allow for fading of text frame // done!
- Script engine // done!
- Character animation // done!
- Choices // done!
- Timed choices // logic done, just need graphics
- Text beeps
- Triggered SFX
- Intro world sounds
- Character animation layering
- Save data (straightforward to do but just bear in mind while making script engine)
- Title screen
- Credits animation
- Music room
- Better font (thicker, for better visibility on composite signal?)
- 2nd Character (portrait)
- CG mode
- vpunch/hpunch

Known Bugs:
- Transitioning between scene modes is not fully task safe, if the scene mode is destructed before the tasks complete, any transition should really be in a task, book-ended with a WaitForTasks() to prevent script progression until transition is complete. But this could get really fiddly if a transition doesn't coincide with a co_yield in the scene. This bug is easy enough to reproduce by clicking through text too fast (which also causes other glitches like palettes being missing)