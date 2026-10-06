# A Generic Platformer Game
It's exactly what it sounds like: a 2d platformer game. Features very simple combat and precision-focused movement. Kill enemies and reach the shiny gold coin at the end of each level to continue.  
Made in a component-based custom 2D engine that utilizes Box2D for collisions, SDL3 for rendering, and FMOD for audio.  
Admittedly not my best work but it's something

<img width="1924" height="935" alt="Screenshot 2026-09-03 111727" src="https://github.com/user-attachments/assets/dc8ecec9-da19-4799-9d84-de4e9e5253c3" />

## Features
- Basic combat, including the ability to chain attacks (purely visual at the moment)
- Regularly spawning enemies that telegraph their attacks
- Knockback upon taking damage
- UI displaying health
- Sound effects for most interactions
<img width="1924" height="935" alt="Screenshot 2026-09-03 111703" src="https://github.com/user-attachments/assets/1242d2eb-a7d2-4550-b706-71f36708ed41" />

## Instructions
Download the current commit in branch main, open GameEngine.sln in Visual Studio 2022 or newer.  
Set configuration to Debug (preferably x64), set Game as startup project if necessary, and start without debugging.  
There is currently no proper release build, will add in the future if/when I get the game fully working.

## External Resources
Box2D: https://box2d.org/  
SDL3: https://github.com/libsdl-org/SDL/releases  
FMOD: https://www.fmod.com/  
Bfxr (sound effects): https://www.bfxr.net/  
Sprites were provided as class resources and as such I have no link for them

## Known Issues
Most of these issues can be chalked up to not having enough time to implement them
- The third level's goal is deliberately placed out of reach because the game crashes upon completing it, as there is no level 4 nor win state
- You can't restart the game after dying either
- Player and goal spawn positions are currently hard-coded (switch/case) because I couldn't get the program to fetch them after being read in from JSON
- Physics behave a lot differently at different screen resolutions

