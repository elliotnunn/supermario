I tried to get System 7.1 (Cube-E) running on top of a NewWorld ROM in
QEMU. It was really fun. Some observations follow...

The NewWorld ROM is recognised as a SuperMario ROM ($077D).

The Linked Patches are easier to work with than they look. Just add an
object file to LinkedPatchObjs that takes advantage of the
LinkedPatchMacros (InstallProc, PatchProc, leaResident...) and the
loader will make sure it becomes part of the System. Let there be no
doubt, the Linked Patches are an integral part of System 7. They should
be RE'd.

Relatively little of the on-disk System deals with hardware directly.
The ROM kinda-sorta acts as a hardware abstraction layer. Only it has
heaps of bugs and missing features that need to be worked around. For
example, I expected hardware interrupts to ruin everything but this
didn't happen.

MacsBug works! It's a bit nicer than spamming the NanoKernel log.
MacsBug did eventually start giving me nonsense explanations for
software exceptions. I thought this could be worked around by disabling
the DR emulator, but this seemingly had no effect.

Developing the System *and* the ROM at the same time must have been a
huge relief for the NewWorld devs. Being unable to edit the ROM directly
(because my build system wasn't set up) was very frustrating.

Things that I couldn't figure out: Balloon Help wouldn't install despite
much massaging, and this tripped up the Process Mgr. The global MenuList
was, at a later stage, getting corrupted and I couldn't figure that out.
Skipping over it, eventually the Memory Manager and Trap Dispatcher
would die.

It was great fun! Definitely worth revisiting later.
