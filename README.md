# Time rewind mechanic exercise

## How it works:

Every client is recording a timeline of world snapshots, saving one snapshot each frame.

Starting and stopping a rewind is server-authorative, but the rewind happens on each client for a smooth experience. Unreal's actor replication is temporarily disabled during a rewind, since each client must be in control of its own actors.

During a rewind a cursor is scrubbed back through the timeline, sampling and restoring snapshots each frame. A snapshot includes, for each actor subject to rewinding:

* Actor transform
* Camera orientation
* Time of death
* AI state
* Animation pose

When a rewind is stopped, the server sends the last snapshot it restored to all of the clients, to keep things in-sync and prevent drift. The clients add this snapshot to their timelines and discard anything that would come after it.

## Snapshotting animation

Since the animation blueprint holds an immense amount of state (state machine current nodes and blending weights, timeline positions in animation montages, etc.) and it's not easily serializable in its entirety (lots of sub-objects, would have to use `->Serialize()` recursively, no guarantees the resulting object would even work after de-serialization, performance, etc.), I opted to save only the final computed pose each frame.

When restoring the pose, I would feed it to the AnimInstance each frame and use a state machine in the anim BP to transition and blend between the usual animation and my snapshot.

A problem with this approach is that any state machine in the anim BP needs to be "pure" and eventually resolve to the same pose given the same snapshotted inputs (like actor position and movement). That is, the state machines should be stateless (funny, I know). For a simple locomotion cycle like the default unreal mannequin's, this is not a problem.

Similarily, the enemy AI's behavior tree must fulfill the same stateless-ness requirement: Given the same inputs (actor position and blackboard state), it should do the same thing. We can't use internally stateful tasks, like, for example wait nodes or non-interruptible tasks.

## Project structure:

* RewindManager.cpp: Client-side rewind and timeline logic (instantiated in PersiaGameState)
* PersiaGameState.cpp: Handles rewind starting and stopping, game pausing
* RewindableCharacter.cpp: Snapshotting logic, character death
* RewindableAnimInstance.cpp / ABP_Manny: Animation snapshotting
* PersiaPlayerCharacter.cpp: Input, camera snapshotting
* PersiaEnemyCharacter.cpp: AI state snapshotting
* BT_EnemyPatrol: Enemy AI behavior tree
* PersiaGameMode.cpp: Game over conditions
* M_Mannequin: Dissolve shader


## Misc other behaviors:

* The game only pauses when either the players won (killed all enemies) or when all players died.
* There's a dissolve shader going on when killing enemies.
* When you get killed, you get ragdolled. I didn't have the time to snapshot the ragdoll's state as well, so it will just snap back on revive.
* UI is reasonably animated.
* AI is implemented as a behavior tree that does an EQS query in front of the enemy actor.
* Enemies get revived as well (this wasn't specified in the brief, but I thought this would make most sense. I think it's easy to change).
* Game pause is not real world pause because of [an engine bug](https://forums.unrealengine.com/t/multiplayer-pause/81893/4).
