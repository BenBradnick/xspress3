epicsEnvSet("CHAN",   "ALL")
epicsEnvSet("CHM1",   "ALL")

dbpf("$(PREFIX)ROI$(CHAN):MinY",    "$(CHM1)")
dbpf("$(PREFIX)ROISUM$(CHAN):MinY", "$(CHM1)")

dbpf("$(PREFIX)ROI$(CHAN):EnableX", "Enable")
dbpf("$(PREFIX)ROI$(CHAN):EnableY", "Enable")
dbpf("$(PREFIX)ROI$(CHAN):EnableZ", "Disable")
dbpf("$(PREFIX)ROI$(CHAN):SizeY", "1")
dbpf("$(PREFIX)ROISUM$(CHAN):EnableX", "Enable")
dbpf("$(PREFIX)ROISUM$(CHAN):EnableY", "Enable")
dbpf("$(PREFIX)ROISUM$(CHAN):SizeY", "1")

dbpf("$(PREFIX)det1:MASK:EnableCallbacks", "Enable")
dbpf("$(PREFIX)det1:MASKSUM:EnableCallbacks", "Enable")
dbpf("$(PREFIX)MCA$(CHAN):EnableCallbacks", "Enable")
dbpf("$(PREFIX)ROI$(CHAN):EnableCallbacks", "Enable")
dbpf("$(PREFIX)MCA$(CHAN):EnableCallbacks", "Enable")
dbpf("$(PREFIX)ROISUM$(CHAN):EnableCallbacks", "Enable")
dbpf("$(PREFIX)MCASUM$(CHAN):EnableCallbacks", "Enable")

dbpf("$(PREFIX)MCA$(CHAN):ArrayData.LOPR", 0.5)
