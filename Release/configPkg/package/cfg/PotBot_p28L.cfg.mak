# invoke SourceDir generated makefile for PotBot.p28L
PotBot.p28L: .libraries,PotBot.p28L
.libraries,PotBot.p28L: package/cfg/PotBot_p28L.xdl
	$(MAKE) -f C:\Users\jbate\workspace_v10\PotBot/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\jbate\workspace_v10\PotBot/src/makefile.libs clean

