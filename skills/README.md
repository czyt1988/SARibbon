# Agent skills shipped with SARibbon

This directory hosts installable agent skills: self-contained instruction
files that teach an AI coding agent how to use SARibbon correctly — the
integration steps, the API surface, and the failure modes that are easy to
hit and hard to diagnose.

These skills are **for users of the library**, not for developing SARibbon
itself. They follow the common `SKILL.md` convention (a directory with a
YAML frontmatter `name`/`description`) and work with any agent runtime that
loads that format.

## Install

Copy the skill directory into your agent's skills location — for the
common `~/.agents/skills` layout:

```bash
cp -r skills/saribbon-qml ~/.agents/skills/
```

For a project-scoped installation (shared with everyone working in the
repository), place it under `.agents/skills/` at the project root instead.
Restart the agent session (or trigger its skill reload) after copying.

## Contents

| Skill | Scope |
|---|---|
| `saribbon-qml` | Embedding the QML module (`import SARibbon 3.0`): CMake wiring, type registration, panel/button/gallery composition, dynamic gallery filling, theme bridging, the collapsed-preview embedding contract, frameless windows, the Qt 5.15 compatibility lane, and a testing cookbook. |

The widgets front end is not covered by a skill yet; see `examples/widgets`
and the docs site for that side.
