# SassC
SassC is an Unreal Engine 4-based prototype of the classic Garry's Mod RTS "Sassilization." Previously this was a private project intended for build game development and Unreal Engine experience.

# Sassilization History
[Sassilization](http://sassilization.com/) was a long-standing Garry's Mod-based community, with a number of servers hosting many gamemodes. Their hallmark game was also their namesake - Sassilization. Sassilization was a First Person RTS game which pitted up to 8 players against each other in small, focused maps. The game had a sort of medieval theme with swordsmen, archers, ballistas, catapults, scallywags (air-balloon archers). There was the ability to dynamically join and leave teams (with a maximum size of three) throughout the game as situations evolved. Bases included workshops to improve the units and buildings you may build, upgradable towers for automated defense, shrines to sacrifice units and remotely attack enemies with the power from the sacrifices, and shield monoliths, to nullify attacks from these shrines. The winner was the first to attain a certain sum of gold, which was passively generated for each city owned as well as activel generated from destroying opposing buildings. Gold was also one of the three primary resources, so there was a requirement to balance building bases and units with accumulating the required amount of gold.

This repository is a prototype for bringing Sassilization into a more permanent existence, since it has come and gone several times over the past decade. It is temporarly not being worked on by the repository owner because of internal indication by the Sassilization team that the mod is being reworked for release this year. The goal of this project is not to compete with the original game, but to memorialize it when it's gone for good.

# Current Features
All features are designed for and tested in a networked multiplayer environment.

Server creation, connection, and disconnection
Server browser with Steam integration for server finding as well as LAN searching
City building and territory control
Dynamic wall and gate construction
Shield monolith construction and dynamic linking
All buildings are buildable, including rotation for micro-management of base building
All units are spawnable, including rotation for ease with larger units, such as the catapult and ballista
Unit combat of all sorts works and is stable over network, including destruction of bases
