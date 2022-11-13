# invoke SourceDir generated makefile for Lab2Idle.p28L
Lab2Idle.p28L: .libraries,Lab2Idle.p28L
.libraries,Lab2Idle.p28L: package/cfg/Lab2Idle_p28L.xdl
	$(MAKE) -f C:\Users\jbate\workspace_v10\PotBot/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\jbate\workspace_v10\PotBot/src/makefile.libs clean

