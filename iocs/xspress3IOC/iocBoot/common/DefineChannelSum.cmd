

###############################################################################
# Plugin chains for summed channels with masking
###############################################################################

# Macros for the templates
epicsEnvSet("CHAN",   "ALL")
epicsEnvSet("CHM1",   "ALL")

# Per frame plugin chain (looks directly as Xspress driver)

# Channel mask plugin to mask individual channels (set them to zero)
XspressChannelMaskConfigure("$(PORT).CHANMASK", "$(NUM_CHANNELS)", "$(QSIZE)", 0, "XSP3", 0, 0, 0, 0, 0, 1)
dbLoadRecords("xspress3ChannelMask.template", "P=$(PREFIX), R=det1:MASK:, PORT=$(PORT).CHANMASK, NDARRAY_PORT=$(PORT)")

# ROI plugin to sum across all (non-zero) channels
NDROIConfigure("CHAN$(CHAN)", "$(QSIZE)", 0, "$(PORT).CHANMASK", 0, -1, -1)
dbLoadRecords("NDROI.template", ,"P=$(PREFIX), R=ROI$(CHAN):, PORT=CHAN$(CHAN), TIMEOUT=1, ADDR=0, NDARRAY_PORT=$(PORT), NDARRAY_ADDR=0, Enabled=0")

# Standard arrays for visualisation
NDStdArraysConfigure("MCA$(CHAN)", 5, 0, "CHAN$(CHAN)", 0, 0)
dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=MCA$(CHAN):,PORT=MCA$(CHAN),ADDR=0,TIMEOUT=1,NDARRAY_PORT=CHAN$(CHAN),TYPE=Float64,FTVL=DOUBLE,NELEMENTS=$(NUM_BINS)")


# Accumulation plugin chain (looks at PROC plugin)

# Channel mask plugin to mask individual channels (set them to zero)
XspressChannelMaskConfigure("$(PORT)SUM.CHANMASK", "$(NUM_CHANNELS)", "$(QSIZE)", 0, "PROC1", 0, 0, 0, 0, 0, 1)
dbLoadRecords("xspress3ChannelMask.template", "P=$(PREFIX), R=det1:MASKSUM:, PORT=$(PORT)SUM.CHANMASK, NDARRAY_PORT=$(PORT)")

# ROI plugin to sum across all (non-zero) channels
NDROIConfigure("CHANSUM$(CHAN)", "$(QSIZE)", 0, "$(PORT)SUM.CHANMASK", 0, -1, -1)
dbLoadRecords("NDROI.template", ,"P=$(PREFIX), R=ROISUM$(CHAN):, PORT=CHANSUM$(CHAN), TIMEOUT=1, ADDR=0, NDARRAY_PORT=PROC1, NDARRAY_ADDR=0, Enabled=0")

# Standard arrays for visualisation
NDStdArraysConfigure("MCASUM$(CHAN)", 5, 0, "CHANSUM$(CHAN)", 0, 0)
dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=MCASUM$(CHAN):,PORT=MCASUM$(CHAN),ADDR=0,TIMEOUT=1,NDARRAY_PORT=CHANSUM$(CHAN),TYPE=Float64,FTVL=DOUBLE,NELEMENTS=$(NUM_BINS)")


###############################################################################

