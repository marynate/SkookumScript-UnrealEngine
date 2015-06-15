Welcome to the SkookumScript UE4 Plugin!
========================================

![Skookum UE4 Workflow](http://skookumscript.com/images/Unreal/SkookumUE4-Workflow_600.jpg)

[More screenshots can be found here](http://skookumscript.com/about/screens/)

What is it?
===========

SkookumScript is the world’s first full-featured programming language and tool suite dedicated to the creation of gameplay in video games. With key game concepts built directly into the language, SkookumScript enables video game developers to quickly and easily convert their ideas into awe-inspiring interactivity, level design and AI.

![SkookumScript IDE Screenshots](http://skookumscript.com/images/galleries/Screens.png)

The [SkookumScript team’s](http://skookumscript.com/about/team/) mission is to make SkookumScript the no-brainer industry standard for every genre of video game, on every platform, everywhere.

SkookumScript is the [culmination of over two decades](http://skookumscript.com/about/origin/) of problem-solving in the game industry trenches. Simple yet powerful, SkookumScript is a revelation to veteran programmers, yet readily accessible to those with no programming experience. Make no mistake—SkookumScript is a powerful language designed to deliver the performance demanded by professional game development. People who use SkookumScript—call them scripters, level designers or simply coders—are true programmers. They just have a great tool in the form of SkookumScript to make their job easier.

It is critically important to give people the power to unleash their creativity directly. Unlike the general-purpose languages commonly used to create games (such as C++, C# and Objective-C) and even non-game-specific scripting languages (such as Lua), SkookumScript allows people to focus on what they want to occur in a game, rather than spend most of their time describing to the computer how to do it. SkookumScript can turn anyone on a development team—yes, even producers—into a gameplay master.

------------------------------------------------------------------------------------

**Why another language?**
Why create a domain-specific language for video games? Domain-specific languages are available for many different fields: webpage layout, databases, geography, graphics, et cetera. However, there has never been a full-featured gameplay programming language and tool suite.

**Authoring gameplay and interactivity**
Often a video game’s gameplay / AI / mission design component is written from scratch, using general-purpose languages for each game title; some larger game studios may develop a limited proprietary scripting language and other gameplay tools. When a project’s gameplay tools are just starting to get interesting (not to mention more usable and less buggy) at the end of a project, many team members inevitably leave the company. The company usually does not reuse the tools because the people who understand them have left, and the tools were usually not designed for reuse anyway. Meanwhile, the departed team members cannot reuse the gameplay tools they created, since their old company retains ownership. This generally causes gameplay development tools to be reinvented for each project.

SkookumScript fills the “gap” of a reusable gameplay authoring tool.

------------------------------------------------------------------------------------

SkookumScript keeps developers “in flow” while being as unobtrusive as possible. Critically, it focuses on clarity of idea expression and rapid turn-around time (ideally live iteration). SkookumScript’s concurrency (multiple things happening at the same time) is truly unique—logical time-flow features are at the language level and are game-changers (in every sense) for authoring interactive content. Additional stand-out features of SkookumScript include built-in game concepts, sophisticated integrated development environment (IDE) and visual debugger, intensely useful script console, dynamic compile-time checking (including game data) with live updating, game-optimized speed and memory, secure “sandboxing” around gameplay commands, and [much more](http://skookumscript.com/about/features/). SkookumScript adapts in countless ways to a project’s setup—world editors, pipeline tools, processes, automation, smoke-tests and any other libraries or tech can be plugged in as desired.


Using SkookumScript
===================

SkookumScript can be used to develop any type of game and is compatible with all PC, console and mobile game systems. The SkookumScript Integrated Development Environment (IDE) connects to the game on its target platform and can interact with the game remotely—run commands, make changes and debug live as the game is running. Its dynamic and versatile visual tools scale to any team or project size, from tiny indie projects to massive studio productions—such as the award-winning open-world hit [Sleeping Dogs](http://www.sleepingdogs.net/) and the upcoming massively multiplayer [Triad Wars](http://www.unitedfrontgames.com/games/triad-wars/).

------------------------------------------------------------------------------------


SkookumScript Showcase: Sleeping Dogs
=====================================

'[Sleeping Dogs](http://www.sleepingdogs.net/) - Logo'Sleeping Dogs is an open-world, third-person game developed by Vancouver-based [United Front Games](http://www.unitedfrontgames.com/) - you are a deep-undercover police officer infiltrating Hong Kong’s notorious triads.

![Sleeping Dogs Portrait](http://skookumscript.com/images/SleepingDogsPortrait.jpg)

The project took four years—plus additional downloadable content (DLC)—with a 160-person team at its height. SkookumScript was its backbone; approximately 20 level-designers and scripters used SkookumScript as their primary development tool to create missions. The second-most-intensive group of users were the audio designers and composers, who used SkookumScript to add interactivity to the music and audio.

![SleepingDogs-sm1](http://skookumscript.com/images/about/SleepingDogs-sm1.jpg)

'Sleeping Dogs - Temple' Everyone on the team used SkookumScript daily to some degree: to jump to progression points in the game, to enable/disable various settings, to test game scenarios, and to run SkookumScript-authored automated smoke-tests for every check-in. Team members were constantly swapping useful SkookumScript code snippets to make their lives easier.

![SleepingDogs-sm7](http://skookumscript.com/images/about/SleepingDogs-sm7.jpg)

'Sleeping Dogs - Street'Sleeping Dogs was published in August 2012 by [Square Enix](http://www.square-enix.com/) on Microsoft Windows (online via Steam), Microsoft Xbox 360, Sony PlayStation 3 and Cloud OnLive. Through March 2013 it was frequently refreshed with new missions and other DLC created with SkookumScript.

It was followed up with Sleeping Dogs: Definitive Edition (2014) on Microsoft Xbox One and Sony PlayStation 4, and the massively multiplayer Triad Wars—also set in the Sleeping Dogs universe—is currently in beta.

The Sleeping Dogs team had a great experience using SkookumScript - check out [what they have to say about it](http://skookumscript.com/about/testimonials/). 

------------------------------------------------------------------------------------


Even more details
=================

SkookumScript is an “embedded scripting language” that works together with other common “low-level” game languages such as C++. SkookumScript is used for what it is best at—writing gameplay and other interactive logic. The “low-level” languages do what they are best at in a game engine—spewing graphics, crunching numbers for physics, applying audio effects, et cetera. It is easy to hook up SkookumScript to the game engine so the engine can call SkookumScript or vice versa. Also don’t let the “Script” part of “SkookumScript” give you any performance worries—SkookumScript is compiled (not interpreted) and is lightning-fast during compilation and when running the game.

SkookumScript is a pure object-oriented language (like [Smalltalk](http://en.wikipedia.org/wiki/Smalltalk)), though it is very easy to type simple-yet-powerful commands without ever needing to create a typical “class” infrastructure. It uses compile-time type-checking, and the types are inferred so you usually don’t have to specify them.

SkookumScript’s main components include:

- **Language** – the language itself (grammar, syntax, semantics, etc.) for the text-based code written in SkookumScript;
- **Libraries** – the commands and classes written in the language. This is generally split into the core/standard SkookumScript library, a company-wide/cross-project library, and libraries specific to a project. Libraries can take as long or longer to learn than the language itself—[Sleeping Dogs](http://skookumscript.com/about/#sleeping-dogs) had something like 8000 library commands and 1700 classes!
- **Compiler** – makes an optimized form of the code for the computer to read, and checks for any errors in the code;
- **Runtime** – the C++ library that is embedded as a part of the game engine to efficiently run SkookumScript code and call engine commands from SkookumScript code;
- **IDE (Integrated Development Environment)** – includes a code editor, class hierarchy browser, console (for running commands live), output and results log, visual debugger with stepping and breakpoints, and much more. The IDE can also call or be called by other favoured tools or pipeline utilities—even other editors and IDEs. It is versatile and powerful.


SkookumScript is evolving
=========================

SkookumScript is pretty fantastic already, and it is always being improved. The [Sleeping Dogs](http://skookumscript.com/about/#sleeping-dogs) team and our subsequent users have given us invaluable feedback, which has helped us make SkookumScript exponentially more awesome. Drop by the [SkookumScript Forum](http://skookumscript.com/forum/) to check out the latest suggestions for improvements to SkookumScript, and to make suggestions of your own.

------------------------------------------------------------------------------------

"The joke I make about evolving SkookumScript is that new versions will continue to be released until finally it is just two buttons – **[Make Game]** and **[Make Better]**."
[Conan Reis](http://skookumscript.com/about/team/#conan-reis) - Creator of SkookumScript

------------------------------------------------------------------------------------



Continue to [SkookumScript’s origin story](http://skookumscript.com/about/origin/)

Posted by [Conan Reis](http://skookumscript.com/about/team/#conan-reis) May 7th, 2015


Copyright © 2015 [Agog Labs Inc.](http://skookumscript.com/about/contact/)
