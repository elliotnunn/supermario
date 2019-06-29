#   File:       count.make
#   Target:     count
#   Sources:    Count.c
#   Created:    Monday, September 17, 1990 10:13:20 AM


OBJS = Count.c.o



count ƒƒ count.make {OBJS}
	Link -d -c '????' -t APPL ∂
		{OBJS} ∂
		#"{CLibraries}"CSANELib.o ∂
		#"{CLibraries}"Math.o ∂
		#"{CLibraries}"Complex.o ∂
		"{CLibraries}"StdClib.o ∂
		"{MPW}"Libraries:Libraries:SIOW.o ∂
		"{Libraries}"Runtime.o ∂
		"{Libraries}"Interface.o ∂
		-o count
Count.c.o ƒ count.make Count.c
	 C -r  Count.c
count ƒƒ "{MPW}"Interfaces:Rincludes:SIOW.r
	Rez -a "{MPW}"Interfaces:Rincludes:SIOW.r -o count
