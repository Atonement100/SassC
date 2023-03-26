# SassC
SassC is an Unreal Engine 5-based prototype of the classic Garry's Mod RTS "Sassilization." Previously this was a private project intended for build game development and Unreal Engine experience.

## Sassilization Background/History
[Sassilization](http://sassilization.com/) was a long-standing Garry's Mod-based community, with a number of servers hosting many gamemodes. Their hallmark game was also their namesake - Sassilization. Sassilization was a First Person RTS game which pitted up to 8 players against each other in small, focused maps. The game had a sort of medieval theme with swordsmen, archers, ballistas, catapults, scallywags (air-balloon archers). There was the ability to dynamically join and leave teams (with a maximum size of three) throughout the game as situations evolved. Bases included workshops to improve the units and buildings you may build, upgradable towers for automated defense, shrines to sacrifice units and remotely attack enemies with the power from the sacrifices, and shield monoliths, to nullify attacks from these shrines. The winner was the first to attain a certain sum of gold, which was passively generated for each city owned as well as activel generated from destroying opposing buildings. Gold was also one of the three primary resources, so there was a requirement to balance building bases and units with accumulating the required amount of gold.

This repository is a prototype for bringing Sassilization into a more permanent existence, since it has come and gone several times over the past decade. It is temporarily not being worked on by the repository owner because of internal indication by the Sassilization team that the mod is being reworked for release this year (2017). The goal of this project is not to compete with the original game, but to memorialize it when it's gone from the Garry's Mod community for good. Any release will be completely free.

## Current Features
All features are designed for and tested in a networked multiplayer environment.

- Server creation and player connection and disconnection
- Server browser with Steam integration for server finding as well as LAN searching
- City building and territory control

![Example of some building as well as territory requirements.](/readme_media/territory.gif)
- Dynamic wall and gate buildables

![Example of wall and gate buildables](/readme_media/walls.gif)
- Shield monolith buildable with dynamic linking

![Example of monoliths linking](/readme_media/shieldmono.gif)
- All buildings are constructable, including rotation for fine-tuning of base building

![Picture with all buildables](/readme_media/all-buildings.png)
- All units are spawnable, including rotation for ease of placement with larger units, such as the catapult and ballista

![Picture with all units](/readme_media/all-units.png)
- All units have their original models, textures, and animations.
- Units are rigged and have working animation state machines
- Unit combat of all sorts works and is stable over network, including destruction of buildables

![Example of combat, with building destruction](/readme_media/combat.gif)
- An untextured map, derivative of the original sa_orbit, has already been built for gameplay testing

## Backlog / Issue Tracker - Trello
The backlog, issue, and progress tracker can be found [on Trello](https://trello.com/b/TZ4kPoE7/sassc)

## Contributing
SassC is built on Unreal Engine 5.1. All other relevant content should be provided here.
